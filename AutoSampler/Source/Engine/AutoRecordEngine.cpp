#include "AutoRecordEngine.h"

namespace autosampler
{

namespace
{
std::pair<int, int> velocitySpanForLayer (int layerIndex, int layerCount, int layerVelocity)
{
    if (layerCount <= 1)
        return { 0, 127 };

    const int prevVel = layerIndex > 0 ? layerVelocity - 32 : 0;
    const int nextVel = layerIndex + 1 < layerCount ? layerVelocity + 32 : 127;
    juce::ignoreUnused (prevVel, nextVel);

    const int velStart = layerIndex == 0 ? 0 : (layerIndex * 127) / layerCount;
    const int velEnd = layerIndex + 1 == layerCount ? 127 : ((layerIndex + 1) * 127) / layerCount - 1;
    return { velStart, juce::jmax (velStart, velEnd) };
}
} // namespace

void AutoRecordEngine::prepare (double sampleRate, int maxBlockSize)
{
    sampleRate_ = sampleRate;
    juce::ignoreUnused (maxBlockSize);
}

void AutoRecordEngine::reset() noexcept
{
    state_ = AutoSampleState::idle;
    noteList_.clear();
    noteIndex_ = 0;
    velocityPassIndex_ = 0;
    currentRootNote_ = -1;
    currentVelocity_ = 100;
    capture_.setSize (0, 0);
    captureWritePos_ = 0;
    samplesUntilNextAction_ = 0;
    targetRecordSamples_ = 0;
    noteHeld_ = false;
    trailingSilentSamples_ = 0;
    noteOffSent_ = false;
}

void AutoRecordEngine::buildNoteList()
{
    noteList_ = NoteMapBuilder::buildNoteList (config_.lowNote,
                                                config_.highNote,
                                                config_.noteStep,
                                                config_.noteMapPreset,
                                                config_.blackKeyPolicy);
}

void AutoRecordEngine::start (Config config)
{
    reset();
    config_ = config;
    if (config_.velocityLayers.empty())
        config_.velocityLayers.push_back (config_.velocity);

    buildNoteList();

    if (noteList_.empty())
    {
        state_ = AutoSampleState::finished;
        return;
    }

    const double recordSec = config_.recordUntilSilence ? config_.maxRecordSeconds : config_.recordSeconds;
    targetRecordSamples_ = static_cast<int> (std::ceil (recordSec * sampleRate_));
    state_ = AutoSampleState::waitingGap;
    samplesUntilNextAction_ = static_cast<int> (std::ceil (config_.gapSeconds * sampleRate_));
    noteIndex_ = 0;
    velocityPassIndex_ = 0;
    trailingSilentSamples_ = 0;
    noteOffSent_ = false;
}

void AutoRecordEngine::abort() noexcept
{
    if (noteHeld_ && config_.midiThru)
        noteHeld_ = false;

    state_ = AutoSampleState::aborted;
    currentRootNote_ = -1;
}

float AutoRecordEngine::progress01() const noexcept
{
    if (noteList_.empty())
        return state_ == AutoSampleState::finished ? 1.0f : 0.0f;

    const int velPasses = juce::jmax (1, static_cast<int> (config_.multiVelocityLayers ? config_.velocityLayers.size() : 1));
    const float totalSteps = static_cast<float> (noteList_.size() * static_cast<std::size_t> (velPasses));
    const float doneNotes = static_cast<float> (noteIndex_ * static_cast<std::size_t> (velPasses) + velocityPassIndex_);

    if (state_ == AutoSampleState::recordingNote && targetRecordSamples_ > 0)
    {
        const float inNote = static_cast<float> (captureWritePos_) / static_cast<float> (targetRecordSamples_);
        return juce::jlimit (0.0f, 1.0f, (doneNotes + inNote) / totalSteps);
    }

    return juce::jlimit (0.0f, 1.0f, doneNotes / totalSteps);
}

void AutoRecordEngine::beginNote (int rootNote)
{
    currentRootNote_ = rootNote;
    currentVelocity_ = config_.multiVelocityLayers && velocityPassIndex_ < config_.velocityLayers.size()
                           ? config_.velocityLayers[static_cast<std::size_t> (velocityPassIndex_)]
                           : config_.velocity;

    const double recordSec = config_.recordUntilSilence ? config_.maxRecordSeconds : config_.recordSeconds;
    targetRecordSamples_ = static_cast<int> (std::ceil (recordSec * sampleRate_));

    const int channels = juce::jmax (1, 2);
    capture_.setSize (channels, targetRecordSamples_, false, false, true);
    capture_.clear();
    captureWritePos_ = 0;
    noteHeld_ = true;
    state_ = AutoSampleState::recordingNote;
}

void AutoRecordEngine::finalizeCurrentZone (MultisampleProgram& destination)
{
    if (currentRootNote_ < 0 || captureWritePos_ <= 0)
        return;

    Zone scratch;
    scratch.rootNote = currentRootNote_;
    scratch.sampleRate = sampleRate_;
    scratch.audio.setSize (capture_.getNumChannels(), captureWritePos_, false, false, true);

    for (int ch = 0; ch < capture_.getNumChannels(); ++ch)
        scratch.audio.copyFrom (ch, 0, capture_, ch, 0, captureWritePos_);

    SamplePostProcessor::processZone (scratch, config_.postProcess);

    VelocityLayer layer;
    layer.audio = std::move (scratch.audio);
    layer.loopStart = scratch.loopStart;
    layer.loopEnd = scratch.loopEnd;
    layer.loopConfidence = scratch.loopConfidence;

    const int velPasses = config_.multiVelocityLayers ? static_cast<int> (config_.velocityLayers.size()) : 1;
    const auto [velStart, velEnd] = velocitySpanForLayer (static_cast<int> (velocityPassIndex_),
                                                          velPasses,
                                                          currentVelocity_);
    layer.velStart = velStart;
    layer.velEnd = velEnd;

    const int existing = destination.zoneIndexForRoot (currentRootNote_);
    if (existing >= 0)
    {
        destination.zones()[static_cast<std::size_t> (existing)].addVelocityLayer (std::move (layer));
    }
    else
    {
        Zone z;
        z.rootNote = currentRootNote_;
        z.sampleRate = sampleRate_;
        z.addVelocityLayer (std::move (layer));
        destination.addZone (std::move (z));
    }

    currentRootNote_ = -1;
    captureWritePos_ = 0;
}

void AutoRecordEngine::advanceSchedule (int samplesInBlock) noexcept
{
    juce::ignoreUnused (samplesInBlock);
    samplesUntilNextAction_ = 0;
}

void AutoRecordEngine::process (const juce::AudioBuffer<float>& input,
                                juce::MidiBuffer& midiOut,
                                MultisampleProgram& destination)
{
    if (state_ == AutoSampleState::idle || state_ == AutoSampleState::finished || state_ == AutoSampleState::aborted)
        return;

    const int numSamples = input.getNumSamples();
    int pos = 0;
    const float silenceThresh = juce::Decibels::decibelsToGain (config_.silenceThresholdDb);
    const int silenceHoldSamples = static_cast<int> (config_.silenceHoldSeconds * sampleRate_);
    while (pos < numSamples)
    {
        if (state_ == AutoSampleState::waitingGap)
        {
            if (noteIndex_ >= noteList_.size())
            {
                state_ = AutoSampleState::finished;
                return;
            }

            const int take = juce::jmin (numSamples - pos, samplesUntilNextAction_);
            pos += take;
            samplesUntilNextAction_ -= take;

            if (samplesUntilNextAction_ <= 0)
            {
                trailingSilentSamples_ = 0;
                noteOffSent_ = false;
                beginNote (noteList_[noteIndex_]);
            }

            continue;
        }

        if (state_ == AutoSampleState::recordingNote)
        {
            if (noteHeld_ && config_.midiThru && currentRootNote_ >= 0)
            {
                midiOut.addEvent (juce::MidiMessage::noteOn (1, currentRootNote_, (juce::uint8) currentVelocity_), pos);
                noteHeld_ = false;
            }

            if (! noteOffSent_ && config_.noteOffAtRatio > 0.0
                && captureWritePos_ >= static_cast<int> (static_cast<double> (targetRecordSamples_) * config_.noteOffAtRatio))
            {
                if (config_.midiThru && currentRootNote_ >= 0)
                    midiOut.addEvent (juce::MidiMessage::noteOff (1, currentRootNote_), pos);
                noteOffSent_ = true;
            }

            const int inCh = input.getNumChannels();
            const int capCh = capture_.getNumChannels();
            const int toCopy = juce::jmin (numSamples - pos, targetRecordSamples_ - captureWritePos_);

            for (int i = 0; i < toCopy; ++i)
            {
                float peak = 0.0f;
                for (int ch = 0; ch < capCh; ++ch)
                {
                    const float s = inCh > 0 ? input.getSample (juce::jmin (ch, inCh - 1), pos + i) : 0.0f;
                    capture_.setSample (ch, captureWritePos_ + i, s);
                    peak = juce::jmax (peak, std::abs (s));
                }

                if (config_.recordUntilSilence && noteOffSent_)
                {
                    if (peak < silenceThresh)
                        trailingSilentSamples_++;
                    else
                        trailingSilentSamples_ = 0;
                }
            }

            pos += toCopy;
            captureWritePos_ += toCopy;

            const bool silenceDone = config_.recordUntilSilence && noteOffSent_ && trailingSilentSamples_ >= silenceHoldSamples;
            const bool timedDone = captureWritePos_ >= targetRecordSamples_;

            if (timedDone || silenceDone)
            {
                finalizeCurrentZone (destination);

                const int velPasses = config_.multiVelocityLayers ? static_cast<int> (config_.velocityLayers.size()) : 1;
                if (config_.multiVelocityLayers && velocityPassIndex_ + 1 < static_cast<std::size_t> (velPasses))
                {
                    ++velocityPassIndex_;
                    state_ = AutoSampleState::waitingGap;
                    samplesUntilNextAction_ = static_cast<int> (std::ceil (config_.gapSeconds * sampleRate_));
                    trailingSilentSamples_ = 0;
                    noteOffSent_ = false;
                }
                else
                {
                    velocityPassIndex_ = 0;
                    ++noteIndex_;
                    if (noteIndex_ >= noteList_.size())
                    {
                        state_ = AutoSampleState::finished;
                        return;
                    }
                    state_ = AutoSampleState::waitingGap;
                    samplesUntilNextAction_ = static_cast<int> (std::ceil (config_.gapSeconds * sampleRate_));
                }
            }
        }
        else
        {
            break;
        }
    }
}

} // namespace autosampler
