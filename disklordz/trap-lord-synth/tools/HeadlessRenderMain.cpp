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
        << "  auditions <output_dir>              2 WAV per category (24 total)\n"
        << "  audition <preset_index> <wav_path>  single preset audition\n"
        << "  diagnose <output_dir>               init saw / sine / middle C checks\n"
        << "  test-note <midi> <wav> [preset]     single-note render\n"
        << "  mpc <preset_index> <parent_dir>     MPC keygroup export\n";
}

bool isSwellAudition (int presetIndex)
{
    const auto& p = PresetLibrary::instance().get (presetIndex);
    if (p.category == "Swells")
        return true;
    return static_cast<float> (p.params.getProperty ("swellDepth", 0.)) > 0.25f;
}

int runAuditions (const juce::String& outDir)
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

    int written = 0;
    for (const auto& cat : order)
    {
        const auto it = byCat.find (cat);
        if (it == byCat.end())
            continue;
        const auto& indices = it->second;
        for (int slot = 0; slot < 2 && slot < (int) indices.size(); ++slot)
        {
            const int idx = indices[slot];
            const auto& preset = PresetLibrary::instance().get (idx);
            const bool swellMod = isSwellAudition (idx);
            const auto timeline = OfflineAudition::buildTimeline (cat, swellMod);
            juce::AudioBuffer<float> audio;
            if (! proc.renderOfflineTimeline (idx, timeline, OfflineAudition::kSampleRate, OfflineAudition::kDurationSec,
                                               OfflineAudition::kBpm, audio))
            {
                std::cerr << "Render failed preset " << idx << "\n";
                return kExitFail;
            }
            const auto file = root.getChildFile (OfflineAudition::safeFilename (cat, preset.name, slot + 1));
            if (! writeWavFile (file, audio, OfflineAudition::kSampleRate, 24))
            {
                std::cerr << "Write failed " << file.getFullPathName() << "\n";
                return kExitFail;
            }
            std::cout << "Wrote " << file.getFileName() << " (#" << idx << " " << preset.name << ")"
                      << (swellMod ? " [mod wheel]" : "") << "\n";
            ++written;
        }
    }

    std::cout << "Auditions total: " << written << "\n";
    return written == 24 ? kExitOk : kExitFail;
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
    const auto timeline = OfflineAudition::buildTimeline (preset.category, swellMod);
    juce::AudioBuffer<float> audio;
    if (! proc.renderOfflineTimeline (presetIndex, timeline, OfflineAudition::kSampleRate, OfflineAudition::kDurationSec,
                                       OfflineAudition::kBpm, audio))
        return kExitFail;
    return writeWavFile (wavFile, audio, OfflineAudition::kSampleRate, 24) ? kExitOk : kExitFail;
}

int runMpcExport (int presetIndex, const juce::File& parentDir)
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
    opts.velocityLayers = 2;
    opts.bodySeconds = 1.15f;
    opts.releaseTailSeconds = 0.35f;
    opts.use24Bit = false;
    opts.ampReleaseSeconds = 0.35f;
    opts.maxProgramMegabytes = 48.f;

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
        return runAuditions (argv[2]);
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
        return runMpcExport (std::atoi (argv[2]), juce::File (argv[3]));
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
