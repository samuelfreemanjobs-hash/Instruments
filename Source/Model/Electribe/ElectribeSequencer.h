#pragma once

#include "ElectribeSong.h"
#include "Audio/RealTimeConstraints.h"
#include "Model/Sequencer/SequencerClock.h"
#include <JuceHeader.h>
#include <atomic>

namespace resonance::model
{
class ElectribeSequencer
{
public:
    void setSong(ElectribeSong* songPtr) noexcept { song = songPtr; }

    void setBpm(double bpm) noexcept;
    void setSwing(int percent) noexcept { swingPercent = juce::jlimit(0, 100, percent); }

    void prepare(double sampleRate);
    void reset();

    void processBlock(int numSamples, juce::MidiBuffer& midiOut);

    int getPlayingStepForUi() const noexcept { return playingStepForUi.load(); }

private:
    ElectribeSong* song = nullptr;
    SequencerClock clock;
    double bpm = 120.0;
    int swingPercent = 50;
    int ppqn = resonance::audio::kDefaultPpqn;
    int64 ticksPerStep = 0;
    int stepIndex = 0;
    std::atomic<int> playingStepForUi { 0 };
};
} // namespace resonance::model
