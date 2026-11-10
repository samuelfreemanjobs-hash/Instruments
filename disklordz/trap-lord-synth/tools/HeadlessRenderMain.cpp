#include <JuceHeader.h>
#include "../Source/OfflineAudition.h"
#include "../Source/PluginProcessor.h"
#include "../Source/PresetLibrary.h"
#include "../Source/KeygroupExportOptions.h"
#include <iostream>
#include <map>

namespace
{
constexpr int kExitOk = 0;
constexpr int kExitFail = 1;

bool writeWavFile (const juce::File& file,
                 const juce::AudioBuffer<float>& buffer,
                 double sampleRate,
                 int bitsPerSample)
{
    file.getParentDirectory().createDirectory();
    file.deleteFile();

    juce::AudioFormatWriterOptions opts;
    opts = opts.withSampleRate (sampleRate)
             .withNumChannels ((int) buffer.getNumChannels())
             .withBitsPerSample (bitsPerSample);

    opts = opts.withSampleFormat (juce::AudioFormatWriterOptions::SampleFormat::integral);

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::OutputStream> stream (file.createOutputStream());
    if (! stream)
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer (wav.createWriterFor (stream, opts));
    if (! writer)
        return false;

    const bool ok = writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
    writer.reset();
    return ok;
}

int runDiagnose (const juce::File& outDir)
{
    outDir.createDirectory();
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;

    struct Case
    {
        const char* name;
        int kind;
        int note;
    };
    const Case cases[] = { { "init_saw", 0, 60 }, { "sine", 1, 60 }, { "middle_c", 0, 60 } };

    for (const auto& c : cases)
    {
        proc.loadDiagnosticPatch (c.kind);
        juce::AudioBuffer<float> audio (2, int (OfflineAudition::kSampleRate * 3.));
        if (! proc.renderNoteOffline (c.note, 110, audio, OfflineAudition::kSampleRate))
            return kExitFail;
        const auto file = outDir.getChildFile (juce::String (c.name) + ".wav");
        if (! writeWavFile (file, audio, OfflineAudition::kSampleRate, 24))
            return kExitFail;
        std::cout << "Diagnose wrote " << file.getFileName() << "\n";
    }
    return kExitOk;
}

void printUsage()
{
    std::cerr
        << "TrapLordSynthRender — headless offline renders\n"
        << "  auditions <output_dir> [manifest.json]  category audition WAVs\n"
        << "  audition <preset_index> <wav_path>  single preset audition\n"
        << "  diagnose <output_dir>               init saw / sine / middle C checks\n"
        << "  test-note <midi> <wav> [preset]     single-note render\n"
        << "  preset-probe <index> <wav>          factory preset middle-C probe\n"
        << "  category-probe <index> <wav>        category phrase + chord probe\n"
        << "  category-probe-json <preset.json> <wav>  probe without bank index\n"
        << "  mpc <preset_index> <parent_dir> [layers]  MPC keygroup export\n";
}

int phraseVariantForParams (const juce::var& params)
{
    const int engine = int (params.getProperty ("engine", 0));
    const int chord = int (params.getProperty ("chord", 0));
    const int filterType = int (params.getProperty ("filterType", 0));
    return juce::jlimit (0, 5, (engine * 2 + chord + filterType) % 6);
}

bool isSwellAudition (int presetIndex)
{
    const auto& p = PresetLibrary::instance().get (presetIndex);
    if (p.category == "Swells")
        return true;
    return static_cast<float> (p.params.getProperty ("swellDepth", 0.)) > 0.25f;
}

int runAuditions (const juce::String& outDir, const juce::File& manifestFile)
{
    const juce::File root (outDir);
    root.createDirectory();

    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    proc.prepareToPlay (OfflineAudition::kSampleRate, 512);

    juce::StringArray order {
        "Pads", "Leads", "Synth Keys", "Swells", "Brass", "Strings", "Church Keys/EP",
        "Organ", "Bells", "Bass", "Choir", "Hits/FX"
    };

    std::map<juce::String, std::vector<int>> byCat;
    for (int i = 0; i < PresetLibrary::instance().count(); ++i)
        byCat[PresetLibrary::instance().get (i).category].push_back (i);

    struct AuditionSlot
    {
        juce::String category;
        int index = 0;
        int slot = 1;
    };
    std::vector<AuditionSlot> slots;

    if (manifestFile.existsAsFile())
    {
        const auto parsed = juce::JSON::parse (manifestFile);
        if (parsed.isObject() && parsed.hasProperty ("auditions"))
        {
            for (const auto& item : *parsed.getProperty ("auditions", {}).getArray())
            {
                AuditionSlot s;
                s.category = item.getProperty ("category", "").toString();
                s.index = int (item.getProperty ("index", 0));
                s.slot = int (item.getProperty ("slot", 1));
                slots.push_back (s);
            }
        }
    }

    if (slots.empty())
    {
        for (const auto& cat : order)
        {
            const auto it = byCat.find (cat);
            if (it == byCat.end())
                continue;
            const auto& indices = it->second;
            for (int s = 0; s < 2 && s < (int) indices.size(); ++s)
                slots.push_back ({ cat, indices[size_t (s)], s + 1 });
        }
    }

    int written = 0;
    for (const auto& slot : slots)
    {
        const int idx = slot.index;
        const auto& preset = PresetLibrary::instance().get (idx);
        const bool swellMod = isSwellAudition (idx);
        const auto timeline =
            OfflineAudition::buildTimeline (preset.category, swellMod, phraseVariantForParams (preset.params));
        juce::AudioBuffer<float> audio;
        if (! proc.renderOfflineTimeline (idx, timeline, OfflineAudition::kSampleRate, OfflineAudition::kDurationSec,
                                           OfflineAudition::kBpm, audio))
        {
            std::cerr << "Render failed preset " << idx << "\n";
            return kExitFail;
        }
        const auto file = root.getChildFile (OfflineAudition::safeFilename (preset.category, preset.name, slot.slot));
        if (! writeWavFile (file, audio, OfflineAudition::kSampleRate, 24))
        {
            std::cerr << "Write failed " << file.getFullPathName() << "\n";
            return kExitFail;
        }
        std::cout << "Wrote " << file.getFileName() << " (#" << idx << " " << preset.name << ")"
                  << (swellMod ? " [mod wheel]" : "") << "\n";
        ++written;
    }

    std::cout << "Auditions total: " << written << "\n";
    return written >= 1 ? kExitOk : kExitFail;
}

int runCategoryProbeJson (const juce::File& jsonFile, const juce::File& wavFile)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    const auto parsed = juce::JSON::parse (jsonFile);
    if (! parsed.isObject())
        return kExitFail;
    const juce::String category = parsed.getProperty ("category", "Pads").toString();
    PresetLibrary::instance().applyParamsObject (proc.state, parsed);
    const float swellDepth = float (parsed.getProperty ("swellDepth", 0.));
    const bool swellMod = category == "Swells" || swellDepth > 0.25f;
    const auto timeline = OfflineAudition::buildTimeline (category, swellMod, phraseVariantForParams (parsed));
    juce::AudioBuffer<float> audio;
    constexpr double kCategoryProbeSec = 5.5;
    if (! proc.renderOfflineTimeline (-1, timeline, OfflineAudition::kSampleRate, kCategoryProbeSec,
                                       OfflineAudition::kBpm, audio))
        return kExitFail;
    return writeWavFile (wavFile, audio, OfflineAudition::kSampleRate, 24) ? kExitOk : kExitFail;
}

int runCategoryProbe (int presetIndex, const juce::File& wavFile)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    const auto& preset = PresetLibrary::instance().get (presetIndex);
    const bool swellMod = isSwellAudition (presetIndex);
    const auto timeline =
        OfflineAudition::buildTimeline (preset.category, swellMod, phraseVariantForParams (preset.params));
    juce::AudioBuffer<float> audio;
    constexpr double kCategoryProbeSec = 5.5;
    if (! proc.renderOfflineTimeline (presetIndex, timeline, OfflineAudition::kSampleRate, kCategoryProbeSec,
                                       OfflineAudition::kBpm, audio))
        return kExitFail;
    return writeWavFile (wavFile, audio, OfflineAudition::kSampleRate, 24) ? kExitOk : kExitFail;
}

int runPresetProbe (int presetIndex, const juce::File& wavFile)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    proc.loadFactoryPreset (presetIndex);
    const double sr = OfflineAudition::kSampleRate;
    juce::AudioBuffer<float> audio (2, int (sr * 3.5));
    if (! proc.renderNoteOffline (60, 110, audio, sr, 2.0f))
        return kExitFail;
    return writeWavFile (wavFile, audio, sr, 24) ? kExitOk : kExitFail;
}

int runTestNote (int note, const juce::File& wavFile, int presetIndex)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    proc.loadFactoryPreset (presetIndex);
    juce::AudioBuffer<float> audio (2, int (OfflineAudition::kSampleRate * 3.));
    if (! proc.renderNoteOffline (note, 110, audio, OfflineAudition::kSampleRate))
        return kExitFail;
    return writeWavFile (wavFile, audio, OfflineAudition::kSampleRate, 24) ? kExitOk : kExitFail;
}

int runSingleAudition (int presetIndex, const juce::File& wavFile)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    const auto& preset = PresetLibrary::instance().get (presetIndex);
    const bool swellMod = isSwellAudition (presetIndex);
    const auto timeline =
        OfflineAudition::buildTimeline (preset.category, swellMod, phraseVariantForParams (preset.params));
    juce::AudioBuffer<float> audio;
    if (! proc.renderOfflineTimeline (presetIndex, timeline, OfflineAudition::kSampleRate, OfflineAudition::kDurationSec,
                                       OfflineAudition::kBpm, audio))
        return kExitFail;
    return writeWavFile (wavFile, audio, OfflineAudition::kSampleRate, 24) ? kExitOk : kExitFail;
}

int runMpcExport (int presetIndex, const juce::File& parentDir, int velocityLayers)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    proc.loadFactoryPreset (presetIndex);
    const auto& preset = PresetLibrary::instance().get (presetIndex);

    KeygroupExportOptions opts;
    opts.programName = preset.name;
    opts.folderPrefix = "DL_";
    opts.lowNote = 36;
    opts.highNote = 96;
    opts.semitoneStep = 1;
    opts.velocityLayers = juce::jlimit (1, 4, velocityLayers);
    opts.useCategoryTiming = true;
    opts.use24Bit = false;
    opts.maxProgramMegabytes = 120.f;

    std::atomic<bool> abort { false };
    const bool ok = proc.exportMpcKeygroupOffline (parentDir, opts, abort, nullptr);
    std::cout << (ok ? "MPC export OK\n" : "MPC export failed\n");
    return ok ? kExitOk : kExitFail;
}
} // namespace

int main (int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return kExitFail;
    }

    const juce::String command (argv[1]);

    if (command == "auditions")
    {
        if (argc < 3)
        {
            printUsage();
            return kExitFail;
        }
        const juce::File manifest = argc > 3 ? juce::File (argv[3]) : juce::File();
        return runAuditions (argv[2], manifest);
    }

    if (command == "audition")
    {
        if (argc < 4)
            return kExitFail;
        return runSingleAudition (std::atoi (argv[2]), juce::File (argv[3]));
    }

    if (command == "test-note")
    {
        if (argc < 4)
            return kExitFail;
        const int preset = argc >= 5 ? std::atoi (argv[4]) : 0;
        return runTestNote (std::atoi (argv[2]), juce::File (argv[3]), preset);
    }

    if (command == "mpc")
    {
        if (argc < 4)
            return kExitFail;
        const int layers = argc > 4 ? std::atoi (argv[4]) : 2;
        return runMpcExport (std::atoi (argv[2]), juce::File (argv[3]), layers);
    }

    if (command == "category-probe-json")
    {
        if (argc < 4)
            return kExitFail;
        return runCategoryProbeJson (juce::File (argv[2]), juce::File (argv[3]));
    }

    if (command == "category-probe")
    {
        if (argc < 4)
            return kExitFail;
        return runCategoryProbe (std::atoi (argv[2]), juce::File (argv[3]));
    }

    if (command == "preset-probe")
    {
        if (argc < 4)
            return kExitFail;
        return runPresetProbe (std::atoi (argv[2]), juce::File (argv[3]));
    }

    if (command == "diagnose")
    {
        if (argc < 3)
            return kExitFail;
        return runDiagnose (juce::File (argv[2]));
    }

    printUsage();
    return kExitFail;
}
