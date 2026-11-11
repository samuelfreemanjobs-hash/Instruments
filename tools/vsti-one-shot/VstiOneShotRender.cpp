#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_processors/format/juce_AudioPluginFormatManagerHelpers.h>
#include <juce_core/juce_core.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

namespace
{

constexpr int kBlockSize = 512;

double dbToGain (double db)
{
    return std::pow (10.0, db / 20.0);
}

float bufferPeak (const juce::AudioBuffer<float>& buf, int start, int end)
{
    float peak = 0.0f;
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
    {
        const float* data = buf.getReadPointer (ch);
        for (int i = start; i < end; ++i)
            peak = std::max (peak, std::abs (data[i]));
    }
    return peak;
}

juce::AudioBuffer<float> mixToMono (const juce::AudioBuffer<float>& stereo)
{
    juce::AudioBuffer<float> mono (1, stereo.getNumSamples());
    mono.clear();
    if (stereo.getNumChannels() <= 1)
    {
        mono.copyFrom (0, 0, stereo, 0, 0, stereo.getNumSamples());
        return mono;
    }
    for (int i = 0; i < stereo.getNumSamples(); ++i)
    {
        float s = 0.0f;
        for (int ch = 0; ch < stereo.getNumChannels(); ++ch)
            s += stereo.getSample (ch, i);
        mono.setSample (0, i, s / static_cast<float> (stereo.getNumChannels()));
    }
    return mono;
}

juce::AudioBuffer<float> trimAndNormalize (juce::AudioBuffer<float> buf,
                                           double tailSilenceDb,
                                           double normalizePeakDb)
{
    const float thresh = static_cast<float> (dbToGain (tailSilenceDb));
    const int n = buf.getNumSamples();
    int start = 0;
    int end = n;

    while (start < end && bufferPeak (buf, start, start + 1) < thresh)
        ++start;
    while (end > start && bufferPeak (buf, end - 1, end) < thresh)
        --end;

    if (end <= start)
    {
        buf.setSize (buf.getNumChannels(), 1);
        buf.clear();
        return buf;
    }

    juce::AudioBuffer<float> trimmed (buf.getNumChannels(), end - start);
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
        trimmed.copyFrom (ch, 0, buf, ch, start, end - start);

    float peak = 0.0f;
    for (int ch = 0; ch < trimmed.getNumChannels(); ++ch)
    {
        const float* data = trimmed.getReadPointer (ch);
        for (int i = 0; i < trimmed.getNumSamples(); ++i)
            peak = std::max (peak, std::abs (data[i]));
    }

    if (peak > 0.0f)
    {
        const float target = static_cast<float> (dbToGain (normalizePeakDb));
        const float gain = target / peak;
        trimmed.applyGain (gain);
    }

    return trimmed;
}

struct RenderDefaults
{
    double sampleRate = 44100.0;
    int velocity = 100;
    bool mono = true;
    double normalizePeakDb = -1.0;
    double tailSilenceDb = -60.0;
    double maxCaptureSec = 8.0;
};

struct RenderItem
{
    juce::String slotId;
    juce::String vst3Path;
    int program = 0;
    int rootMidi = 60;
    double maxCaptureSec = 0.0; // 0 → use defaults
};

RenderDefaults parseDefaults (const juce::var& root)
{
    RenderDefaults d;
    if (auto* obj = root.getDynamicObject())
    {
        if (obj->hasProperty ("defaults"))
        {
            auto* defs = obj->getProperty ("defaults").getDynamicObject();
            if (defs != nullptr)
            {
                d.sampleRate = defs->getProperty ("sample_rate");
                d.velocity = static_cast<int> (defs->getProperty ("velocity"));
                d.mono = defs->getProperty ("channels").toString() == "mono";
                d.normalizePeakDb = defs->getProperty ("normalize_peak_db");
                d.tailSilenceDb = defs->getProperty ("tail_silence_db");
                d.maxCaptureSec = defs->getProperty ("max_capture_sec");
            }
        }
    }
    return d;
}

std::vector<RenderItem> parseRenders (const juce::var& root, juce::String& error)
{
    std::vector<RenderItem> items;
    auto* obj = root.getDynamicObject();
    if (obj == nullptr || ! obj->hasProperty ("renders"))
    {
        error = "job missing renders array";
        return items;
    }

    auto* arr = obj->getProperty ("renders").getArray();
    if (arr == nullptr || arr->isEmpty())
    {
        error = "renders array empty";
        return items;
    }

    for (const auto& v : *arr)
    {
        auto* r = v.getDynamicObject();
        if (r == nullptr)
            continue;

        RenderItem item;
        item.slotId = r->getProperty ("slot_id").toString();
        item.vst3Path = r->getProperty ("vst3_path").toString();
        item.program = static_cast<int> (r->getProperty ("program"));
        item.rootMidi = static_cast<int> (r->getProperty ("root_midi"));
        if (r->hasProperty ("max_capture_sec"))
            item.maxCaptureSec = static_cast<double> (r->getProperty ("max_capture_sec"));

        if (item.slotId.isEmpty() || item.vst3Path.isEmpty())
        {
            error = "render entry missing slot_id or vst3_path";
            return {};
        }
        if (item.rootMidi < 0 || item.rootMidi > 127)
        {
            error = "invalid root_midi for slot " + item.slotId;
            return {};
        }
        items.push_back (std::move (item));
    }
    return items;
}

juce::AudioPluginFormat* findVst3Format (juce::AudioPluginFormatManager& manager)
{
    for (auto* format : manager.getFormats())
        if (format->getName() == "VST3")
            return format;
    return nullptr;
}

std::unique_ptr<juce::AudioPluginInstance> loadPlugin (juce::AudioPluginFormatManager& manager,
                                                       const juce::File& vst3File,
                                                       double sampleRate,
                                                       juce::String& error)
{
    auto* format = findVst3Format (manager);
    if (format == nullptr)
    {
        error = "VST3 format not available (build with JUCE_PLUGINHOST_VST3=1)";
        return nullptr;
    }

    if (! vst3File.exists())
    {
        error = "VST3 not found: " + vst3File.getFullPathName();
        return nullptr;
    }

    juce::OwnedArray<juce::PluginDescription> types;
    format->findAllTypesForFile (types, vst3File.getFullPathName());

    if (types.isEmpty())
    {
        error = "No plugin types in " + vst3File.getFullPathName();
        return nullptr;
    }

    return format->createInstanceFromDescription (*types.getFirst(), sampleRate, kBlockSize, error);
}

juce::AudioBuffer<float> renderNote (juce::AudioPluginInstance& plugin,
                                     int midiNote,
                                     int velocity,
                                     double sampleRate,
                                     double maxCaptureSec)
{
    plugin.setNonRealtime (true);
    plugin.prepareToPlay (sampleRate, kBlockSize);

    const int maxSamples = static_cast<int> (std::ceil (maxCaptureSec * sampleRate));
    juce::AudioBuffer<float> capture (2, maxSamples);
    capture.clear();

    juce::AudioBuffer<float> block (2, kBlockSize);
    juce::MidiBuffer midi;
    bool noteOnSent = false;
    bool noteOffSent = false;
    int written = 0;

    const float silenceThresh = static_cast<float> (dbToGain (-55.0));
    int silentBlocksAfterOn = 0;
    constexpr int silentBlocksToStop = 24;

    while (written < maxSamples)
    {
        const int numThisBlock = std::min (kBlockSize, maxSamples - written);
        block.setSize (2, numThisBlock, false, false, true);
        block.clear();
        midi.clear();

        if (! noteOnSent)
        {
            midi.addEvent (juce::MidiMessage::noteOn (1, midiNote, static_cast<juce::uint8> (velocity)), 0);
            noteOnSent = true;
        }

        plugin.processBlock (block, midi);

        for (int ch = 0; ch < 2; ++ch)
            capture.copyFrom (ch, written, block, ch, 0, numThisBlock);

        if (bufferPeak (block, 0, numThisBlock) < silenceThresh)
            ++silentBlocksAfterOn;
        else
            silentBlocksAfterOn = 0;

        if (noteOnSent && silentBlocksAfterOn >= silentBlocksToStop && written > static_cast<int> (sampleRate * 0.05))
        {
            if (! noteOffSent)
            {
                midi.clear();
                midi.addEvent (juce::MidiMessage::noteOff (1, midiNote), 0);
                noteOffSent = true;
                plugin.processBlock (block, midi);
                for (int ch = 0; ch < 2; ++ch)
                    capture.copyFrom (ch, written, block, ch, 0, numThisBlock);
            }
            break;
        }

        written += numThisBlock;
    }

    capture.setSize (2, written, true, true, true);
    return capture;
}

bool writeWav (const juce::File& outFile, const juce::AudioBuffer<float>& buffer, double sampleRate)
{
    outFile.getParentDirectory().createDirectory();
    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> stream (outFile.createOutputStream());
    if (stream == nullptr)
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer (
        wav.createWriterFor (stream.get(), sampleRate, static_cast<unsigned int> (buffer.getNumChannels()),
                             24, {}, 0));

    if (writer == nullptr)
        return false;

    stream.release();
    return writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
}

int listPrograms (const juce::String& vst3Path)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    juce::AudioPluginFormatManager formatManager;
    addDefaultFormatsToManager (formatManager);

    juce::String error;
    auto plugin = loadPlugin (formatManager, juce::File (vst3Path), 44100.0, error);
    if (plugin == nullptr)
    {
        std::cerr << error << '\n';
        return 1;
    }

    juce::DynamicObject::Ptr root (new juce::DynamicObject());
    root->setProperty ("vst3_path", vst3Path);
    root->setProperty ("num_programs", plugin->getNumPrograms());
    root->setProperty ("plugin_name", plugin->getName());

    juce::Array<juce::var> programs;
    for (int i = 0; i < plugin->getNumPrograms(); ++i)
    {
        juce::DynamicObject::Ptr p (new juce::DynamicObject());
        p->setProperty ("index", i);
        p->setProperty ("name", plugin->getProgramName (i));
        programs.add (juce::var (p.get()));
    }
    root->setProperty ("programs", programs);

    std::cout << juce::JSON::toString (juce::var (root.get())) << std::endl;
    return 0;
}

int runJob (const juce::File& jobFile)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    const auto jsonText = jobFile.loadFileAsString();
    const auto parsed = juce::JSON::parse (jsonText);
    if (parsed.isVoid())
    {
        std::cerr << "Invalid job JSON\n";
        return 1;
    }

    auto* jobObj = parsed.getDynamicObject();
    if (jobObj == nullptr || ! jobObj->hasProperty ("output_dir"))
    {
        std::cerr << "Job missing output_dir\n";
        return 1;
    }

    const juce::File outputDir (jobObj->getProperty ("output_dir").toString());
    const RenderDefaults defaults = parseDefaults (parsed);

    juce::String parseError;
    const auto items = parseRenders (parsed, parseError);
    if (! parseError.isEmpty())
    {
        std::cerr << parseError << '\n';
        return 1;
    }

    juce::AudioPluginFormatManager formatManager;
    addDefaultFormatsToManager (formatManager);

    juce::String currentPath;
    std::unique_ptr<juce::AudioPluginInstance> plugin;
    juce::Array<juce::var> manifestEntries;

    for (const auto& item : items)
    {
        if (item.vst3Path != currentPath)
        {
            currentPath = item.vst3Path;
            plugin.reset();
            juce::String error;
            plugin = loadPlugin (formatManager, juce::File (currentPath), defaults.sampleRate, error);
            if (plugin == nullptr)
            {
                std::cerr << error << '\n';
                return 1;
            }
        }

        const int numPrograms = plugin->getNumPrograms();
        const int programIndex = numPrograms > 0 ? juce::jlimit (0, numPrograms - 1, item.program) : 0;
        plugin->setCurrentProgram (programIndex);

        const double captureSec = item.maxCaptureSec > 0.0 ? item.maxCaptureSec : defaults.maxCaptureSec;
        auto captured = renderNote (*plugin, item.rootMidi, defaults.velocity, defaults.sampleRate, captureSec);
        if (defaults.mono)
        {
            auto mono = mixToMono (captured);
            captured = std::move (mono);
        }

        captured = trimAndNormalize (captured, defaults.tailSilenceDb, defaults.normalizePeakDb);

        const juce::File wavOut = outputDir.getChildFile (item.slotId + ".wav");
        if (! writeWav (wavOut, captured, defaults.sampleRate))
        {
            std::cerr << "Failed to write " << wavOut.getFullPathName() << '\n';
            return 1;
        }

        juce::DynamicObject::Ptr entry (new juce::DynamicObject());
        entry->setProperty ("slot_id", item.slotId);
        entry->setProperty ("wav", wavOut.getFullPathName());
        entry->setProperty ("program", programIndex);
        entry->setProperty ("program_name", plugin->getProgramName (programIndex));
        entry->setProperty ("root_midi", item.rootMidi);
        entry->setProperty ("duration_sec", captured.getNumSamples() / defaults.sampleRate);
        manifestEntries.add (juce::var (entry.get()));

        std::cout << "Rendered " << item.slotId << " -> " << wavOut.getFileName() << '\n';
    }

    juce::DynamicObject::Ptr manifest (new juce::DynamicObject());
    manifest->setProperty ("version", 1);
    manifest->setProperty ("count", manifestEntries.size());
    manifest->setProperty ("samples", manifestEntries);

    const juce::File manifestFile = outputDir.getChildFile ("manifest.json");
    manifestFile.replaceWithText (juce::JSON::toString (juce::var (manifest.get()), true));

    std::cout << "Wrote manifest: " << manifestFile.getFullPathName() << '\n';
    return 0;
}

int runPreview (const juce::String& outPath,
                const juce::String& vst3Path,
                int program,
                int midiNote,
                int velocity,
                double maxCaptureSec,
                double sampleRate)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    juce::AudioPluginFormatManager formatManager;
    addDefaultFormatsToManager (formatManager);

    juce::String error;
    auto plugin = loadPlugin (formatManager, juce::File (vst3Path), sampleRate, error);
    if (plugin == nullptr)
    {
        std::cerr << error << '\n';
        return 1;
    }

    const int numPrograms = plugin->getNumPrograms();
    const int programIndex = numPrograms > 0 ? juce::jlimit (0, numPrograms - 1, program) : 0;
    plugin->setCurrentProgram (programIndex);

    auto captured = renderNote (*plugin, midiNote, velocity, sampleRate, maxCaptureSec);
    auto mono = mixToMono (captured);
    const juce::File outFile (outPath);
    if (! writeWav (outFile, mono, sampleRate))
    {
        std::cerr << "Preview write failed\n";
        return 1;
    }
    return 0;
}

void printUsage()
{
    std::cerr << "VstiOneShotRender — headless VST3 one-shot WAV renderer\n"
              << "  --list-programs <path.vst3>   JSON catalog to stdout\n"
              << "  --job <job.json>              Render all entries (strict schema)\n"
              << "  --preview <out.wav> <vst3> <program> <midiNote> [velocity] [maxSec] [sampleRate]\n";
}

} // namespace

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    const juce::String cmd (argv[1]);

    if (cmd == "--list-programs" && argc >= 3)
        return listPrograms (argv[2]);

    if (cmd == "--job" && argc >= 3)
        return runJob (juce::File (argv[2]));

    if (cmd == "--preview" && argc >= 6)
    {
        const juce::String outPath (argv[2]);
        const juce::String vst3Path (argv[3]);
        const int program = std::atoi (argv[4]);
        const int midiNote = std::atoi (argv[5]);
        const int velocity = argc > 6 ? std::atoi (argv[6]) : 100;
        const double maxSec = argc > 7 ? std::atof (argv[7]) : 1.5;
        const double sampleRate = argc > 8 ? std::atof (argv[8]) : 44100.0;
        return runPreview (outPath, vst3Path, program, midiNote, velocity, maxSec, sampleRate);
    }

    printUsage();
    return 1;
}
