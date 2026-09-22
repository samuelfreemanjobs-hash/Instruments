#include "PluginProcessor.h"
#include "Presets/Jz400Presets.h"

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
    std::cerr << "Usage: Rev2TrapOfflineRender <output.wav> [program] [midiNote] [velocity] [seconds] [sampleRate]\n";
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
    int program = argc > 2 ? std::atoi (argv[2]) : 0;
    program = juce::jlimit (0, rev2trap::presets::kTotalFactoryPrograms - 1, program);
    const int midiNote = argc > 3 ? std::atoi (argv[3]) : 60;
    const int velocity = argc > 4 ? std::atoi (argv[4]) : 100;
    const double seconds = argc > 5 ? std::atof (argv[5]) : 2.8;
    const double sampleRate = argc > 6 ? std::atof (argv[6]) : 44100.0;

    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());
    processor->prepareToPlay (sampleRate, 512);
    processor->setCurrentProgram (program);

    const int totalSamples = static_cast<int> (std::ceil (seconds * sampleRate));
    const int noteOffSample = std::max (1, static_cast<int> (0.82 * static_cast<double> (totalSamples)));

    juce::AudioBuffer<float> buffer (2, 512);
    juce::MidiBuffer midi;
    juce::AudioBuffer<float> capture (1, totalSamples);
    capture.clear();

    for (int offset = 0; offset < totalSamples; offset += 512)
    {
        const int n = std::min (512, totalSamples - offset);
        buffer.setSize (2, n, false, false, true);
        buffer.clear();
        midi.clear();
        if (offset == 0)
            midi.addEvent (juce::MidiMessage::noteOn (1, midiNote, static_cast<juce::uint8> (velocity)), 0);
        if (offset <= noteOffSample && noteOffSample < offset + n)
            midi.addEvent (juce::MidiMessage::noteOff (1, midiNote), noteOffSample - offset);

        processor->processBlock (buffer, midi);
        capture.copyFrom (0, offset, buffer, 0, 0, n);
    }

    outFile.getParentDirectory().createDirectory();
    juce::WavAudioFormat format;
    std::unique_ptr<juce::FileOutputStream> stream (outFile.createOutputStream());
    if (stream == nullptr)
        return 1;

    std::unique_ptr<juce::AudioFormatWriter> writer (
        format.createWriterFor (stream.get(), sampleRate, 1, 16, {}, 0));
    if (writer == nullptr)
        return 1;
    stream.release();
    writer->writeFromAudioSampleBuffer (capture, 0, totalSamples);
    std::cout << outFile.getFullPathName() << "\n";
    return 0;
}
