#pragma once

#include "ElectribeSong.h"
#include "Audio/RealTimeConstraints.h"
#include "Model/Sequencer/SequencerClock.h"
#include <JuceHeader.h>
#include <atomic>

namespace vmpc::model
{
class ElectribeSequencer
{
public:
    void setSong(ElectribeSong* songPtr) noexcept { song = songPtr; }

    void setBpm(double bpm) noexcept;
    void setSwing(int percent) noexcept
    {
        swingPercent = juce::jlimit(50, 75, percent);
        scheduleNextStepDuration();
    }

    void prepare(double sampleRate);
    void reset();

    void processBlock(int numSamples, juce::MidiBuffer& midiOut);

    int getPlayingStepForUi() const noexcept { return playingStepForUi.load(); }

private:
    void scheduleNextStepDuration();
    void fireCurrentStep(juce::MidiBuffer& midiOut, int numSamples);

    ElectribeSong* song = nullptr;
    SequencerClock clock;
    double sampleRateHz = 44100.0;
    double bpm = 120.0;
    int swingPercent = 50;
    int ppqn = vmpc::audio::kDefaultPpqn;
    int64 ticksPerStep = 0;
    int stepIndex = 0;
    double samplesUntilNextStep = 0.0;
    std::atomic<int> playingStepForUi { 0 };
};
} // namespace vmpc::model
