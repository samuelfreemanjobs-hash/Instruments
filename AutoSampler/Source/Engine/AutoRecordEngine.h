#pragma once

#include "MultisampleProgram.h"
#include "NoteMapBuilder.h"
#include "SamplePostProcessor.h"

#include <JuceHeader.h>

#include <array>
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
        /** MPC gate: MIDI note held (ms). */
        double noteHoldMs = 1000.0;
        /** MPC release tail recorded after note-off (ms). */
        double tailMs = 2000.0;
        /** Audio captured before note-on (ms). */
        double prerollMs = 0.0;
        /** Pause between note captures (ms). */
        double pauseMs = 150.0;
        /** Legacy single duration; used only when hold/tail not set in JSON. */
        double recordSeconds = 3.0;
        int velocity = 100;
        std::array<int, 4> velocitySlots { { 127, 0, 0, 0 } };
        int midiChannel = 1;
        bool midiThru = true;
        PostProcessConfig postProcess {};
        NoteMapPreset noteMapPreset = NoteMapPreset::chromatic;
        BlackKeyPolicy blackKeyPolicy = BlackKeyPolicy::include;
        bool recordUntilSilence = false;
        double maxRecordSeconds = 30.0;
        float silenceThresholdDb = -50.0f;
        double silenceHoldSeconds = 0.25;
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
    int currentVelocity_ = 100;
    std::size_t velocityPassIndex_ = 0;
    int trailingSilentSamples_ = 0;
    bool noteOffSent_ = false;

    juce::AudioBuffer<float> capture_;
    int captureWritePos_ = 0;
    int samplesUntilNextAction_ = 0;
    int targetRecordSamples_ = 0;
    int prerollSamples_ = 0;
    int holdSamples_ = 0;
    int noteOnSamplePos_ = -1;
    int noteOffSamplePos_ = -1;
    bool noteHeld_ = false;
    std::vector<int> velocityList_;
};

} // namespace autosampler
