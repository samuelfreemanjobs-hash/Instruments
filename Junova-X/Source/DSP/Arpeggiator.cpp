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
    held_.fill (-1);
}

float Arpeggiator::stepPeriodSamples (float arpRate, double bpm) const noexcept
{
    const double beatsPerStep = 0.25 + (1.0 - static_cast<double> (juce::jlimit (0.0f, 1.0f, arpRate))) * 1.75;
    const double sec = (60.0 / juce::jmax (20.0, bpm)) * beatsPerStep;
    return static_cast<float> (sec * sampleRate_);
}

void Arpeggiator::ingestMidi (const juce::MidiBuffer& input) noexcept
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

void Arpeggiator::process (const juce::MidiBuffer& input,
                           juce::MidiBuffer& output,
                           float arpRate,
                           float arpRange,
                           double bpm,
                           int numSamples) noexcept
{
    ingestMidi (input);

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

    const int rangeOct = juce::jlimit (1, 4, static_cast<int> (std::lround (arpRange)));

    if (heldCount_ == 0)
    {
        if (playingNote_ >= 0)
        {
            output.addEvent (juce::MidiMessage::noteOff (1, playingNote_), 0);
            playingNote_ = -1;
        }
        return;
    }

    if (heldCount_ > 0 && playingNote_ < 0)
        samplesUntilStep_ = 0.0;

    samplesUntilStep_ -= static_cast<double> (numSamples);

    while (samplesUntilStep_ <= 0.0)
    {
        if (playingNote_ >= 0)
            output.addEvent (juce::MidiMessage::noteOff (1, playingNote_), 0);

        const int notesPerCycle = juce::jmax (1, heldCount_ * rangeOct);
        const int idx = stepIndex_ % notesPerCycle;
        const int noteIdx = idx % heldCount_;
        const int octave = idx / heldCount_;
        const int n = juce::jlimit (0, 127, held_[static_cast<std::size_t> (noteIdx)] + octave * 12);

        output.addEvent (juce::MidiMessage::noteOn (1, n, static_cast<juce::uint8> (100)), 0);
        playingNote_ = n;
        ++stepIndex_;
        samplesUntilStep_ += stepPeriodSamples (arpRate, bpm);
    }
}
} // namespace junovax::dsp
