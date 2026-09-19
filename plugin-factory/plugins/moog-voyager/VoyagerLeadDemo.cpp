#include "GfunkPresets.h"
#include "SynthProcessor.h"
#include <JuceHeader.h>

namespace
{
struct NoteEvent
{
    int midi;
    double startBeat;
    double durationBeats;
    int velocity;
};

void renderLead(MoogVoyagerAudioProcessor& processor, const juce::File& outFile)
{
    const double sampleRate = 44100.0;
    const int blockSize = 512;
    const double bpm = 94.0;
    const double beatsPerSecond = bpm / 60.0;
    const double totalBeats = 16.0;
    const int totalSamples = static_cast<int>(sampleRate * totalBeats / beatsPerSecond);

    gfunk::applyPreset(processor, gfunk::PresetId::gFunkLead);
    processor.prepareToPlay(sampleRate, blockSize);

    const NoteEvent line[] = {
        { 74, 0.0, 0.45, 108 },  // D5
        { 77, 0.5, 0.45, 104 },  // F5
        { 79, 1.0, 0.45, 110 },  // G5
        { 81, 1.5, 0.9, 112 },   // A5
        { 79, 2.5, 0.45, 105 },
        { 77, 3.0, 0.45, 102 },
        { 74, 3.5, 0.45, 108 },
        { 72, 4.0, 1.2, 115 },   // D4 hook
        { 74, 5.5, 0.45, 108 },
        { 77, 6.0, 0.45, 104 },
        { 81, 6.5, 0.45, 112 },
        { 79, 7.0, 1.0, 110 },
        { 77, 8.0, 0.45, 105 },
        { 74, 8.5, 0.45, 108 },
        { 72, 9.0, 0.45, 112 },
        { 69, 9.5, 1.5, 118 },   // A4
    };

    juce::MidiBuffer midi;
    for (const auto& ev : line)
    {
        const int start = static_cast<int>(ev.startBeat / beatsPerSecond * sampleRate);
        const int len = static_cast<int>(ev.durationBeats / beatsPerSecond * sampleRate);
        midi.addEvent(juce::MidiMessage::noteOn(1, ev.midi, static_cast<juce::uint8>(ev.velocity)), start);
        midi.addEvent(juce::MidiMessage::noteOff(1, ev.midi), start + juce::jmax(1, len));
    }

    juce::AudioBuffer<float> buffer(2, blockSize);
    outFile.deleteFile();
    std::unique_ptr<juce::FileOutputStream> stream(outFile.createOutputStream());
    juce::WavAudioFormat wav;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wav.createWriterFor(stream.get(), sampleRate, 2, 16, {}, 0));
    if (writer == nullptr)
        return;
    stream.release();

    int processed = 0;
    while (processed < totalSamples)
    {
        const int n = juce::jmin(blockSize, totalSamples - processed);
        buffer.clear();
        juce::MidiBuffer blockMidi;
        for (const auto metadata : midi)
        {
            const int pos = metadata.samplePosition;
            if (pos >= processed && pos < processed + n)
                blockMidi.addEvent(metadata.getMessage(), pos - processed);
        }
        processor.processBlock(buffer, blockMidi);
        writer->writeFromAudioSampleBuffer(buffer, 0, n);
        processed += n;
    }
}
} // namespace

int main()
{
    MoogVoyagerAudioProcessor processor;
    juce::File outFile =
        juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("v-voyager-gfunk-lead.wav");
    renderLead(processor, outFile);
    std::cout << outFile.getFullPathName() << std::endl;
    return 0;
}
