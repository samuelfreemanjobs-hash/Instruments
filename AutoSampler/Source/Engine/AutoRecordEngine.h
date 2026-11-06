#pragma once

#include "MultisampleProgram.h"
#include "SamplePostProcessor.h"

#include <JuceHeader.h>

#include <vector>

namespace autosampler
{

enum class AutoSampleState
{
    idle = 0,
    waitingGap,
    recordingNote,
    finished,
    aborted
};

/** Schedules MIDI notes and captures stereo input into a MultisampleProgram. */
class AutoRecordEngine
{
public:
    struct Config
    {
        int lowNote = 36;
        int highNote = 72;
        int noteStep = 1;
        double recordSeconds = 2.0;
        double gapSeconds = 0.15;
        int velocity = 100;
        bool midiThru = true;
        PostProcessConfig postProcess {};
    };

    void prepare (double sampleRate, int maxBlockSize);
    void reset() noexcept;

    void start (Config config);
    void abort() noexcept;

    bool isActive() const noexcept { return state_ != AutoSampleState::idle && state_ != AutoSampleState::finished && state_ != AutoSampleState::aborted; }
    AutoSampleState state() const noexcept { return state_; }
    float progress01() const noexcept;

    int currentRootNote() const noexcept { return currentRootNote_; }

    /** Call each block: pass input audio, optional MIDI out for source instrument. */
    void process (const juce::AudioBuffer<float>& input,
                  juce::MidiBuffer& midiOut,
                  MultisampleProgram& destination);

private:
    void advanceSchedule (int samplesInBlock) noexcept;
    void beginNote (int rootNote);
    void finalizeCurrentZone (MultisampleProgram& destination);
    void buildNoteList();

    double sampleRate_ = 44100.0;
    Config config_{};
    AutoSampleState state_ = AutoSampleState::idle;

    std::vector<int> noteList_;
    std::size_t noteIndex_ = 0;
    int currentRootNote_ = -1;

    juce::AudioBuffer<float> capture_;
    int captureWritePos_ = 0;
    int samplesUntilNextAction_ = 0;
    int targetRecordSamples_ = 0;
    bool noteHeld_ = false;
};

} // namespace autosampler
