#include "PatternSequencer.h"

#include <algorithm>
#include <cmath>

namespace sp1200
{
PatternSequencer::PatternSequencer()
{
    for (int i = 0; i < static_cast<int> (song_.size()); ++i)
        song_[static_cast<std::size_t> (i)].patternIndex = i % 8;
    for (int p = 0; p < kMaxPatterns; ++p)
        patterns_[static_cast<std::size_t> (p)].bars = 2;
}

float quantizeToMultiPitch (float semitones) noexcept
{
    semitones = std::clamp (semitones, -12.0f, 12.0f);
    float best = kDefaultMultiPitchOffsets[0];
    float bestDist = 999.0f;
    for (float off : kDefaultMultiPitchOffsets)
    {
        const float d = std::abs (off - semitones);
        if (d < bestDist)
        {
            bestDist = d;
            best = off;
        }
    }
    return best;
}

Pattern& PatternSequencer::pattern (int index)
{
    return patterns_[static_cast<std::size_t> (std::clamp (index, 0, kMaxPatterns - 1))];
}

const Pattern& PatternSequencer::pattern (int index) const
{
    return patterns_[static_cast<std::size_t> (std::clamp (index, 0, kMaxPatterns - 1))];
}

void PatternSequencer::setPatternBars (int bars)
{
    pattern (currentPattern_).bars = std::clamp (bars, kMinPatternBars, kMaxPatternBars);
}

int PatternSequencer::patternBars() const
{
    return pattern (currentPattern_).bars;
}

void PatternSequencer::addStep (int pad, int stepIndex, float velocity, float tuneSemis)
{
    auto& p = pattern (currentPattern_);
    PatternStep st;
    st.pad = std::clamp (pad, 0, kNumPads - 1);
    st.stepIndex = std::clamp (stepIndex, 0, p.totalSteps() - 1);
    st.velocity = velocity;
    st.tuneSemitones = quantizeToMultiPitch (tuneSemis);
    p.steps.push_back (st);
}

void PatternSequencer::clearCurrentPattern()
{
    pattern (currentPattern_).steps.clear();
}

void PatternSequencer::setSongSlot (int slot, int patternIndex)
{
    if (slot >= 0 && slot < static_cast<int> (song_.size()))
        song_[static_cast<std::size_t> (slot)].patternIndex = std::clamp (patternIndex, 0, kMaxPatterns - 1);
}

int PatternSequencer::songSlot (int slot) const
{
    if (slot < 0 || slot >= static_cast<int> (song_.size()))
        return 0;
    return song_[static_cast<std::size_t> (slot)].patternIndex;
}

void PatternSequencer::startPattern()
{
    playingSong_ = false;
    playing_ = true;
    currentStep_ = 0;
    songPosition_ = 0;
    stepsInPattern_ = pattern (currentPattern_).totalSteps();
    samplesUntilNextStep_ = 0.0;
}

void PatternSequencer::startSong()
{
    playingSong_ = true;
    playing_ = true;
    songPosition_ = 0;
    currentPattern_ = song_[0].patternIndex;
    currentStep_ = 0;
    stepsInPattern_ = pattern (currentPattern_).totalSteps();
    samplesUntilNextStep_ = 0.0;
}

void PatternSequencer::stop()
{
    playing_ = false;
}

void PatternSequencer::scheduleStep (int stepInPattern, std::vector<std::pair<int, float>>& padHits)
{
    const auto& p = pattern (currentPattern_);
    for (const auto& st : p.steps)
    {
        if (st.stepIndex == stepInPattern)
            padHits.emplace_back (st.pad, st.velocity);
    }
}

void PatternSequencer::advance (double hostSampleRate, int numSamples, std::vector<std::pair<int, float>>& padHits)
{
    if (! playing_ || stepsInPattern_ <= 0)
        return;

    const double secPerStep = (60.0 / bpm_) / static_cast<double> (kStepsPerBar);
    const double samplesPerStep = secPerStep * hostSampleRate;

    samplesUntilNextStep_ -= static_cast<double> (numSamples);
    while (samplesUntilNextStep_ <= 0.0)
    {
        scheduleStep (currentStep_, padHits);
        ++currentStep_;
        if (currentStep_ >= stepsInPattern_)
        {
            currentStep_ = 0;
            if (playingSong_)
            {
                songPosition_ = (songPosition_ + 1) % static_cast<int> (song_.size());
                if (song_[static_cast<std::size_t> (songPosition_)].patternIndex < 0)
                    songPosition_ = 0;
                currentPattern_ = std::max (0, song_[static_cast<std::size_t> (songPosition_)].patternIndex);
                stepsInPattern_ = pattern (currentPattern_).totalSteps();
            }
        }
        samplesUntilNextStep_ += samplesPerStep;
    }
}

} // namespace sp1200
