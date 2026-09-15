#pragma once

#include "AudioConstants.h"
#include "SynthVoice.h"

#include <array>
#include <cstdint>

namespace jdupgraded::dsp
{

using TonePatchSet = std::array<TonePatch, kTonesPerVoice>;

class VoicePool final
{
public:
    void prepare (double sampleRate, std::size_t maxBlockSize) noexcept
    {
        sampleRate_ = sampleRate;
        maxBlockSize_ = maxBlockSize;
        for (auto& voice : voices_)
            voice.setSampleRate (sampleRate);
    }

    void setDefaultPatches (const TonePatchSet& patches) noexcept
    {
        defaultPatches_ = patches;
    }

    void setCouplingMode (ToneCouplingMode mode) noexcept
    {
        for (auto& voice : voices_)
            voice.setCouplingMode (mode);
    }

    void noteOn (std::uint8_t midiNote, std::uint8_t velocity) noexcept
    {
        const int voiceIndex = allocateVoice (midiNote);
        if (voiceIndex < 0)
            return;

        voices_[static_cast<std::size_t> (voiceIndex)].start (midiNote, velocity, defaultPatches_);
    }

    void noteOff (std::uint8_t midiNote) noexcept
    {
        for (auto& voice : voices_)
        {
            if (voice.isActive() && voice.getMidiNote() == midiNote)
                voice.release();
        }
    }

    void allNotesOff() noexcept
    {
        for (auto& voice : voices_)
            voice.release();
    }

    void render (float* output, std::size_t numSamples) noexcept
    {
        for (std::size_t i = 0; i < numSamples; ++i)
            output[i] = 0.0f;

        for (auto& voice : voices_)
        {
            if (! voice.isActive())
                continue;

            voice.render (mixScratch_.data(), numSamples);
            for (std::size_t i = 0; i < numSamples; ++i)
                output[i] += mixScratch_[i];
        }
    }

private:
    int allocateVoice (std::uint8_t midiNote) noexcept
    {
        int freeIndex = -1;
        int stealIndex = -1;

        for (std::size_t i = 0; i < voices_.size(); ++i)
        {
            if (! voices_[i].isActive())
            {
                freeIndex = static_cast<int> (i);
                break;
            }

            if (voices_[i].getMidiNote() == midiNote)
                stealIndex = static_cast<int> (i);
        }

        if (freeIndex >= 0)
            return freeIndex;

        if (stealIndex >= 0)
            return stealIndex;

        return static_cast<int> (nextStealIndex_++ % voices_.size());
    }

    double sampleRate_ = 44100.0;
    std::size_t maxBlockSize_ = 512;
    std::array<SynthVoice, kMaxPolyphony> voices_{};
    TonePatchSet defaultPatches_{};
    std::array<float, 8192> mixScratch_{};
    std::size_t nextStealIndex_ = 0;
};

} // namespace jdupgraded::dsp
