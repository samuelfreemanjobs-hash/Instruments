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

namespace wave9090::engine
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
        noiseLp_.prepare (sampleRate);
        ampEnv_.setSampleRate (sampleRate);
        filtEnv_.setSampleRate (sampleRate);
        pitchEnv_.setSampleRate (sampleRate);
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
        noiseLp_.reset();
        ampEnv_.reset();
        filtEnv_.reset();
        pitchEnv_.reset();
        panL_ = 0.7071f;
        panR_ = 0.7071f;
    }

    bool isActive() const noexcept { return active_; }
    int getNote() const noexcept { return note_; }

    void start (int midiNote, float velocity, const SynthParams& p, float startFreqHz) noexcept
    {
        note_ = midiNote;
        velocity_ = std::clamp (velocity, 0.05f, 1.0f);
        targetFreq_ = startFreqHz;
        if (currentFreq_ <= 0.0f || p.glideMs <= 0.0f)
            currentFreq_ = targetFreq_;

        const float rand01 = static_cast<float> ((midiNote * 7919 + 17) % 1000) / 1000.0f;
        phase_ = static_cast<double> (rand01);
        subPhase_ = phase_ * 0.37;
        scanPhase_ = 0.0;
        rng_ = static_cast<std::uint32_t> (midiNote * 9973 + 1);

        const float spread = (rand01 - 0.5f) * p.stereoWidth;
        panL_ = std::sqrt (0.5f - spread * 0.5f);
        panR_ = std::sqrt (0.5f + spread * 0.5f);

        applyParams (p);
        destruct_.reset();
        fadeInTotal_ = static_cast<int> (sampleRate_ * 0.006);
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
        ampEnv_.setParams (p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease);
        filtEnv_.setParams (p.filtAttack, p.filtDecay, p.filtSustain, p.filtRelease);
        pitchEnv_.setParams (p.pitchEnvAttack, p.pitchEnvDecay, 0.0f, p.pitchEnvRelease);
        tape_.setMacro (p.tapeWobble);
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
        const float freq = currentFreq_ * tapeMul * detuneMul;

        const double phaseInc = freq / sampleRate_;
        phase_ += phaseInc;
        subPhase_ += phaseInc * 0.5;

        scanPhase_ += (0.2 + static_cast<double> (params_.wtScan) * 6.0) / sampleRate_;
        const float wtPos = std::clamp (params_.wtPosition + static_cast<float> (std::sin (scanPhase_ * 6.283185307) * params_.wtScan * 0.35f),
                                        0.0f, 1.0f);

        float sample = bank_->sampleMorph (phase_, wtPos, phaseInc);
        const float sub = bank_->sampleSine (subPhase_) * params_.subLevel * 0.55f;
        sample = sample + sub;

        const float velAmp = 0.18f + 0.82f * std::pow (velocity_, 0.75f);
        const float velCut = 0.35f + 0.65f * velocity_;

        const float filtEnv = filtEnv_.process();
        const float cutoff = std::clamp ((params_.filterCutoff + filtEnv * params_.filtEnvAmt) * velCut, 0.0f, 1.0f);
        sample = filter_.process (sample, cutoff, params_.filterRes);
        sample = destruct_.process (sample, params_.drive, params_.crush, params_.filterDestructive);

        const float noiseRaw = tape_.nextNoiseSample (rng_) * velocity_;
        const float noise = noiseLp_.process (noiseRaw, 0.22f, 0.05f);
        sample += noise;

        const float amp = ampEnv_.process() * velAmp;
        float fadeIn = 1.0f;
        if (fadeInSamples_ > 0 && fadeInTotal_ > 0)
        {
            fadeIn = 1.0f - static_cast<float> (fadeInSamples_) / static_cast<float> (fadeInTotal_);
            --fadeInSamples_;
        }
        sample *= amp * params_.voiceGain * fadeIn;

        if (! ampEnv_.isActive())
            active_ = false;

        outL += sample * panL_;
        outR += sample * panR_;
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
    double phase_ = 0.0;
    double subPhase_ = 0.0;
    double scanPhase_ = 0.0;
    std::uint32_t rng_ = 1;
    float panL_ = 0.7071f;
    float panR_ = 0.7071f;
    int fadeInSamples_ = 0;
    int fadeInTotal_ = 0;

    dsp::ZdfLowpass filter_{};
    dsp::ZdfLowpass noiseLp_{};
    dsp::DestructiveStage destruct_{};
    dsp::TapeWobble tape_{};
    dsp::AdsrEnvelope ampEnv_{};
    dsp::AdsrEnvelope filtEnv_{};
    dsp::AdsrEnvelope pitchEnv_{};
};

} // namespace wave9090::engine