// Offline G-funk style bass render for V Voyager (used with UI screen capture).
#include "SynthProcessor.h"
#include <JuceHeader.h>

namespace
{
void addNote(juce::MidiBuffer& midi, int note, int startSample, int numSamples, int velocity = 105)
{
    midi.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(velocity)), startSample);
    midi.addEvent(juce::MidiMessage::noteOff(1, note), startSample + numSamples - 1);
}

/** West-coast style minor funk bass (~92 BPM), 8 bars. */
void fillSnoopBassPattern(juce::MidiBuffer& midi, double sampleRate, int totalSamples)
{
    const double bpm = 92.0;
    const double beatsPerSecond = bpm / 60.0;
    const int sixteenth = static_cast<int>(sampleRate / (beatsPerSecond * 4.0));

    // D minor pocket: D2 F2 G2 A2
    const int D2 = 38, F2 = 41, G2 = 43, A2 = 45, C3 = 48;
    const int pattern[] = {
        D2, D2, D2, G2, F2, G2, A2, G2,
        D2, D2, F2, F2, G2, A2, A2, G2,
        D2, C3, A2, G2, F2, G2, D2, D2,
        A2, G2, F2, D2, D2, G2, F2, D2,
    };

    const int patternLen = static_cast<int>(std::size(pattern));
    int sample = sixteenth; // 1-beat count-in rest
    int idx = 0;
    while (sample + sixteenth < totalSamples)
    {
        const int note = pattern[idx % patternLen];
        const int vel = (idx % 4 == 0) ? 115 : 98;
        const int len = (idx % 4 == 3) ? sixteenth / 2 : static_cast<int>(sixteenth * 0.85);
        addNote(midi, note, sample, juce::jmax(1, len), vel);
        sample += sixteenth;
        ++idx;
    }
}
} // namespace

int main()
{
    MoogVoyagerAudioProcessor processor;
    auto& apvts = processor.getApvts();
    auto setFloat = [&apvts](std::string_view id, float value) {
        if (auto* param = apvts.getParameter(std::string(id)))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    };
    // G-funk sub: saw stack, low ladder filter, glide, moderate drive.
    setFloat(SynthParamIDs::osc1Level, 0.95f);
    setFloat(SynthParamIDs::osc2Level, 0.55f);
    setFloat(SynthParamIDs::osc3Level, 0.35f);
    setFloat(SynthParamIDs::osc1Octave, 2.0f); // choice index -> octave 0
    setFloat(SynthParamIDs::filterCutoff, 520.0f);
    setFloat(SynthParamIDs::filterResonance, 0.62f);
    setFloat(SynthParamIDs::filterDrive, 1.85f);
    setFloat(SynthParamIDs::filterEnvAmount, 0.55f);
    setFloat(SynthParamIDs::glideTime, 0.11f);
    setFloat(SynthParamIDs::ampAttack, 0.003f);
    setFloat(SynthParamIDs::ampDecay, 0.18f);
    setFloat(SynthParamIDs::ampSustain, 0.92f);
    setFloat(SynthParamIDs::ampRelease, 0.35f);
    setFloat(SynthParamIDs::outputGainDb, 2.0f);

    const double sampleRate = 44100.0;
    const int blockSize = 512;
    const double durationSec = 14.0;
    const int totalSamples = static_cast<int>(sampleRate * durationSec);

    processor.prepareToPlay(sampleRate, blockSize);

    juce::AudioBuffer<float> buffer(2, blockSize);
    juce::MidiBuffer midi;

    juce::File outFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getChildFile("v-voyager-snoop-bass.wav");
    outFile.deleteFile();

    std::unique_ptr<juce::FileOutputStream> stream(outFile.createOutputStream());
    if (stream == nullptr)
        return 1;

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wav.createWriterFor(stream.get(), sampleRate, 2, 16, {}, 0));
    if (writer == nullptr)
        return 1;
    stream.release();

    fillSnoopBassPattern(midi, sampleRate, totalSamples);

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

    juce::Logger::writeToLog("Wrote " + outFile.getFullPathName());
    std::cout << outFile.getFullPathName() << std::endl;
    return 0;
}
