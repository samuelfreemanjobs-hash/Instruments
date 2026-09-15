#include "ElectribeSequencer.h"
#include "Audio/RealTimeConstraints.h"

namespace vmpc::model
{
void ElectribeSequencer::setBpm(double newBpm) noexcept
{
    bpm = juce::jlimit(20.0, 999.0, newBpm);
    clock.setBpm(bpm);
}

void ElectribeSequencer::prepare(double sampleRate)
{
    sampleRateHz = sampleRate;
    clock.setSampleRate(sampleRate);
    clock.setBpm(bpm);
    ticksPerStep = ppqn / 4;
    scheduleNextStepDuration();
}

void ElectribeSequencer::reset()
{
    clock.reset();
    stepIndex = 0;
    samplesUntilNextStep = 0.0;
    playingStepForUi.store(0);
    scheduleNextStepDuration();
}

void ElectribeSequencer::scheduleNextStepDuration()
{
    samplesUntilNextStep = SequencerClock::samplesForSixteenthStep(sampleRateHz, bpm, stepIndex, swingPercent);
}

void ElectribeSequencer::fireCurrentStep(juce::MidiBuffer& midiOut, int numSamples)
{
    if (song == nullptr)
        return;

    playingStepForUi.store(stepIndex);

    for (int p = 0; p < ElectribeSong::kNumParts; ++p)
    {
        const auto& part = song->getPart(p);
        if (part.muted)
            continue;

        const auto& step = part.steps[static_cast<size_t>(stepIndex)];
        if (!step.active)
            continue;

        const int vel = step.accent ? juce::jmin(127, static_cast<int>(step.velocity) + 20)
                                    : static_cast<int>(step.velocity);
        const float level = juce::jlimit(0.0f, 1.0f, part.level);
        const int scaledVel = juce::jlimit(1, 127, static_cast<int>(vel * level));

        midiOut.addEvent(juce::MidiMessage::noteOn(1, part.midiNote, static_cast<juce::uint8>(scaledVel)), 0);
        midiOut.addEvent(juce::MidiMessage::noteOff(1, part.midiNote), juce::jmin(numSamples - 1, 48));
    }
}

void ElectribeSequencer::processBlock(int numSamples, juce::MidiBuffer& midiOut)
{
    if (song == nullptr || numSamples <= 0)
        return;

    int remaining = numSamples;
    while (remaining > 0)
    {
        if (samplesUntilNextStep <= 0.0)
        {
            fireCurrentStep(midiOut, remaining);
            stepIndex = (stepIndex + 1) & 15;
            scheduleNextStepDuration();
        }

        const int consume = static_cast<int>(juce::jmin(static_cast<double>(remaining), samplesUntilNextStep));
        samplesUntilNextStep -= static_cast<double>(consume);
        remaining -= consume;
    }
}
} // namespace vmpc::model
