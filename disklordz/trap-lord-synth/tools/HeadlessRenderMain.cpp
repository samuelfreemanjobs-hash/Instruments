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

bool writeWav24 (const juce::File& file, const juce::AudioBuffer<float>& buffer, double sampleRate)
{
    file.getParentDirectory().createDirectory();
    std::unique_ptr<juce::FileOutputStream> stream (file.createOutputStream());
    if (! stream)
        return false;

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::AudioFormatWriter> writer (
        wav.createWriterFor (stream.get(), sampleRate, (unsigned int) buffer.getNumChannels(), 24, {}, 0));
    if (! writer)
        return false;
    stream.release();
    return writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
}

void printUsage()
{
    std::cerr
        << "TrapLordSynthRender — headless offline renders\n"
        << "  auditions <output_dir>              2 WAV per category (24 total)\n"
        << "  audition <preset_index> <wav_path>  single preset audition\n"
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
            const auto timeline = OfflineAudition::buildTimeline (swellMod);
            juce::AudioBuffer<float> audio;
            if (! proc.renderOfflineTimeline (idx, timeline, OfflineAudition::kSampleRate, OfflineAudition::kDurationSec,
                                               OfflineAudition::kBpm, audio))
            {
                std::cerr << "Render failed preset " << idx << "\n";
                return kExitFail;
            }
            const auto file = root.getChildFile (OfflineAudition::safeFilename (cat, preset.name, slot + 1));
            if (! writeWav24 (file, audio, OfflineAudition::kSampleRate))
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

int runSingleAudition (int presetIndex, const juce::File& wavFile)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    const bool swellMod = isSwellAudition (presetIndex);
    const auto timeline = OfflineAudition::buildTimeline (swellMod);
    juce::AudioBuffer<float> audio;
    if (! proc.renderOfflineTimeline (presetIndex, timeline, OfflineAudition::kSampleRate, OfflineAudition::kDurationSec,
                                       OfflineAudition::kBpm, audio))
        return kExitFail;
    return writeWav24 (wavFile, audio, OfflineAudition::kSampleRate) ? kExitOk : kExitFail;
}

int runMpcExport (int presetIndex, const juce::File& parentDir)
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    TrapLordSynthAudioProcessor proc;
    proc.loadFactoryPreset (presetIndex);
    const auto& preset = PresetLibrary::instance().get (presetIndex);

    KeygroupExportOptions opts;
    opts.programName = preset.name.replaceCharacters ("/\\:*?\"<>|", "_______").substring (0, 48);
    opts.lowNote = 48;
    opts.highNote = 72;
    opts.semitoneStep = 6;
    opts.velocityLayers = 2;
    opts.bodySeconds = 2.f;
    opts.releaseTailSeconds = 0.5f;
    opts.use24Bit = true;

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

    if (command == "mpc")
    {
        if (argc < 4)
            return kExitFail;
        return runMpcExport (std::atoi (argv[2]), juce::File (argv[3]));
    }

    printUsage();
    return kExitFail;
}
