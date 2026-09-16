#include "PluginProcessor.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

namespace
{

void printUsage()
{
    std::cerr << "Usage: OfflineRender <output.wav> [program] [midiNote] [velocity] [seconds] [sampleRate]\n"
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

    constexpr int blockSize = 512;
    processor->prepareToPlay (sampleRate, blockSize);
    processor->setCurrentProgram (program);

    const int totalSamples = static_cast<int> (std::ceil (seconds * sampleRate));
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
            midi.addEvent (juce::MidiMessage::noteOn (1, midiNote, static_cast<juce::uint8> (velocity)), 0);
        if (offset <= noteOffSample && noteOffSample < offset + numThisBlock)
            midi.addEvent (juce::MidiMessage::noteOff (1, midiNote), noteOffSample - offset);

        processor->processBlock (buffer, midi);

        for (int ch = 0; ch < 2; ++ch)
            capture.copyFrom (ch, offset, buffer, ch, 0, numThisBlock);
    }

    outFile.getParentDirectory().createDirectory();

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> stream (outFile.createOutputStream());
    if (stream == nullptr)
    {
        std::cerr << "Cannot open output file\n";
        return 1;
    }

    std::unique_ptr<juce::AudioFormatWriter> writer (
        wav.createWriterFor (stream.get(), sampleRate, static_cast<unsigned int> (capture.getNumChannels()),
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
