#include "Arpeggiator.h"

#include <algorithm>
#include <cmath>

namespace junovax::dsp
{
void Arpeggiator::prepare (double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    reset();
}

void Arpeggiator::reset() noexcept
{
    heldCount_ = 0;
    stepIndex_ = 0;
    playingNote_ = -1;
    samplesUntilStep_ = 0.0;
    nextStepPpq_ = 0.0;
    freeRunPpq_ = 0.0;
    held_.fill (-1);
}

double Arpeggiator::stepPpq (float arpRate) const noexcept
{
    static constexpr double kSteps[] = { 0.25, 0.5, 1.0, 2.0 };
    const int idx = juce::jlimit (0, 3, static_cast<int> (std::floor (juce::jlimit (0.0f, 1.0f, arpRate) * 4.0f)));
    return kSteps[static_cast<std::size_t> (idx)];
}

void Arpeggiator::ingestMidi (const juce::MidiBuffer& input, bool latch) noexcept
{
    for (const auto metadata : input)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            const int n = msg.getNoteNumber();
            const bool exists = std::any_of (held_.begin(), held_.begin() + heldCount_, [n] (int h) { return h == n; });
            if (! exists && heldCount_ < kMaxHeld)
                held_[static_cast<std::size_t> (heldCount_++)] = n;
        }
        else if (msg.isNoteOff())
        {
            if (latch)
                continue;
            const int n = msg.getNoteNumber();
            for (int i = 0; i < heldCount_; ++i)
            {
                if (held_[static_cast<std::size_t> (i)] == n)
                {
                    held_[static_cast<std::size_t> (i)] = held_[static_cast<std::size_t> (heldCount_ - 1)];
                    --heldCount_;
                    break;
                }
            }
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            reset();
        }
    }

    if (heldCount_ > 1)
        std::sort (held_.begin(), held_.begin() + heldCount_);
}

void Arpeggiator::emitStep (juce::MidiBuffer& output, int samplePos, int rangeOct) noexcept
{
    if (heldCount_ <= 0)
        return;

    if (playingNote_ >= 0)
        output.addEvent (juce::MidiMessage::noteOff (1, playingNote_), samplePos);

    const int notesPerCycle = juce::jmax (1, heldCount_ * rangeOct);
    const int idx = stepIndex_ % notesPerCycle;
    const int noteIdx = idx % heldCount_;
    const int octave = idx / heldCount_;
    const int n = juce::jlimit (0, 127, held_[static_cast<std::size_t> (noteIdx)] + octave * 12);

    output.addEvent (juce::MidiMessage::noteOn (1, n, static_cast<juce::uint8> (100)), samplePos);
    playingNote_ = n;
    ++stepIndex_;
}

void Arpeggiator::schedulePpqSteps (juce::MidiBuffer& output,
                                    float arpRate,
                                    float arpRange,
                                    const ArpHostContext& host,
                                    int numSamples) noexcept
{
    const int rangeOct = juce::jlimit (1, 4, static_cast<int> (std::lround (arpRange)));
    const double bpm = juce::jmax (20.0, host.bpm);
    const double ppqPerSample = (bpm / 60.0) / sampleRate_;
    const double blockStart = host.ppqValid ? host.ppqPosition : freeRunPpq_;
    const double blockEnd = blockStart + ppqPerSample * static_cast<double> (numSamples);

    if (! host.ppqValid)
        freeRunPpq_ = blockEnd;

    const double step = stepPpq (arpRate);

    if (heldCount_ > 0 && playingNote_ < 0)
        nextStepPpq_ = blockStart;

    if (nextStepPpq_ < blockStart)
        nextStepPpq_ = blockStart;

    while (nextStepPpq_ < blockEnd + 1.0e-9)
    {
        const double ppqIntoBlock = nextStepPpq_ - blockStart;
        const int samplePos = juce::jlimit (0, numSamples - 1,
                                            static_cast<int> (std::lround (ppqIntoBlock / ppqPerSample)));
        emitStep (output, samplePos, rangeOct);
        nextStepPpq_ += step;
    }
}

void Arpeggiator::process (const juce::MidiBuffer& input,
                           juce::MidiBuffer& output,
                           float arpRate,
                           float arpRange,
                           bool latch,
                           const ArpHostContext& host,
                           int numSamples) noexcept
{
    ingestMidi (input, latch);

    if (arpRate <= 0.01f)
    {
        output = input;
        return;
    }

    output.clear();

    for (const auto metadata : input)
    {
        const auto msg = metadata.getMessage();
        if (! msg.isNoteOn() && ! msg.isNoteOff())
            output.addEvent (msg, metadata.samplePosition);
    }

    if (heldCount_ == 0)
    {
        if (playingNote_ >= 0)
        {
            output.addEvent (juce::MidiMessage::noteOff (1, playingNote_), 0);
            playingNote_ = -1;
        }
        return;
    }

    schedulePpqSteps (output, arpRate, arpRange, host, numSamples);
}
} // namespace junovax::dsp
