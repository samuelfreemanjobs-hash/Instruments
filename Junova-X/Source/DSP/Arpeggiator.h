#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <array>

namespace junovax::dsp
{
/** Pre-voice MIDI arp — upward pattern, host BPM when arpRate > 0. */
class Arpeggiator
{
public:
    static constexpr int kMaxHeld = 16;

    void prepare (double sampleRate) noexcept;
    void reset() noexcept;

    /** arpRate 0…1 (≤0.01 bypasses). arpRange 1…4 octaves. */
    void process (const juce::MidiBuffer& input,
                  juce::MidiBuffer& output,
                  float arpRate,
                  float arpRange,
                  double bpm,
                  int numSamples) noexcept;

private:
    void ingestMidi (const juce::MidiBuffer& input) noexcept;
    float stepPeriodSamples (float arpRate, double bpm) const noexcept;

    double sampleRate_ = 48000.0;
    std::array<int, kMaxHeld> held_{};
    int heldCount_ = 0;
    int stepIndex_ = 0;
    int playingNote_ = -1;
    double samplesUntilStep_ = 0.0;
};
} // namespace junovax::dsp
