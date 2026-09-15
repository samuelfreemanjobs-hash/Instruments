#pragma once

#include <JuceHeader.h>
#include "Audio/RealTimeConstraints.h"

namespace vmpc::model
{
/** Internal PPQN clock; advances from audio thread using sample position. */
class SequencerClock
{
public:
    void setSampleRate(double sr) noexcept { sampleRate = sr > 0.0 ? sr : 44100.0; }
    void setBpm(double bpm) noexcept { beatsPerMinute = juce::jlimit(20.0, 999.0, bpm); }

    void reset() noexcept
    {
        tickPosition = 0;
        samplesUntilNextTick = 0.0;
    }

    /** Roger Linn MPC-style swing on 16th-note pairs (0–100, 50 = straight). */
    static double swingDelayRatioForStep(int stepIndex, int swingPercent) noexcept;

    /** Sample length of one 16th step with MPC swing (pair of 16ths stays one 8th long). */
    static double samplesForSixteenthStep(double sampleRate,
                                          double bpm,
                                          int sixteenthIndex,
                                          int swingPercent) noexcept;

    /** Call once per audio block; returns number of ticks crossed. */
    int advance(int numSamples, int ppqn) noexcept;

    int64 getTickPosition() const noexcept { return tickPosition; }

private:
    double sampleRate = 44100.0;
    double beatsPerMinute = 92.0;
    int64 tickPosition = 0;
    double samplesUntilNextTick = 0.0;
};
} // namespace vmpc::model
