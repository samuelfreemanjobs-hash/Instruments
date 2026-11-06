#include "AutoRecordEngine.h"

namespace autosampler
{

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
    currentRootNote_ = -1;
    capture_.setSize (0, 0);
    captureWritePos_ = 0;
    samplesUntilNextAction_ = 0;
    targetRecordSamples_ = 0;
    noteHeld_ = false;
}

void AutoRecordEngine::buildNoteList()
{
    noteList_.clear();
    if (config_.noteStep < 1)
        config_.noteStep = 1;

    const int lo = juce::jlimit (0, 127, config_.lowNote);
    const int hi = juce::jlimit (lo, 127, config_.highNote);

    for (int n = lo; n <= hi; n += config_.noteStep)
        noteList_.push_back (n);
}

void AutoRecordEngine::start (Config config)
{
    reset();
    config_ = config;
    buildNoteList();

    if (noteList_.empty())
    {
        state_ = AutoSampleState::finished;
        return;
    }

    targetRecordSamples_ = static_cast<int> (std::ceil (config_.recordSeconds * sampleRate_));
    state_ = AutoSampleState::waitingGap;
    samplesUntilNextAction_ = static_cast<int> (std::ceil (config_.gapSeconds * sampleRate_));
    noteIndex_ = 0;
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

    const float base = static_cast<float> (noteIndex_) / static_cast<float> (noteList_.size());
    if (state_ == AutoSampleState::recordingNote && targetRecordSamples_ > 0)
    {
        const float inNote = static_cast<float> (captureWritePos_) / static_cast<float> (targetRecordSamples_);
        return juce::jlimit (0.0f, 1.0f, base + inNote / static_cast<float> (noteList_.size()));
    }

    return juce::jlimit (0.0f, 1.0f, base);
}

void AutoRecordEngine::beginNote (int rootNote)
{
    currentRootNote_ = rootNote;
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

    Zone z;
    z.rootNote = currentRootNote_;
    z.sampleRate = sampleRate_;
    z.audio.setSize (capture_.getNumChannels(), captureWritePos_, false, false, true);

    for (int ch = 0; ch < capture_.getNumChannels(); ++ch)
        z.audio.copyFrom (ch, 0, capture_, ch, 0, captureWritePos_);

    SamplePostProcessor::processZone (z, config_.postProcess);
    destination.addZone (std::move (z));
    currentRootNote_ = -1;
    captureWritePos_ = 0;
}

void AutoRecordEngine::advanceSchedule (int samplesInBlock) noexcept
{
    samplesUntilNextAction_ -= samplesInBlock;
    if (samplesUntilNextAction_ > 0)
        return;

    samplesUntilNextAction_ = 0;

    if (noteIndex_ >= noteList_.size())
    {
        state_ = AutoSampleState::finished;
        return;
    }

    beginNote (noteList_[noteIndex_++]);
}

void AutoRecordEngine::process (const juce::AudioBuffer<float>& input,
                                juce::MidiBuffer& midiOut,
                                MultisampleProgram& destination)
{
    if (state_ == AutoSampleState::idle || state_ == AutoSampleState::finished || state_ == AutoSampleState::aborted)
        return;

    const int numSamples = input.getNumSamples();
    int pos = 0;

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
                beginNote (noteList_[noteIndex_++]);

            continue;
        }

        if (state_ == AutoSampleState::recordingNote)
        {
            if (noteHeld_ && config_.midiThru && currentRootNote_ >= 0)
            {
                midiOut.addEvent (juce::MidiMessage::noteOn (1, currentRootNote_, (juce::uint8) config_.velocity), pos);
                noteHeld_ = false;
            }

            const int inCh = input.getNumChannels();
            const int capCh = capture_.getNumChannels();
            const int toCopy = juce::jmin (numSamples - pos, targetRecordSamples_ - captureWritePos_);

            for (int i = 0; i < toCopy; ++i)
            {
                for (int ch = 0; ch < capCh; ++ch)
                {
                    const float s = inCh > 0 ? input.getSample (juce::jmin (ch, inCh - 1), pos + i) : 0.0f;
                    capture_.setSample (ch, captureWritePos_ + i, s);
                }
            }

            pos += toCopy;
            captureWritePos_ += toCopy;

            if (captureWritePos_ >= targetRecordSamples_)
            {
                if (config_.midiThru && currentRootNote_ >= 0)
                    midiOut.addEvent (juce::MidiMessage::noteOff (1, currentRootNote_), pos - 1);

                finalizeCurrentZone (destination);

                if (noteIndex_ >= noteList_.size())
                {
                    state_ = AutoSampleState::finished;
                    return;
                }

                state_ = AutoSampleState::waitingGap;
                samplesUntilNextAction_ = static_cast<int> (std::ceil (config_.gapSeconds * sampleRate_));
            }
        }
        else
        {
            break;
        }
    }
}

} // namespace autosampler
