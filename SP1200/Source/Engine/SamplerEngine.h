#pragma once

#include "../Import/SampleImporter.h"
#include "../Memory/SampleMemoryPool.h"
#include "PadAssignments.h"
#include "PatternSequencer.h"
#include "../DSP/Ssm2044BusFilter.h"
#include "SampleVoice.h"

#include <array>
#include <optional>
#include <vector>

namespace sp1200
{
class SamplerEngine
{
public:
    SamplerEngine();

    void prepare (double sampleRate, int maxBlockSize);
    void process (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

    SampleMemoryPool& memoryPool() noexcept { return pool_; }
    [[nodiscard]] const SampleMemoryPool& memoryPool() const noexcept { return pool_; }

    PatternSequencer& sequencer() noexcept { return sequencer_; }
    [[nodiscard]] const PatternSequencer& sequencer() const noexcept { return sequencer_; }

    std::optional<std::size_t> importFile (const juce::File& file, int bankIndex, juce::String name);
    void assignSegmentToPad (int padIndex, int segmentIndex);
    void setPadAssignment (int padIndex, PadAssignment assignment);

    void setPadLevel (int padIndex, float level);
    void setPadTune (int padIndex, float semitones);
    void setFaderMode (int modeIndex) { faderMode_ = modeIndex; }

    [[nodiscard]] PadAssignment getPad (int padIndex) const;
    [[nodiscard]] int multiPitchSourceSegment() const noexcept { return multiPitch_.sourceSegmentIndex; }

    void setFilterCutoffNorm (float norm);
    void setFilterResonance (float norm);
    [[nodiscard]] float getFilterCutoffNorm() const noexcept { return filterCutoffNorm_; }
    [[nodiscard]] float getFilterResonance() const noexcept { return filterResonance_; }

    /** Audition segment or [start,end) without editing memory. */
    void previewSegment (std::size_t segmentIndex, std::int64_t startSample = 0, std::int64_t endSample = -1);

    std::optional<std::size_t> commitRecording (const juce::AudioBuffer<float>& recorded, double sourceRate, int bankIndex);

    void startRecording();
    void appendRecording (const juce::AudioBuffer<float>& input);
    void cancelRecording();
    std::optional<std::size_t> stopRecordingAndCommit (int bankIndex);
    [[nodiscard]] bool isRecording() const noexcept { return recording_; }

    void triggerPadFromUi (int padIndex, float velocity = 0.9f);

    void setMultiPitchEnabled (bool on) { multiPitch_.enabled = on; }
    void setMultiPitchSourceSegment (int segmentIndex) { multiPitch_.sourceSegmentIndex = segmentIndex; }
    [[nodiscard]] bool multiPitchEnabled() const noexcept { return multiPitch_.enabled; }

    /** Transient auto-chop; returns new segment indices (empty on failure). */
    std::vector<std::size_t> autoChopSegment (std::size_t segmentIndex, int numSlices);

    void recordStepOnCurrentPattern (int padIndex, float velocity);

private:
    int findFreeVoice() noexcept;
    void triggerPad (int padIndex, float velocity, float extraTune = 0.0f, float pan = 0.0f, float filterStep = 0.5f);
    void handleMidi (const juce::MidiMessage& msg);

    SampleMemoryPool pool_;
    SampleImporter importer_;
    PatternSequencer sequencer_;
    std::array<SampleVoice, kNumVoices> voices_ {};
    PadBank pads_ {};
    MultiPitchState multiPitch_ { defaultMultiPitchState() };
    int faderMode_ = 0;

    bool recording_ = false;
    juce::AudioBuffer<float> recordBuffer_;
    double recordSourceRate_ = kSampleRateHz;
    double hostSampleRate_ = kSampleRateHz;

    std::vector<ScheduledHit> pendingPadHits_;
    Ssm2044BusFilter busFilter_;
    float filterCutoffNorm_ = 0.72f;
    float filterResonance_ = 0.35f;
};

} // namespace sp1200
