#pragma once

#include "AudioConstants.h"
#include "Tone.h"

#include <array>
#include <cstdint>

namespace jdupgraded::dsp
{

class SynthVoice final
{
public:
    void setSampleRate (double sampleRate) noexcept
    {
        for (auto& tone : tones_)
            tone.setSampleRate (sampleRate);
    }

    void start (std::uint8_t midiNote, std::uint8_t velocity,
                const std::array<TonePatch, kTonesPerVoice>& patches) noexcept
    {
        midiNote_ = midiNote;
        velocityGain_ = static_cast<float> (velocity) / 127.0f;
        for (std::size_t t = 0; t < kTonesPerVoice; ++t)
        {
            tones_[t].applyPatch (patches[t]);
            tones_[t].start (midiNote);
        }
        active_ = true;
    }

    void release() noexcept
    {
        for (auto& tone : tones_)
            tone.release();
    }

    bool isActive() noexcept
    {
        if (! active_)
            return false;

        for (const auto& tone : tones_)
            if (tone.isActive())
                return true;

        active_ = false;
        return false;
    }

    std::uint8_t getMidiNote() const noexcept { return midiNote_; }

    void render (float* output, std::size_t numSamples) noexcept
    {
        std::array<float, kControlRateDivisor> toneScratch{};
        const std::size_t chunk = kControlRateDivisor;

        for (std::size_t offset = 0; offset < numSamples; offset += chunk)
        {
            const std::size_t n = std::min (chunk, numSamples - offset);
            for (std::size_t i = 0; i < n; ++i)
                output[offset + i] = 0.0f;

            for (auto& tone : tones_)
            {
                tone.render (toneScratch.data(), n);
                for (std::size_t i = 0; i < n; ++i)
                    output[offset + i] += toneScratch[i];
            }

            for (std::size_t i = 0; i < n; ++i)
                output[offset + i] *= velocityGain_;
        }
    }

private:
    std::array<Tone, kTonesPerVoice> tones_{};
    std::uint8_t midiNote_ = 0;
    float velocityGain_ = 1.0f;
    bool active_ = false;
};

} // namespace jdupgraded::dsp
