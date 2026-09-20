#include "SamplerEngine.h"

#include "../Import/TransientChop.h"
#include "../SP1200Constants.h"

#include <algorithm>
#include <vector>

namespace sp1200
{
SamplerEngine::SamplerEngine() = default;

void SamplerEngine::prepare (double sampleRate, int maxBlockSize)
{
    hostSampleRate_ = sampleRate;
    recordSourceRate_ = sampleRate;
    busFilter_.prepare (sampleRate, maxBlockSize);
    busFilter_.setBaseCutoffNorm (filterCutoffNorm_);
    busFilter_.setResonance (filterResonance_);
}

void SamplerEngine::process (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    for (const auto metadata : midi)
        handleMidi (metadata.getMessage());

    pendingPadHits_.clear();
    busFilter_.beginBlock();
    sequencer_.advance (hostSampleRate_, buffer.getNumSamples(), pendingPadHits_);
    for (const auto& hit : pendingPadHits_)
        triggerPad (hit.pad, hit.velocity, hit.tuneSemitones, hit.pan, hit.filterCutoff);

    buffer.clear();
    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : left;
    const int n = buffer.getNumSamples();

    for (int i = 0; i < n; ++i)
    {
        float sumL = 0.0f;
        float sumR = 0.0f;
        for (auto& v : voices_)
        {
            const float s = v.renderNextSample();
            sumL += s * v.lastPanLeft();
            sumR += s * v.lastPanRight();
        }
        left[i] = sumL;
        right[i] = sumR;
    }

    busFilter_.process (buffer);
}

std::optional<std::size_t> SamplerEngine::importFile (const juce::File& file, int bankIndex, juce::String name)
{
    auto result = importer_.importFromFile (file, bankIndex, std::move (name));
    if (! result.ok)
        return std::nullopt;
    return pool_.appendSegment (std::move (result.segment));
}

void SamplerEngine::assignSegmentToPad (int padIndex, int segmentIndex)
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return;
    pads_.pads[padIndex].segmentIndex = segmentIndex;
}

void SamplerEngine::setPadAssignment (int padIndex, PadAssignment assignment)
{
    if (padIndex >= 0 && padIndex < kNumPads)
        pads_.pads[padIndex] = assignment;
}

void SamplerEngine::setFilterCutoffNorm (float norm)
{
    filterCutoffNorm_ = std::clamp (norm, 0.0f, 1.0f);
    busFilter_.setBaseCutoffNorm (filterCutoffNorm_);
}

void SamplerEngine::setFilterResonance (float norm)
{
    filterResonance_ = std::clamp (norm, 0.0f, 1.0f);
    busFilter_.setResonance (filterResonance_);
}

void SamplerEngine::previewSegment (std::size_t segmentIndex, std::int64_t startSample, std::int64_t endSample)
{
    const auto* seg = pool_.getSegment (segmentIndex);
    if (seg == nullptr)
        return;

    const auto n = static_cast<std::int64_t> (seg->data.size());
    const auto start = std::clamp (startSample, std::int64_t { 0 }, n);
    const auto end = endSample < 0 ? n : std::clamp (endSample, start, n);
    if (end <= start)
        return;

    const int vi = findFreeVoice();
    voices_[static_cast<std::size_t> (vi)].start (&seg->data,
                                                 0.95f,
                                                 0.0f,
                                                 1.0f,
                                                 0.0f,
                                                 static_cast<std::size_t> (start),
                                                 static_cast<std::size_t> (end),
                                                 1.0f,
                                                 hostSampleRate_,
                                                 -1);
}

void SamplerEngine::setPadLevel (int padIndex, float level)
{
    if (padIndex >= 0 && padIndex < kNumPads)
        pads_.pads[padIndex].level = std::clamp (level, 0.0f, 2.0f);
}

void SamplerEngine::setPadTune (int padIndex, float semitones)
{
    if (padIndex >= 0 && padIndex < kNumPads)
        pads_.pads[padIndex].tuneSemitones = quantizeToMultiPitch (semitones);
}

void SamplerEngine::setPadDecay (int padIndex, float decayNorm)
{
    if (padIndex >= 0 && padIndex < kNumPads)
        pads_.pads[padIndex].decay = std::clamp (decayNorm, 0.0f, 1.0f);
}

PadAssignment SamplerEngine::getPad (int padIndex) const
{
    if (padIndex >= 0 && padIndex < kNumPads)
        return pads_.pads[padIndex];
    return {};
}

std::optional<std::size_t> SamplerEngine::commitRecording (const juce::AudioBuffer<float>& recorded,
                                                           double sourceRate,
                                                           int bankIndex)
{
    auto result = importer_.importFromAudioBuffer (recorded, sourceRate, bankIndex, "INPUT REC");
    if (! result.ok)
        return std::nullopt;
    return pool_.appendSegment (std::move (result.segment));
}

void SamplerEngine::startRecording()
{
    recording_ = true;
    recordBuffer_.setSize (1, 0);
    recordSourceRate_ = hostSampleRate_;
}

void SamplerEngine::cancelRecording()
{
    recording_ = false;
    recordBuffer_.setSize (1, 0);
}

std::optional<std::size_t> SamplerEngine::stopRecordingAndCommit (int bankIndex)
{
    recording_ = false;
    if (recordBuffer_.getNumSamples() <= 0)
        return std::nullopt;
    auto idx = commitRecording (recordBuffer_, recordSourceRate_, bankIndex);
    recordBuffer_.setSize (1, 0);
    return idx;
}

void SamplerEngine::triggerPadFromUi (int padIndex, float velocity)
{
    triggerPad (padIndex, velocity);
}

void SamplerEngine::recordStepOnCurrentPattern (int padIndex, float velocity)
{
    sequencer_.addStep (padIndex, sequencer_.recordStepCursor(), velocity, pads_.pads[padIndex].tuneSemitones);
    const int total = sequencer_.pattern (sequencer_.currentPattern()).totalSteps();
    sequencer_.setRecordStepCursor ((sequencer_.recordStepCursor() + 1) % std::max (1, total));
}

void SamplerEngine::appendRecording (const juce::AudioBuffer<float>& input)
{
    if (! recording_)
        return;

    const int ch = input.getNumChannels();
    const int n = input.getNumSamples();
    if (recordBuffer_.getNumSamples() == 0)
        recordBuffer_.setSize (1, 0);

    const int offset = recordBuffer_.getNumSamples();
    recordBuffer_.setSize (1, offset + n, true, false, true);
    float* dst = recordBuffer_.getWritePointer (0, offset);
    if (ch == 1)
    {
        juce::FloatVectorOperations::copy (dst, input.getReadPointer (0), n);
    }
    else
    {
        juce::FloatVectorOperations::clear (dst, n);
        for (int c = 0; c < ch; ++c)
            juce::FloatVectorOperations::add (dst, input.getReadPointer (c), n);
        const float inv = 1.0f / static_cast<float> (ch);
        juce::FloatVectorOperations::multiply (dst, inv, n);
    }
}

std::vector<std::size_t> SamplerEngine::autoChopSegment (std::size_t segmentIndex, int numSlices)
{
    std::vector<std::size_t> created;
    const auto* seg = pool_.getSegment (segmentIndex);
    if (seg == nullptr || numSlices <= 0)
        return created;

    const auto n = seg->data.size();
    std::vector<float> mono (n);
    seg->data.writeToFloat (mono.data(), n);

    const auto slices = detectTransientSlices (mono.data(),
                                               n,
                                               0,
                                               static_cast<std::int64_t> (n),
                                               numSlices,
                                               kSampleRateHz);
    if (slices.empty())
        return created;

    for (std::size_t i = 0; i < slices.size(); ++i)
    {
        const auto& sl = slices[i];
        const auto len = static_cast<std::size_t> (sl.endSample - sl.startSample);
        if (len == 0)
            continue;

        SampleSegment piece;
        piece.name = seg->name + "_CHOP" + std::to_string (i + 1);
        piece.bank = seg->bank;
        piece.data.resize (len);
        for (std::size_t s = 0; s < len; ++s)
            piece.data.setSample (s, seg->data.getSample (static_cast<std::size_t> (sl.startSample) + s));

        if (auto idx = pool_.appendSegment (std::move (piece)))
            created.push_back (*idx);
    }
    return created;
}

int SamplerEngine::findFreeVoice() noexcept
{
    for (int i = 0; i < kNumVoices; ++i)
        if (! voices_[static_cast<std::size_t> (i)].isActive())
            return i;

    int steal = 0;
    for (int i = 1; i < kNumVoices; ++i)
        if (voiceGeneration_[static_cast<std::size_t> (i)] < voiceGeneration_[static_cast<std::size_t> (steal)])
            steal = i;
    return steal;
}

void SamplerEngine::triggerPad (int padIndex, float velocity, float extraTune, float pan, float filterStep)
{
    busFilter_.pushStepModulation (filterStep);
    if (padIndex < 0 || padIndex >= kNumPads)
        return;

    int segIdx = pads_.pads[padIndex].segmentIndex;
    float tune = pads_.pads[padIndex].tuneSemitones + extraTune;

    if (multiPitch_.enabled && multiPitch_.sourceSegmentIndex >= 0)
    {
        segIdx = multiPitch_.sourceSegmentIndex;
        tune = multiPitch_.semitoneOffsets[static_cast<std::size_t> (padIndex)];
    }

    if (segIdx < 0)
        return;

    const auto* seg = pool_.getSegment (static_cast<std::size_t> (segIdx));
    if (seg == nullptr)
        return;

    tune = quantizeToMultiPitch (tune);

    for (auto& v : voices_)
        if (v.isActive() && v.padIndex() == padIndex)
            v.forceStop();

    const int vi = findFreeVoice();
    voiceGeneration_[static_cast<std::size_t> (vi)] = nextVoiceGen_++;
    voices_[static_cast<std::size_t> (vi)].start (&seg->data,
                                                 velocity,
                                                 tune,
                                                 pads_.pads[padIndex].level,
                                                 pan,
                                                 0,
                                                 0,
                                                 pads_.pads[padIndex].decay,
                                                 hostSampleRate_,
                                                 padIndex);
}

void SamplerEngine::handleMidi (const juce::MidiMessage& msg)
{
    if (msg.getChannel() != kDefaultMidiChannel)
        return;

    if (msg.isNoteOn())
    {
        const int note = msg.getNoteNumber();
        if (note >= kPadNoteStart && note < kPadNoteEnd)
            triggerPad (note - kPadNoteStart, msg.getFloatVelocity());
    }
    else if (msg.isController())
    {
        const int cc = msg.getControllerNumber();
        const int pad = cc - kFaderCcStart;
        if (pad >= 0 && pad < kNumPads && cc >= kFaderCcStart && cc < kFaderCcEnd)
        {
            const float norm = static_cast<float> (msg.getControllerValue()) / 127.0f;
            if (faderMode_ == 1)
                setPadTune (pad, (norm - 0.5f) * 24.0f);
            else if (faderMode_ == 2)
                setPadDecay (pad, norm);
            else
                setPadLevel (pad, norm * 2.0f);
        }
    }
}

} // namespace sp1200
