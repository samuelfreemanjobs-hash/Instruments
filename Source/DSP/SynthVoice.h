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

                sumFourToneBuffers (output + offset, toneScratch[0].data(), toneScratch[1].data(),
                                    toneScratch[2].data(), toneScratch[3].data(), n);
            }
            else
            {
                for (std::size_t i = 0; i < n; ++i)
                    output[offset + i] = 0.0f;
                renderWithCoupling (output, offset, n, toneScratch);
            }

            scaleBuffer (output + offset, velocityGain_, n);
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

    void renderIndependentTones (float* output,
                                 std::size_t offset,
                                 std::size_t n,
                                 std::array<std::array<float, kControlRateDivisor>, kTonesPerVoice>& scratch,
                                 const std::array<bool, kTonesPerVoice>& include) noexcept
    {
        for (std::size_t t = 0; t < kTonesPerVoice; ++t)
        {
            if (! include[t])
                continue;

            tones_[t].render (scratch[t].data(), n);
            addBuffers (output + offset, scratch[t].data(), n);
        }
    }

    void renderWithCoupling (float* output,
                             std::size_t offset,
                             std::size_t n,
                             std::array<std::array<float, kControlRateDivisor>, kTonesPerVoice>& scratch) noexcept
    {
        switch (couplingMode_)
        {
            case ToneCouplingMode::ringPair01:
                renderIndependentTones (output, offset, n, scratch, { false, false, true, true });
                renderCoupledSampleLoop (output, offset, n, ToneCouplingMode::ringPair01, 0, 1);
                return;
            case ToneCouplingMode::ringPair23:
                renderIndependentTones (output, offset, n, scratch, { true, true, false, false });
                renderCoupledSampleLoop (output, offset, n, ToneCouplingMode::ringPair23, 2, 3);
                return;
            case ToneCouplingMode::crossModPair01:
                renderIndependentTones (output, offset, n, scratch, { false, false, true, true });
                renderCoupledSampleLoop (output, offset, n, ToneCouplingMode::crossModPair01, 0, 1);
                return;
            case ToneCouplingMode::crossModPair23:
                renderIndependentTones (output, offset, n, scratch, { true, true, false, false });
                renderCoupledSampleLoop (output, offset, n, ToneCouplingMode::crossModPair23, 2, 3);
                return;
            case ToneCouplingMode::hardSyncPair01:
                renderIndependentTones (output, offset, n, scratch, { false, false, true, true });
                renderCoupledSampleLoop (output, offset, n, ToneCouplingMode::hardSyncPair01, 0, 1);
                return;
            case ToneCouplingMode::hardSyncPair23:
                renderIndependentTones (output, offset, n, scratch, { true, true, false, false });
                renderCoupledSampleLoop (output, offset, n, ToneCouplingMode::hardSyncPair23, 2, 3);
                return;
            default:
                break;
        }

        renderCoupledSampleLoop (output, offset, n, couplingMode_, 0, 3);
    }

    void renderCoupledSampleLoop (float* output,
                                  std::size_t offset,
                                  std::size_t n,
                                  ToneCouplingMode mode,
                                  std::size_t pairLow,
                                  std::size_t pairHigh) noexcept
    {
        const bool ring = mode == ToneCouplingMode::ringPair01 || mode == ToneCouplingMode::ringPair23;
        const bool crossLow = mode == ToneCouplingMode::crossModPair01 && pairLow == 0;
        const bool crossHigh = mode == ToneCouplingMode::crossModPair23 && pairLow == 2;

        for (std::size_t i = 0; i < n; ++i)
        {
            std::array<float, kTonesPerVoice> osc{};

            for (std::size_t t = pairLow; t <= pairHigh; ++t)
            {
                if (! tones_[t].isActive())
                    continue;

                if ((crossLow && t == 1) || (crossHigh && t == 3))
                    tones_[t].setPhaseModSource (tones_[crossModSourceIndex (t)].getLastOscillatorSample());

                osc[t] = tones_[t].renderOscillatorSample();
            }

            for (std::size_t t = pairLow; t <= pairHigh; ++t)
            {
                if (! tones_[t].isActive())
                    continue;

                float ringPartner = 0.0f;
                float ringAmt = 0.0f;

                if (ring)
                {
                    const std::size_t partner = t == pairLow ? pairHigh : pairLow;
                    ringPartner = osc[partner];
                    ringAmt = 0.5f;
                }

                output[offset + i] += tones_[t].applyFilterAndAmp (osc[t], ringPartner, ringAmt);
            }
        }
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
