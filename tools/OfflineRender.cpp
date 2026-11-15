#include "PluginProcessor.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

namespace
{

void renderMidiToBuffer (juce::AudioProcessor& processor,
                         bool hostCallsPrepare,
                         double sampleRate,
                         int program,
                         int midiNote,
                         int velocity,
                         double seconds,
                         juce::AudioBuffer<float>& capture)
{
    constexpr int blockSize = 512;

    if (hostCallsPrepare)
        processor.prepareToPlay (sampleRate, blockSize);

    processor.setCurrentProgram (program);

    const int totalSamples = static_cast<int> (std::ceil (seconds * sampleRate));
    const int noteOffSample = std::max (1, static_cast<int> (0.85 * static_cast<double> (totalSamples)));

    capture.setSize (2, totalSamples, false, false, true);
    capture.clear();

    juce::AudioBuffer<float> buffer (2, blockSize);
    juce::MidiBuffer midi;

    for (int offset = 0; offset < totalSamples; offset += blockSize)
    {
        const int numThisBlock = std::min (blockSize, totalSamples - offset);
        buffer.setSize (2, numThisBlock, false, false, true);
        buffer.clear();
        midi.clear();

        if (offset == 0)
            midi.addEvent (juce::MidiMessage::noteOn (1, midiNote, static_cast<juce::uint8> (velocity)), 0);
        if (offset <= noteOffSample && noteOffSample < offset + numThisBlock)
            midi.addEvent (juce::MidiMessage::noteOff (1, midiNote), noteOffSample - offset);

        processor.processBlock (buffer, midi);

        for (int ch = 0; ch < capture.getNumChannels(); ++ch)
            capture.copyFrom (ch, offset, buffer, ch, 0, numThisBlock);
    }
}

bool writeCaptureWav (const juce::File& outFile, const juce::AudioBuffer<float>& capture, double sampleRate)
{
    outFile.getParentDirectory().createDirectory();
    if (outFile.existsAsFile())
        outFile.deleteFile();

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> stream (outFile.createOutputStream());
    if (stream == nullptr)
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer (
        wav.createWriterFor (stream.get(), sampleRate, static_cast<unsigned int> (capture.getNumChannels()),
                             24, {}, 0));

    if (writer == nullptr)
        return false;

    stream.release();
    return writer->writeFromAudioSampleBuffer (capture, 0, capture.getNumSamples());
}

bool smokeContractPass (const juce::File& wavFile)
{
    const auto path = wavFile.getFullPathName();
    const auto cmd = "python3 - \"" + path + "\" <<'PY'\n"
                     "import math, sys\n"
                     "path = sys.argv[1]\n"
                     "with open(path, 'rb') as f:\n"
                     "    f.read(44)\n"
                     "    data = f.read()\n"
                     "frames = len(data) // 6\n"
                     "left = [int.from_bytes(data[i*6:i*6+3], 'little', signed=True) / 8388608.0 for i in range(frames)]\n"
                     "rms = math.sqrt(sum(s*s for s in left) / max(1, len(left)))\n"
                     "peak = max((abs(s) for s in left), default=0.0)\n"
                     "rms_db = 20.0 * math.log10(rms + 1e-12)\n"
                     "peak_db = 20.0 * math.log10(peak + 1e-12)\n"
                     "max_peak = 10.0 ** (-0.5 / 20.0)\n"
                     "print(f'rms={rms_db:.2f} dBFS peak={peak_db:.2f} dBFS')\n"
                     "if rms_db <= -60.0 or peak > max_peak + 1e-4:\n"
                     "    raise SystemExit(1)\n"
                     "PY";

    return std::system (cmd.toRawUTF8()) == 0;
}

bool runSmokeMode (bool hostCallsPrepare)
{
    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());
    if (processor == nullptr)
        return false;

    juce::AudioBuffer<float> capture;
    renderMidiToBuffer (*processor, hostCallsPrepare, 44100.0, 0, 60, 100, 2.0, capture);

    const juce::File temp = juce::File::getSpecialLocation (juce::File::tempDirectory)
                                .getChildFile (hostCallsPrepare ? "jdupg-smoke-prep.wav" : "jdupg-smoke-lazy.wav");

    if (! writeCaptureWav (temp, capture, 44100.0))
        return false;

    return smokeContractPass (temp);
}

void printUsage()
{
    std::cerr << "Usage: OfflineRender <output.wav> [program] [midiNote] [velocity] [seconds] [sampleRate]\n"
              << "       OfflineRender --smoke-check | --smoke-lazy-check\n"
              << "  Defaults: program=0 note=60 velocity=100 seconds=2.0 sampleRate=44100\n";
}

} // namespace

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    const juce::String arg1 (argv[1]);
    if (arg1 == "--smoke-check")
    {
        std::puts ("--- with host prepareToPlay ---");
        return runSmokeMode (true) ? 0 : 1;
    }

    if (arg1 == "--smoke-lazy-check")
    {
        std::puts ("--- without host prepareToPlay (constructor/lazy DSP) ---");
        return runSmokeMode (false) ? 0 : 1;
    }

    const juce::File outFile (argv[1]);
    const int program = argc > 2 ? std::atoi (argv[2]) : 0;
    const int midiNote = argc > 3 ? std::atoi (argv[3]) : 60;
    const int velocity = argc > 4 ? std::atoi (argv[4]) : 100;
    const double seconds = argc > 5 ? std::atof (argv[5]) : 2.0;
    const double sampleRate = argc > 6 ? std::atof (argv[6]) : 44100.0;

    if (seconds <= 0.0 || sampleRate <= 0.0)
    {
        std::cerr << "Invalid duration or sample rate\n";
        return 1;
    }

    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());
    if (processor == nullptr)
    {
        std::cerr << "Failed to create plugin processor\n";
        return 1;
    }

    juce::AudioBuffer<float> capture;
    renderMidiToBuffer (*processor, true, sampleRate, program, midiNote, velocity, seconds, capture);

    if (! writeCaptureWav (outFile, capture, sampleRate))
    {
        std::cerr << "Failed to write WAV\n";
        return 1;
    }

    std::cout << "Wrote " << capture.getNumSamples() << " samples to " << outFile.getFullPathName() << '\n';
    return 0;
}