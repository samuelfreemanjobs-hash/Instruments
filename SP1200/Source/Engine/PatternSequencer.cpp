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
    auto& p = pattern (currentPattern_);
    p.bars = std::clamp (bars, kMinPatternBars, kMaxPatternBars);
    const int maxStep = p.totalSteps();
    p.steps.erase (std::remove_if (p.steps.begin(), p.steps.end(),
                                   [maxStep] (const PatternStep& st)
                                   {
                                       return st.stepIndex >= maxStep;
                                   }),
                   p.steps.end());
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

void PatternSequencer::toggleStep (int pad, int stepIndex, float velocity, float tuneSemis, bool chromaticMode)
{
    auto& p = pattern (currentPattern_);
    const int maxStep = std::max (1, p.totalSteps()) - 1;
    const int step = std::clamp (stepIndex, 0, maxStep);
    const int padCl = std::clamp (pad, 0, kNumPads - 1);

    for (auto it = p.steps.begin(); it != p.steps.end(); ++it)
    {
        if (it->pad == padCl && it->stepIndex == step)
        {
            p.steps.erase (it);
            return;
        }
    }

    const float tune = chromaticMode ? quantizeToMultiPitch (tuneSemis) : 0.0f;
    addStep (padCl, step, velocity, tune);
}

bool PatternSequencer::hasStep (int pad, int stepIndex) const
{
    const auto& p = pattern (currentPattern_);
    for (const auto& st : p.steps)
        if (st.pad == pad && st.stepIndex == stepIndex)
            return true;
    return false;
}

float PatternSequencer::stepTune (int pad, int stepIndex) const
{
    const auto& p = pattern (currentPattern_);
    for (const auto& st : p.steps)
        if (st.pad == pad && st.stepIndex == stepIndex)
            return st.tuneSemitones;
    return 0.0f;
}

void PatternSequencer::setSongSlot (int slot, int patternIndex)
{
    if (slot < 0 || slot >= static_cast<int> (song_.size()))
        return;
    if (patternIndex == kSongSlotEnd)
        song_[static_cast<std::size_t> (slot)].patternIndex = kSongSlotEnd;
    else
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
    songPosition_ = 0;
    while (songPosition_ < static_cast<int> (song_.size()) && isSongSlotEnd (songPosition_))
        ++songPosition_;

    if (songPosition_ >= static_cast<int> (song_.size()) || isSongSlotEnd (songPosition_))
    {
        playing_ = false;
        playingSong_ = false;
        return;
    }

    playing_ = true;
    currentPattern_ = song_[static_cast<std::size_t> (songPosition_)].patternIndex;
    currentStep_ = 0;
    stepsInPattern_ = pattern (currentPattern_).totalSteps();
    samplesUntilNextStep_ = 0.0;
}

void PatternSequencer::stop()
{
    playing_ = false;
}

bool PatternSequencer::advanceSongPositionAfterPattern()
{
    int next = songPosition_ + 1;
    if (next >= static_cast<int> (song_.size()))
    {
        if (! songLoop_)
            return false;
        next = 0;
    }

    songPosition_ = next;
    if (isSongSlotEnd (songPosition_))
        return false;

    currentPattern_ = song_[static_cast<std::size_t> (songPosition_)].patternIndex;
    stepsInPattern_ = pattern (currentPattern_).totalSteps();
    return true;
}

void PatternSequencer::scheduleStep (int stepInPattern, std::vector<ScheduledHit>& padHits)
{
    const auto& p = pattern (currentPattern_);
    for (const auto& st : p.steps)
    {
        if (st.stepIndex == stepInPattern)
            padHits.push_back ({ st.pad, st.velocity, st.tuneSemitones, st.pan, st.filterCutoff });
    }
}

void PatternSequencer::advance (double hostSampleRate, int numSamples, std::vector<ScheduledHit>& padHits)
{
    if (! playing_ || stepsInPattern_ <= 0)
        return;

    const double secPerStep = (60.0 / bpm_) / static_cast<double> (kStepsPerBar);

    auto samplesForStep = [&] (int stepIndex) -> double
    {
        double dur = secPerStep;
        if ((stepIndex % 2) == 1)
            dur += secPerStep * static_cast<double> (swing_) * 0.5;
        return dur * hostSampleRate;
    };

    samplesUntilNextStep_ -= static_cast<double> (numSamples);
    while (samplesUntilNextStep_ <= 0.0)
    {
        scheduleStep (currentStep_, padHits);
        const double stepLen = samplesForStep (currentStep_);
        ++currentStep_;
        if (currentStep_ >= stepsInPattern_)
        {
            currentStep_ = 0;
            if (playingSong_ && ! advanceSongPositionAfterPattern())
                playing_ = false;
        }
        samplesUntilNextStep_ += stepLen;
    }
}

} // namespace sp1200
