#include "PluginProcessor.h"
#include "Golden/GoldenScenarios.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace
{
class OfflinePlayHead final : public juce::AudioPlayHead
{
public:
    void set (double bpmIn, double ppqIn) noexcept
    {
        bpm = bpmIn;
        ppq = ppqIn;
    }

    void advance (int numSamples, double sampleRate) noexcept
    {
        ppq += (static_cast<double> (numSamples) / sampleRate) * (bpm / 60.0);
    }

    Optional<PositionInfo> getPosition() const override
    {
        PositionInfo info;
        info.setBpm (bpm);
        info.setPpqPosition (ppq);
        info.setIsPlaying (true);
        return info;
    }

private:
    double bpm = 120.0;
    double ppq = 0.0;
};

void printUsage()
{
    std::cerr << "Usage:\n"
              << "  JunovaOfflineRender <output.wav> [program] [midiNote] [velocity] [seconds] [sampleRate]\n"
              << "  JunovaOfflineRender <output.wav> --scenario <id> [midiNote] [velocity] [seconds] [sampleRate]\n"
              << "  Scenarios: " << junovax::golden::listScenarioIds() << "\n"
              << "  Defaults: program=0 note=60 velocity=100 seconds=0.5 sampleRate=44100\n"
              << "  Golden renders force arpRate=0 except ab05-arp-sync (uses host PPQ).\n";
}

struct RenderConfig
{
    int program = 0;
    const char* scenario = nullptr;
    int midiNote = 60;
    int velocity = 100;
    double seconds = 0.5;
    double sampleRate = 44100.0;
    bool arpHostSync = false;
};

bool parseArgs (int argc, char** argv, RenderConfig& cfg)
{
    if (argc < 2)
        return false;

    if (argc >= 4 && juce::String (argv[2]) == "--scenario")
    {
        cfg.scenario = argv[3];
        cfg.midiNote = argc > 4 ? std::atoi (argv[4]) : 60;
        cfg.velocity = argc > 5 ? std::atoi (argv[5]) : 100;
        cfg.seconds = argc > 6 ? std::atof (argv[6]) : 0.5;
        cfg.sampleRate = argc > 7 ? std::atof (argv[7]) : 44100.0;
        cfg.arpHostSync = juce::String (cfg.scenario) == "ab05-arp-sync";
        if (juce::String (cfg.scenario) == "ab04-filter-sweep" && argc <= 6)
            cfg.seconds = 2.0;
        return true;
    }

    cfg.program = argc > 2 ? std::atoi (argv[2]) : 0;
    cfg.midiNote = argc > 3 ? std::atoi (argv[3]) : 60;
    cfg.velocity = argc > 4 ? std::atoi (argv[4]) : 100;
    cfg.seconds = argc > 5 ? std::atof (argv[5]) : 0.5;
    cfg.sampleRate = argc > 6 ? std::atof (argv[6]) : 44100.0;
    return true;
}

void applyDeterministicOverrides (JunovaXAudioProcessor& junova, const RenderConfig& cfg)
{
    auto& apvts = junova.getApvts();
    if (cfg.arpHostSync)
        return;

    auto zero = [&apvts] (const char* id)
    {
        if (auto* p = apvts.getParameter (id))
            p->setValueNotifyingHost (0.0f);
    };
    zero (junovax::ParameterIDs::arpRate);
    zero (junovax::ParameterIDs::drift);
    zero (junovax::ParameterIDs::dcoNoise);
}
} // namespace

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    RenderConfig cfg;
    if (! parseArgs (argc, argv, cfg))
    {
        printUsage();
        return 1;
    }

    if (cfg.seconds <= 0.0 || cfg.sampleRate <= 0.0)
    {
        std::cerr << "Invalid duration or sample rate\n";
        return 1;
    }

    const juce::File outFile (argv[1]);

    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());
    if (processor == nullptr)
    {
        std::cerr << "Failed to create Junova-X processor\n";
        return 1;
    }

    auto* junova = dynamic_cast<JunovaXAudioProcessor*> (processor.get());
    if (junova == nullptr)
    {
        std::cerr << "Processor type mismatch\n";
        return 1;
    }

    OfflinePlayHead playHead;
    playHead.set (120.0, 0.0);
    processor->setPlayHead (&playHead);

    constexpr int blockSize = 512;
    processor->prepareToPlay (cfg.sampleRate, blockSize);

    if (cfg.scenario != nullptr)
    {
        if (! junova->applyGoldenScenario (cfg.scenario))
        {
            std::cerr << "Unknown scenario: " << cfg.scenario << "\n";
            return 1;
        }
    }
    else
    {
        processor->setCurrentProgram (cfg.program);
    }

    applyDeterministicOverrides (*junova, cfg);

    const int totalSamples = static_cast<int> (std::ceil (cfg.seconds * cfg.sampleRate));
    const int noteOffSample = std::max (1, static_cast<int> (0.85 * static_cast<double> (totalSamples)));

    juce::AudioBuffer<float> buffer (2, blockSize);
    juce::MidiBuffer midi;
    juce::AudioBuffer<float> capture (2, totalSamples);
    capture.clear();

    for (int offset = 0; offset < totalSamples; offset += blockSize)
    {
        const int numThisBlock = std::min (blockSize, totalSamples - offset);
        buffer.setSize (2, numThisBlock, false, false, true);
        buffer.clear();
        midi.clear();

        if (offset == 0)
            midi.addEvent (juce::MidiMessage::noteOn (1, cfg.midiNote, static_cast<juce::uint8> (cfg.velocity)), 0);

        if (cfg.arpHostSync && offset == 0)
        {
            midi.addEvent (juce::MidiMessage::noteOn (1, 60, static_cast<juce::uint8> (cfg.velocity)), 0);
            midi.addEvent (juce::MidiMessage::noteOn (1, 64, static_cast<juce::uint8> (cfg.velocity)), 0);
            midi.addEvent (juce::MidiMessage::noteOn (1, 67, static_cast<juce::uint8> (cfg.velocity)), 0);
        }

        if (offset <= noteOffSample && noteOffSample < offset + numThisBlock)
        {
            midi.addEvent (juce::MidiMessage::noteOff (1, cfg.midiNote), noteOffSample - offset);
            if (cfg.arpHostSync)
            {
                midi.addEvent (juce::MidiMessage::noteOff (1, 60), noteOffSample - offset);
                midi.addEvent (juce::MidiMessage::noteOff (1, 64), noteOffSample - offset);
                midi.addEvent (juce::MidiMessage::noteOff (1, 67), noteOffSample - offset);
            }
        }

        processor->processBlock (buffer, midi);
        playHead.advance (numThisBlock, cfg.sampleRate);

        for (int ch = 0; ch < 2; ++ch)
            capture.copyFrom (ch, offset, buffer, ch, 0, numThisBlock);
    }

    outFile.getParentDirectory().createDirectory();
    if (outFile.existsAsFile())
        outFile.deleteFile();

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> stream (outFile.createOutputStream());
    if (stream == nullptr)
    {
        std::cerr << "Cannot open output file\n";
        return 1;
    }

    std::unique_ptr<juce::AudioFormatWriter> writer (
        wav.createWriterFor (stream.get(), cfg.sampleRate, static_cast<unsigned int> (capture.getNumChannels()),
                             24, {}, 0));

    if (writer == nullptr)
    {
        std::cerr << "Failed to create WAV writer\n";
        return 1;
    }

    stream.release();
    if (! writer->writeFromAudioSampleBuffer (capture, 0, totalSamples))
    {
        std::cerr << "WAV write failed\n";
        return 1;
    }

    std::cout << "Wrote " << totalSamples << " samples to " << outFile.getFullPathName() << '\n';
    return 0;
}
