#include "SequencerClock.h"

namespace vmpc::model
{
double SequencerClock::samplesForSixteenthStep(double sampleRate,
                                               double bpm,
                                               int sixteenthIndex,
                                               int swingPercent) noexcept
{
    const double sixteenth = sampleRate * 60.0 / juce::jmax(20.0, bpm) / 4.0;
    if (swingPercent <= 50)
        return sixteenth;

    const double swing = juce::jlimit(0.0, 1.0, (static_cast<double>(swingPercent) - 50.0) / 50.0);
    const double pair = sixteenth * 2.0;
    const bool offbeat = (sixteenthIndex & 1) == 1;
    const double first = pair * (0.5 - swing * 0.2);
    const double second = pair - first;
    return offbeat ? second : first;
}

double SequencerClock::swingDelayRatioForStep(int stepIndex, int swingPercent) noexcept
{
    const bool offbeatSixteenth = (stepIndex % 2) == 1;
    if (!offbeatSixteenth || swingPercent == 50)
        return 1.0;

    const double amount = juce::jlimit(50, 75, swingPercent) / 100.0;
    const double straight = 1.0;
    const double swung = 1.0 + (amount - 0.5) * 2.0;
    return offbeatSixteenth ? swung : straight;
}

int SequencerClock::advance(int numSamples, int ppqn) noexcept
{
    if (numSamples <= 0 || ppqn <= 0)
        return 0;

    const double samplesPerBeat = sampleRate * 60.0 / beatsPerMinute;
    const double samplesPerTick = samplesPerBeat / static_cast<double>(ppqn);

    int ticksCrossed = 0;
    int remaining = numSamples;

    while (remaining > 0)
    {
        if (samplesUntilNextTick <= 0.0)
        {
            ++tickPosition;
            ++ticksCrossed;
            samplesUntilNextTick += samplesPerTick;
        }

        const int consume = static_cast<int>(juce::jmin(static_cast<double>(remaining), samplesUntilNextTick));
        samplesUntilNextTick -= consume;
        remaining -= consume;
    }

    return ticksCrossed;
}
} // namespace vmpc::model
