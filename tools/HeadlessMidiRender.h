#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <iostream>
#include <memory>

namespace headless
{

struct MidiRenderArgs
{
    int program = 0;
    int midiNote = 60;
    int velocity = 100;
    double seconds = 2.0;
    double sampleRate = 44100.0;
    int blockSize = 512;
    double noteOffFraction = 0.85;
};

inline bool renderProcessorToWav (juce::AudioProcessor& processor,
                                  const juce::File& outFile,
                                  const MidiRenderArgs& args,
                                  juce::String& error)
{
    if (args.seconds <= 0.0 || args.sampleRate <= 0.0)
    {
        error = "Invalid duration or sample rate";
        return false;
    }

    processor.prepareToPlay (args.sampleRate, args.blockSize);
    processor.setCurrentProgram (args.program);

    const int totalSamples = static_cast<int> (std::ceil (args.seconds * args.sampleRate));
    const int noteOffSample =
        std::max (1, static_cast<int> (args.noteOffFraction * static_cast<double> (totalSamples)));

    juce::AudioBuffer<float> buffer (2, args.blockSize);
    juce::MidiBuffer midi;
    juce::AudioBuffer<float> capture (2, totalSamples);
    capture.clear();

    for (int offset = 0; offset < totalSamples; offset += args.blockSize)
    {
        const int numThisBlock = std::min (args.blockSize, totalSamples - offset);
        buffer.setSize (2, numThisBlock, false, false, true);
        buffer.clear();
        midi.clear();

        if (offset == 0)
            midi.addEvent (juce::MidiMessage::noteOn (1, args.midiNote, static_cast<juce::uint8> (args.velocity)), 0);
        if (offset <= noteOffSample && noteOffSample < offset + numThisBlock)
            midi.addEvent (juce::MidiMessage::noteOff (1, args.midiNote), noteOffSample - offset);

        processor.processBlock (buffer, midi);

        const int chCount = std::min (2, buffer.getNumChannels());
        for (int ch = 0; ch < chCount; ++ch)
            capture.copyFrom (ch, offset, buffer, ch, 0, numThisBlock);
    }

    outFile.getParentDirectory().createDirectory();

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> stream (outFile.createOutputStream());
    if (stream == nullptr)
    {
        error = "Cannot open output file: " + outFile.getFullPathName();
        return false;
    }

    std::unique_ptr<juce::AudioFormatWriter> writer (
        wav.createWriterFor (stream.get(), args.sampleRate, static_cast<unsigned int> (capture.getNumChannels()),
                             24, {}, 0));

    if (writer == nullptr)
    {
        error = "Failed to create WAV writer";
        return false;
    }

    stream.release();
    if (! writer->writeFromAudioSampleBuffer (capture, 0, totalSamples))
    {
        error = "WAV write failed";
        return false;
    }

    std::cout << "Wrote " << totalSamples << " samples to " << outFile.getFullPathName() << '\n';
    return true;
}

} // namespace headless
