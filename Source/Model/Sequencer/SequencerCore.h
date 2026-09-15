#pragma once

#include "SequencerClock.h"
#include "SixteenStepPattern.h"
#include <JuceHeader.h>
#include <atomic>

namespace vmpc::model
{
/** Track event placeholder for Phase 3 expansion. */
struct SequencerEvent
{
    int64 tick = 0;
    int noteNumber = 36;
    uint8_t velocity = 100;
    bool isNoteOn = true;
};

/**
 * Sequencer model: clock + 16-step pattern → MIDI-style events.
 * Audio thread may call processBlock; GUI reads atomics / copies state on message thread.
 */
class SequencerCore
{
public:
    SequencerCore();

    void setBpm(double bpm) noexcept;
    double getBpm() const noexcept { return bpm; }

    void setSwing(int percent) noexcept { swingPercent = juce::jlimit(0, 100, percent); }
    int getSwing() const noexcept { return swingPercent; }

    void setPpqn(int newPpqn) noexcept { ppqn = juce::jmax(1, newPpqn); }
    int getPpqn() const noexcept { return ppqn; }

    SixteenStepPattern& getPattern() noexcept { return pattern; }
    const SixteenStepPattern& getPattern() const noexcept { return pattern; }

    void prepare(double sampleRate);
    void reset();

    /** Real-time: advance time and fill note-ons for this block. */
    void processBlock(int numSamples, juce::MidiBuffer& midiOut);

    int getPlayingStepForUi() const noexcept { return playingStepForUi.load(); }

private:
    SequencerClock clock;
    SixteenStepPattern pattern;
    double bpm = 92.0;
    int swingPercent = 54;
    int ppqn = vmpc::audio::kDefaultPpqn;
    int64 ticksPerStep = 0;
    int stepIndex = 0;
    std::atomic<int> playingStepForUi { 0 };
};
} // namespace vmpc::model
