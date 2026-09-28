#pragma once

#include "Engine/SynthParams.h"
#include "DSP/AdsrEnvelope.h"
#include "DSP/Effects/DestructiveStage.h"
#include "DSP/Effects/TapeWobble.h"
#include "DSP/Filter/ZdfLowpass.h"
#include "DSP/WavetableBank.h"

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace wave909::engine
{

class Voice final
{
public:
    void prepare (double sampleRate, const dsp::WavetableBank& bank) noexcept
    {
        sampleRate_ = sampleRate;
        bank_ = &bank;
        filter_.prepare (sampleRate);
        destruct_.prepare (sampleRate);
        tape_.prepare (sampleRate);
        ampEnv_.setSampleRate (sampleRate);
        filtEnv_.setSampleRate (sampleRate);
    }

    void reset() noexcept
    {
        active_ = false;
        note_ = 0;
        phase_ = 0.0;
        subPhase_ = 0.0;
        scanPhase_ = 0.0;
        filter_.reset();
        destruct_.reset();
        tape_.reset();
        ampEnv_.reset();
        filtEnv_.reset();
    }

    bool isActive() const noexcept { return active_; }
    int getNote() const noexcept { return note_; }

    void start (int midiNote, float velocity, const SynthParams& p, float startFreqHz) noexcept
    {
        note_ = midiNote;
        velocity_ = std::clamp (velocity, 0.0f, 1.0f);
        targetFreq_ = startFreqHz;
        if (currentFreq_ <= 0.0f || p.glideMs <= 0.0f)
            currentFreq_ = targetFreq_;

        phase_ = 0.0;
        subPhase_ = 0.0;
        scanPhase_ = 0.0;
        rng_ = static_cast<std::uint32_t> (midiNote * 9973 + 1);

        applyParams (p);
        ampEnv_.noteOn();
        filtEnv_.noteOn();
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
    }

    void applyParams (const SynthParams& p) noexcept
    {
        params_ = p;
        ampEnv_.setParams (p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease);
        filtEnv_.setParams (p.filtAttack, p.filtDecay, p.filtSustain, p.filtRelease);
        tape_.setMacro (p.tapeWobble);
    }

    void render (float& outL, float& outR) noexcept
    {
        if (! active_ && ! ampEnv_.isActive())
            return;

        glideTowardTarget();

        const float pitchMul = tape_.pitchMultiplier();
        const float detuneMul = std::pow (2.0f, (params_.detuneCents + tape_.subDetuneCents()) / 1200.0f);
        const float freq = currentFreq_ * pitchMul * detuneMul;

        const double phaseInc = freq / sampleRate_;
        phase_ += phaseInc;
        subPhase_ += phaseInc * 0.5;

        scanPhase_ += (0.2 + static_cast<double> (params_.wtScan) * 6.0) / sampleRate_;
        const float wtPos = std::clamp (params_.wtPosition + static_cast<float> (std::sin (scanPhase_ * 6.283185307) * params_.wtScan * 0.35f),
                                        0.0f, 1.0f);

        float sample = bank_->sampleMorph (phase_, wtPos);
        const float sub = std::sin (subPhase_ * 6.283185307) * params_.subLevel * 0.6f;
        sample = sample + sub;

        const float filtEnv = filtEnv_.process();
        const float cutoff = std::clamp (params_.filterCutoff + filtEnv * params_.filtEnvAmt, 0.0f, 1.0f);
        sample = filter_.process (sample, cutoff, params_.filterRes);
        sample = destruct_.process (sample, params_.drive, params_.crush, params_.filterDestructive);

        const float noise = tape_.nextNoiseSample (rng_) * velocity_;
        sample += noise;

        const float amp = ampEnv_.process() * velocity_;
        sample *= amp;

        if (! ampEnv_.isActive())
            active_ = false;

        const float pan = (note_ % 2 == 0 ? -1.0f : 1.0f) * params_.stereoWidth;
        outL += sample * (1.0f - pan * 0.5f);
        outR += sample * (1.0f + pan * 0.5f);
    }

private:
    void glideTowardTarget() noexcept
    {
        if (params_.glideMs <= 0.0f)
        {
            currentFreq_ = targetFreq_;
            return;
        }

        const float coef = 1.0f / (0.001f + params_.glideMs * static_cast<float> (sampleRate_) * 0.001f);
        currentFreq_ += (targetFreq_ - currentFreq_) * std::min (1.0f, coef);
    }

    double sampleRate_ = 44100.0;
    const dsp::WavetableBank* bank_ = nullptr;
    SynthParams params_{};
    bool active_ = false;
    int note_ = 0;
    float velocity_ = 0.0f;
    float targetFreq_ = 440.0f;
    float currentFreq_ = 440.0f;
    double phase_ = 0.0;
    double subPhase_ = 0.0;
    double scanPhase_ = 0.0;
    std::uint32_t rng_ = 1;

    dsp::ZdfLowpass filter_{};
    dsp::DestructiveStage destruct_{};
    dsp::TapeWobble tape_{};
    dsp::AdsrEnvelope ampEnv_{};
    dsp::AdsrEnvelope filtEnv_{};
};

} // namespace wave909::engine
