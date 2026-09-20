#pragma once

#include "../Import/SampleImporter.h"
#include "../Memory/SampleMemoryPool.h"
#include "PadAssignments.h"
#include "SampleVoice.h"

#include <array>
#include <optional>

namespace sp1200
{
class SamplerEngine
{
public:
    void prepare (double sampleRate, int maxBlockSize);
    void process (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

    SampleMemoryPool& memoryPool() noexcept { return pool_; }
    [[nodiscard]] const SampleMemoryPool& memoryPool() const noexcept { return pool_; }

    std::optional<std::size_t> importFile (const juce::File& file, int bankIndex, juce::String name);
    void assignSegmentToPad (int padIndex, int segmentIndex);

    void setPadLevel (int padIndex, float level);
    void setPadTune (int padIndex, float semitones);
    void setFaderMode (int modeIndex) { faderMode_ = modeIndex; }

    [[nodiscard]] PadAssignment getPad (int padIndex) const;

    /** Capture input to new segment (mono sum, SP rate). Returns segment index. */
    std::optional<std::size_t> commitRecording (const juce::AudioBuffer<float>& recorded, double sourceRate, int bankIndex);

    void startRecording();
    void appendRecording (const juce::AudioBuffer<float>& input);
    void cancelRecording();
    std::optional<std::size_t> stopRecordingAndCommit (int bankIndex);
    [[nodiscard]] bool isRecording() const noexcept { return recording_; }

    void triggerPadFromUi (int padIndex, float velocity = 0.9f);

private:
    int findFreeVoice() noexcept;
    void triggerPad (int padIndex, float velocity);
    void handleMidi (const juce::MidiMessage& msg);

    SampleMemoryPool pool_;
    SampleImporter importer_;
    std::array<SampleVoice, kNumVoices> voices_ {};
    PadBank pads_ {};
    int faderMode_ = 0; // 0 vol 1 pitch 2 decay

    bool recording_ = false;
    juce::AudioBuffer<float> recordBuffer_;
    double recordSourceRate_ = kSampleRateHz;
    double hostSampleRate_ = kSampleRateHz;
};

} // namespace sp1200
