#pragma once

#include "Engine/SynthParams.h"
#include "DSP/AdsrEnvelope.h"
#include "DSP/DspUtils.h"
#include "DSP/Effects/DestructiveStage.h"
#include "DSP/Effects/TapeWobble.h"
#include "DSP/Filter/ZdfMultimode.h"
#include "DSP/Oversampler2x.h"
#include "DSP/WavetableBank.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <algorithm>

namespace wave9090::engine
{

class Voice final
{
public:
    static constexpr int kUnisonVoices = 7;

    void prepare (double sampleRate, const dsp::WavetableBank& bank) noexcept
    {
        sampleRate_ = sampleRate;
        bank_ = &bank;
        filter_.prepare (sampleRate);
        filter_.setMode (dsp::ZdfMultimode::Mode::lowPass24);
        noiseLp_.prepare (sampleRate);
        noiseLp_.setMode (dsp::ZdfMultimode::Mode::lowPass12);
        tape_.prepare (sampleRate);
        noiseLp_.prepare (sampleRate);
        dcBlocker_.prepare (sampleRate);
        ampEnv_.setSampleRate (sampleRate);
        filtEnv_.setSampleRate (sampleRate);
        pitchEnv_.setSampleRate (sampleRate);

        wtPosSmoother_.prepare (sampleRate);
        cutoffSmoother_.prepare (sampleRate);
        driveSmoother_.prepare (sampleRate);
        crushSmoother_.prepare (sampleRate);
        wtPosSmoother_.setTimeMs (12.0f);
        cutoffSmoother_.setTimeMs (8.0f);
        driveSmoother_.setTimeMs (6.0f);
        crushSmoother_.setTimeMs (6.0f);
    }

    void reset() noexcept
    {
        active_ = false;
        note_ = 0;
        unisonPhases_.fill (0.0);
        subPhase_ = 0.0;
        scanPhase_ = 0.0;
        filter_.reset();
        destruct_.reset();
        destructOs_.reset();
        tape_.reset();
        noiseLp_.reset();
        dcBlocker_.reset();
        ampEnv_.reset();
        filtEnv_.reset();
        pitchEnv_.reset();
        fadeInSamples_ = 0;
    }

    bool isActive() const noexcept { return active_; }
    int getNote() const noexcept { return note_; }
    float ampLevel() const noexcept { return ampEnv_.currentLevel(); }

    void start (int midiNote, float velocity, const SynthParams& p, float startFreqHz) noexcept
    {
        note_ = midiNote;
        velocity_ = std::clamp (velocity, 0.05f, 1.0f);
        targetFreq_ = startFreqHz;
        if (currentFreq_ <= 0.0f || p.glideMs <= 0.0f)
            currentFreq_ = targetFreq_;

        rng_ = static_cast<std::uint32_t> (midiNote * 9973 + 1);
        for (int u = 0; u < kUnisonVoices; ++u)
        {
            rng_ = rng_ * 1664525u + 1013904223u;
            const float rand01 = static_cast<float> (rng_ & 0xffff) / 65535.0f;
            unisonPhases_[static_cast<std::size_t> (u)] = static_cast<double> (rand01);
        }
        subPhase_ = unisonPhases_[0] * 0.37;
        scanPhase_ = 0.0;

        applyParams (p);
        destruct_.reset();
        destructOs_.reset();
        fadeInTotal_ = static_cast<int> (sampleRate_ * 0.004);
        fadeInSamples_ = fadeInTotal_;
        ampEnv_.noteOn();
        filtEnv_.noteOn();
        pitchEnv_.noteOn();
        active_ = true;
    }

    void stop (bool allowTail) noexcept
    {
        if (! allowTail)
        {
            reset();
            return;
        }
        ampEnv_.noteOff();
        filtEnv_.noteOff();
        pitchEnv_.noteOff();
    }

    void applyParams (const SynthParams& p) noexcept
    {
        params_ = p;
        ampEnv_.setParams (p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease, 1.15f);
        filtEnv_.setParams (p.filtAttack, p.filtDecay, p.filtSustain, p.filtRelease, 1.0f);
        pitchEnv_.setParams (p.pitchEnvAttack, p.pitchEnvDecay, 0.0f, p.pitchEnvRelease, 1.0f);
        tape_.setMacro (p.tapeWobble);
        filter_.setMode (static_cast<dsp::ZdfMultimode::Mode> (std::clamp (p.filterMode, 0, 5)));
        wtPosSmoother_.setTarget (p.wtPosition);
        cutoffSmoother_.setTarget (p.filterCutoff);
        driveSmoother_.setTarget (p.drive);
        crushSmoother_.setTarget (p.crush);
    }

    void render (float& outL, float& outR) noexcept
    {
        if (! active_ && ! ampEnv_.isActive())
            return;

        glideTowardTarget();

        const float pitchEnv = pitchEnv_.process();
        const float pitchDrop = pitchEnv * params_.pitchEnvDepthCents;
        const float tapeMul = tape_.pitchMultiplier();
        const float detuneMul = std::pow (2.0f, (params_.detuneCents + tape_.subDetuneCents() + pitchDrop) / 1200.0f);
        const float baseFreq = currentFreq_ * tapeMul * detuneMul;

        scanPhase_ += (0.2 + static_cast<double> (params_.wtScan) * 6.0) / sampleRate_;
        const float wtPosRaw = std::clamp (params_.wtPosition + static_cast<float> (std::sin (scanPhase_ * 6.283185307) * params_.wtScan * 0.35f),
                                        0.0f, 1.0f);
        wtPosSmoother_.setTarget (wtPosRaw);
        const float wtPos = wtPosSmoother_.process();

        const float spreadCents = std::abs (params_.detuneCents) * 0.65f + params_.stereoWidth * 24.0f;
        const int unisonCount = spreadCents > 0.5f ? kUnisonVoices : 1;
        const float unisonGain = 1.0f / std::sqrt (static_cast<float> (unisonCount));

        float oscL = 0.0f;
        float oscR = 0.0f;
        for (int u = 0; u < unisonCount; ++u)
        {
            const float voiceT = unisonCount > 1 ? (static_cast<float> (u) / static_cast<float> (unisonCount - 1) * 2.0f - 1.0f) : 0.0f;
            const float cents = voiceT * spreadCents;
            const float freq = baseFreq * std::pow (2.0f, cents / 1200.0f);
            const double phaseInc = static_cast<double> (freq) / sampleRate_;

            auto& phase = unisonPhases_[static_cast<std::size_t> (u)];
            phase += phaseInc;

            float sample = bank_->sampleMorph (phase, wtPos, phaseInc);
            sample = dsp::flushDenormal (sample);

            const float spread = voiceT * params_.stereoWidth;
            const float panL = std::sqrt (std::clamp (0.5f - spread * 0.5f, 0.0f, 0.5f));
            const float panR = std::sqrt (std::clamp (0.5f + spread * 0.5f, 0.0f, 0.5f));
            oscL += sample * panL;
            oscR += sample * panR;
        }
        oscL *= unisonGain;
        oscR *= unisonGain;

        subPhase_ += (static_cast<double> (baseFreq) * 0.5) / sampleRate_;
        const float sub = bank_->sampleSine (subPhase_) * params_.subLevel * 0.55f;
        oscL += sub * 0.7071f;
        oscR += sub * 0.7071f;

        const float velAmp = 0.18f + 0.82f * std::pow (velocity_, 0.75f);
        const float velCut = 0.35f + 0.65f * velocity_;

        const float filtEnv = filtEnv_.process();
        cutoffSmoother_.setTarget (std::clamp ((params_.filterCutoff + filtEnv * params_.filtEnvAmt) * velCut, 0.0f, 1.0f));
        const float cutoff = cutoffSmoother_.process();
        const float res = params_.filterRes;

        float sampleL = filter_.process (oscL, cutoff, res);
        float sampleR = filter_.process (oscR, cutoff, res);

        const float drive = driveSmoother_.process();
        const float crush = crushSmoother_.process();
        const bool destructive = params_.filterDestructive;
        const bool heavyDistort = destructive && (drive > 0.25f || crush > 0.15f);

        auto distort = [&] (float x) noexcept
        {
            return destruct_.process (x, drive, crush, destructive);
        };

        if (heavyDistort && crush > 0.45f)
        {
            dsp::Oversampler2x os1;
            dsp::Oversampler2x os2;
            sampleL = os2.process (sampleL, [&] (float x) { return os1.process (x, distort); });
            sampleR = os2.process (sampleR, [&] (float x) { return os1.process (x, distort); });
        }
        else if (heavyDistort)
        {
            sampleL = destructOs_.process (sampleL, distort);
            sampleR = destructOs_.process (sampleR, distort);
        }
        else
        {
            sampleL = distort (sampleL);
            sampleR = distort (sampleR);
        }

        const float noiseRaw = tape_.nextNoiseSample (rng_) * velocity_;
        const float noise = noiseLp_.process (noiseRaw, 0.22f, 0.05f);
        sampleL = tape_.processTape (sampleL + noise * 0.5f);
        sampleR = tape_.processTape (sampleR + noise * 0.5f);

        const float amp = ampEnv_.process() * velAmp;
        float fadeIn = 1.0f;
        if (fadeInSamples_ > 0 && fadeInTotal_ > 0)
        {
            fadeIn = 1.0f - static_cast<float> (fadeInSamples_) / static_cast<float> (fadeInTotal_);
            --fadeInSamples_;
        }
        const float gain = amp * params_.voiceGain * fadeIn;
        sampleL *= gain;
        sampleR *= gain;

        sampleL = dcBlocker_.process (sampleL);
        sampleR = dcBlocker_.process (sampleR);

        const float width = std::clamp (params_.stereoWidth, 0.0f, 1.0f);
        const float mid = 0.5f * (sampleL + sampleR);
        const float side = 0.5f * (sampleL - sampleR) * width;
        sampleL = mid + side;
        sampleR = mid - side;

        if (! ampEnv_.isActive())
            active_ = false;

        outL += sampleL;
        outR += sampleR;
    }

private:
    void glideTowardTarget() noexcept
    {
        if (params_.glideMs <= 0.0f)
        {
            currentFreq_ = targetFreq_;
            return;
        }

        const float coef = 1.0f - std::exp (-1.0f / (0.001f + params_.glideMs * static_cast<float> (sampleRate_) * 0.001f));
        currentFreq_ += (targetFreq_ - currentFreq_) * coef;
    }

    double sampleRate_ = 44100.0;
    const dsp::WavetableBank* bank_ = nullptr;
    SynthParams params_{};
    bool active_ = false;
    int note_ = 0;
    float velocity_ = 0.0f;
    float targetFreq_ = 440.0f;
    float currentFreq_ = 440.0f;
    std::array<double, kUnisonVoices> unisonPhases_{};
    double subPhase_ = 0.0;
    double scanPhase_ = 0.0;
    std::uint32_t rng_ = 1;
    int fadeInSamples_ = 0;
    int fadeInTotal_ = 0;

    dsp::ZdfMultimode filter_{};
    dsp::ZdfMultimode noiseLp_{};
    dsp::DestructiveStage destruct_{};
    dsp::Oversampler2x destructOs_{};
    dsp::TapeWobble tape_{};
    dsp::DcBlocker dcBlocker_{};
    dsp::ParamSmoother wtPosSmoother_{};
    dsp::ParamSmoother cutoffSmoother_{};
    dsp::ParamSmoother driveSmoother_{};
    dsp::ParamSmoother crushSmoother_{};
    dsp::AdsrEnvelope ampEnv_{};
    dsp::AdsrEnvelope filtEnv_{};
    dsp::AdsrEnvelope pitchEnv_{};
};

} // namespace wave9090::engine