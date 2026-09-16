#pragma once

#include "AudioConstants.h"
#include "Modulation/ToneCoupling.h"
#include "Simd/VoiceSimd.h"
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

    void setCouplingMode (ToneCouplingMode mode) noexcept
    {
        couplingMode_ = mode;
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
        configureCouplingLinks();
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
        std::array<std::array<float, kControlRateDivisor>, kTonesPerVoice> toneScratch{};
        const std::size_t chunk = kControlRateDivisor;

        for (std::size_t offset = 0; offset < numSamples; offset += chunk)
        {
            const std::size_t n = std::min (chunk, numSamples - offset);

            if (couplingMode_ == ToneCouplingMode::independent)
            {
                for (std::size_t t = 0; t < kTonesPerVoice; ++t)
                    tones_[t].render (toneScratch[t].data(), n);

                const std::array<const float*, kTonesPerVoice> ptrs = {
                    toneScratch[0].data(), toneScratch[1].data(),
                    toneScratch[2].data(), toneScratch[3].data()
                };
                sumFourToneBuffers (output + offset, ptrs[0], ptrs[1], ptrs[2], ptrs[3], n);
            }
            else
            {
                for (std::size_t i = 0; i < n; ++i)
                    output[offset + i] = 0.0f;
                renderWithCoupling (output, offset, n);
            }

            for (std::size_t i = 0; i < n; ++i)
                output[offset + i] *= velocityGain_;
        }
    }

private:
    void configureCouplingLinks() noexcept
    {
        for (auto& tone : tones_)
            tone.setHardSyncMaster (nullptr);

        switch (couplingMode_)
        {
            case ToneCouplingMode::hardSyncPair01:
                tones_[1].setHardSyncMaster (&tones_[0].getEngine());
                break;
            case ToneCouplingMode::hardSyncPair23:
                tones_[3].setHardSyncMaster (&tones_[2].getEngine());
                break;
            case ToneCouplingMode::independent:
            case ToneCouplingMode::crossModPair01:
            case ToneCouplingMode::crossModPair23:
            case ToneCouplingMode::ringPair01:
            case ToneCouplingMode::ringPair23:
            default:
                break;
        }
    }

    void renderWithCoupling (float* output, std::size_t offset, std::size_t n) noexcept
    {
        for (std::size_t i = 0; i < n; ++i)
        {
            std::array<float, kTonesPerVoice> osc{};

            for (std::size_t t = 0; t < kTonesPerVoice; ++t)
            {
                if (! tones_[t].isActive())
                    continue;

                if (needsCrossMod (t))
                    tones_[t].setPhaseModSource (tones_[crossModSourceIndex (t)].getLastOscillatorSample());

                osc[t] = tones_[t].renderOscillatorSample();
            }

            float sum = 0.0f;
            for (std::size_t t = 0; t < kTonesPerVoice; ++t)
            {
                if (! tones_[t].isActive())
                    continue;

                float ringPartner = 0.0f;
                float ringAmt = 0.0f;

                if (couplingMode_ == ToneCouplingMode::ringPair01 && (t == 0 || t == 1))
                {
                    ringPartner = osc[t == 0 ? 1 : 0];
                    ringAmt = 0.5f;
                }
                else if (couplingMode_ == ToneCouplingMode::ringPair23 && (t == 2 || t == 3))
                {
                    ringPartner = osc[t == 2 ? 3 : 2];
                    ringAmt = 0.5f;
                }

                sum += tones_[t].applyFilterAndAmp (osc[t], ringPartner, ringAmt);
            }

            output[offset + i] += sum;
        }
    }

    bool needsCrossMod (std::size_t toneIndex) const noexcept
    {
        if (couplingMode_ == ToneCouplingMode::crossModPair01 && toneIndex == 1)
            return true;
        if (couplingMode_ == ToneCouplingMode::crossModPair23 && toneIndex == 3)
            return true;
        return false;
    }

    std::size_t crossModSourceIndex (std::size_t toneIndex) const noexcept
    {
        if (toneIndex == 1)
            return 0;
        return 2;
    }

    std::array<Tone, kTonesPerVoice> tones_{};
    ToneCouplingMode couplingMode_ = ToneCouplingMode::independent;
    std::uint8_t midiNote_ = 0;
    float velocityGain_ = 1.0f;
    bool active_ = false;
};

} // namespace jdupgraded::dsp
