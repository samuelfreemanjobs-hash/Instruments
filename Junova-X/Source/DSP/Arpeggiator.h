#pragma once

#include "ArpHostContext.h"

#include <juce_audio_basics/juce_audio_basics.h>

#include <array>

namespace junovax::dsp
{
/** Pre-voice MIDI arp — PPQ grid (1/16–1/2) when host PPQ valid; else BPM fallback. */
class Arpeggiator
{
public:
    static constexpr int kMaxHeld = 16;

    void prepare (double sampleRate) noexcept;
    void reset() noexcept;

    /** arpRate 0…1 (≤0.01 bypasses). arpRange 1…4 octaves. latch keeps held notes after note-off. */
    void process (const juce::MidiBuffer& input,
                  juce::MidiBuffer& output,
                  float arpRate,
                  float arpRange,
                  bool latch,
                  const ArpHostContext& host,
                  int numSamples) noexcept;

private:
    void ingestMidi (const juce::MidiBuffer& input, bool latch) noexcept;
    double stepPpq (float arpRate) const noexcept;
    void emitStep (juce::MidiBuffer& output, int samplePos, int rangeOct) noexcept;
    void schedulePpqSteps (juce::MidiBuffer& output,
                           float arpRate,
                           float arpRange,
                           const ArpHostContext& host,
                           int numSamples) noexcept;
    double sampleRate_ = 48000.0;
    std::array<int, kMaxHeld> held_{};
    int heldCount_ = 0;
    int stepIndex_ = 0;
    int playingNote_ = -1;
    double nextStepPpq_ = 0.0;
    double samplesUntilStep_ = 0.0;
    double freeRunPpq_ = 0.0;
};
} // namespace junovax::dsp
