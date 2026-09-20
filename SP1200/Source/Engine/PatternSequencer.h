#pragma once

#include "../SP1200Constants.h"

#include <algorithm>
#include <array>
#include <vector>

namespace sp1200
{
struct PatternStep
{
    int pad = 0;
    int stepIndex = 0;
    float velocity = 0.9f;
    float tuneSemitones = 0.0f;
    float pan = 0.0f;           // -1 L … +1 R
    float filterCutoff = 0.5f;  // per-step bus filter offset (0..1)
};

struct Pattern
{
    int bars = 2;
    std::vector<PatternStep> steps;

    [[nodiscard]] int totalSteps() const { return std::clamp (bars, kMinPatternBars, kMaxPatternBars) * kStepsPerBar; }
};

struct SongSlot
{
    int patternIndex = 0; // 0-based into pattern bank
};

struct ScheduledHit
{
    int pad = 0;
    float velocity = 0.9f;
    float tuneSemitones = 0.0f;
    float pan = 0.0f;
    float filterCutoff = 0.5f;
};

/** Quantize chromatic / roll pitch to nearest multi-pitch slot offset. */
float quantizeToMultiPitch (float semitones) noexcept;

class PatternSequencer
{
public:
    PatternSequencer();

    Pattern& pattern (int index);
    [[nodiscard]] const Pattern& pattern (int index) const;

    void setCurrentPattern (int index) { currentPattern_ = std::clamp (index, 0, kMaxPatterns - 1); }
    [[nodiscard]] int currentPattern() const noexcept { return currentPattern_; }

    void setPatternBars (int bars);
    [[nodiscard]] int patternBars() const;

    void addStep (int pad, int stepIndex, float velocity, float tuneSemis = 0.0f);
    /** Add if missing, remove if same pad+step exists. */
    void toggleStep (int pad, int stepIndex, float velocity, float tuneSemis, bool chromaticMode);
    void clearCurrentPattern();
    [[nodiscard]] bool hasStep (int pad, int stepIndex) const;
    [[nodiscard]] float stepTune (int pad, int stepIndex) const;
    [[nodiscard]] int recordStepCursor() const noexcept { return recordStepCursor_; }
    void setRecordStepCursor (int step) { recordStepCursor_ = std::max (0, step); }

    void setSongSlot (int slot, int patternIndex);
    [[nodiscard]] int songSlot (int slot) const;

    void setBpm (double bpm) { bpm_ = std::clamp (bpm, 40.0, 240.0); }
    [[nodiscard]] double bpm() const noexcept { return bpm_; }

    void setSwing (float s) { swing_ = std::clamp (s, 0.0f, 1.0f); }

    void startPattern();
    void startSong();
    void stop();
    [[nodiscard]] bool isPlaying() const noexcept { return playing_; }
    [[nodiscard]] int playbackStepIndex() const noexcept { return currentStep_; }

    /** Call from audio thread; returns pads to trigger this block. */
    void advance (double hostSampleRate, int numSamples, std::vector<ScheduledHit>& padHits);

private:
    void scheduleStep (int stepInPattern, std::vector<ScheduledHit>& padHits);

    std::array<Pattern, kMaxPatterns> patterns_ {};
    std::array<SongSlot, 32> song_ {};
    int currentPattern_ = 0;
    int songPosition_ = 0;
    bool playingSong_ = false;

    bool playing_ = false;
    double bpm_ = 92.0;
    float swing_ = 0.59f;
    double samplesUntilNextStep_ = 0.0;
    int currentStep_ = 0;
    int stepsInPattern_ = 32;
    int recordStepCursor_ = 0;
};

} // namespace sp1200
