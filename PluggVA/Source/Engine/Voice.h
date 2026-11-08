#pragma once

#include "Engine/SynthParams.h"
#include "DSP/AdsrEnvelope.h"
#include "DSP/PolyBlepOscillator.h"
#include "DSP/ZdfLowpass.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <algorithm>

namespace pluggva::engine
{

class Voice final
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
        filter_.prepare (sampleRate);
        for (auto& o : osc1Stack_)
            o.prepare (sampleRate);
        osc2_.prepare (sampleRate);
        ampEnv_.setSampleRate (sampleRate);
        filtEnv_.setSampleRate (sampleRate);
    }

    void reset() noexcept
    {
        active_ = false;
        note_ = 0;
        filter_.reset();
        for (auto& o : osc1Stack_)
            o.reset();
        osc2_.reset();
        ampEnv_.reset();
        filtEnv_.reset();
        driftPhase_ = 0.0;
        subPhase_ = 0.0;
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

        driftSeed_ = static_cast<float> ((midiNote * 7919) % 1000) * 0.001f;
        subPhase_ = 0.0;

        for (auto& o : osc1Stack_)
        {
            o.reset();
            o.setFrequency (currentFreq_);
        }
        osc2_.reset();
        osc2_.setFrequency (osc2Frequency (currentFreq_, p));

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
        configureUnison();
    }

    void render (float& outL, float& outR) noexcept
    {
        if (! active_ && ! ampEnv_.isActive())
            return;

        glideTowardTarget();

        const float driftCents = params_.drift > 0.0f
                                   ? static_cast<float> (std::sin (driftPhase_ * 6.283185307)) * params_.drift * 12.0f
                                   : 0.0f;
        driftPhase_ += (0.15 + driftSeed_) / sampleRate_;

        const float pitchMul = std::pow (2.0f, driftCents / 1200.0f);
        const float freq = currentFreq_ * pitchMul;
        const float freq2 = osc2Frequency (freq, params_);

        configureUnison();
        for (std::size_t i = 0; i < unisonCount_; ++i)
        {
            const float detune = unisonDetuneCents (i);
            const float f = freq * std::pow (2.0f, detune / 1200.0f);
            osc1Stack_[i].setFrequency (f);
        }
        osc2_.setFrequency (freq2 * std::pow (2.0f, params_.osc2DetuneCents / 1200.0f));

        float oscMix = 0.0f;
        for (std::size_t i = 0; i < unisonCount_; ++i)
            oscMix += osc1Stack_[i].process (params_.osc1Wave, params_.pulseWidth) / static_cast<float> (unisonCount_);

        oscMix *= params_.osc1Level;

        const float osc2Out = osc2_.process (params_.osc2Wave, params_.pulseWidth) * params_.osc2Level;
        float sample = oscMix + osc2Out;

        subPhase_ += static_cast<double> (freq) * 0.5 / sampleRate_;
        if (subPhase_ >= 1.0)
            subPhase_ -= 1.0;
        const float sub = static_cast<float> (std::sin (subPhase_ * 6.283185307)) * params_.subLevel * 0.55f;
        sample += sub;

        const float filtEnv = filtEnv_.process();
        const float cutoff = std::clamp (params_.filterCutoff + filtEnv * params_.filtEnvAmt, 0.0f, 1.0f);
        sample = filter_.process (sample, cutoff, params_.filterRes);

        if (params_.drive > 0.0f)
        {
            const float driveGain = 1.0f + params_.drive * 6.0f;
            sample = std::tanh (sample * driveGain) / std::tanh (driveGain);
        }

        if (params_.crush > 0.01f)
        {
            const float steps = 2.0f + (1.0f - params_.crush) * 62.0f;
            sample = std::round (sample * steps) / steps;
        }

        const float amp = ampEnv_.process() * velocity_;
        sample *= amp;

        if (! ampEnv_.isActive())
            active_ = false;

        const float pan = (note_ % 2 == 0 ? -1.0f : 1.0f) * params_.stereoWidth;
        outL += sample * (1.0f - pan * 0.5f);
        outR += sample * (1.0f + pan * 0.5f);
    }

private:
    static float osc2Frequency (float baseFreq, const SynthParams& p) noexcept
    {
        const float octaveMul = std::pow (2.0f, static_cast<float> (p.osc2Octave));
        return baseFreq * octaveMul;
    }

    void configureUnison() noexcept
    {
        unisonCount_ = static_cast<std::size_t> (1 + std::clamp (static_cast<int> (params_.unisonAmount * 4.99f), 0, 4));
    }

    float unisonDetuneCents (std::size_t index) const noexcept
    {
        if (unisonCount_ <= 1)
            return 0.0f;

        const float spread = params_.unisonSpreadCents;
        const float norm = (static_cast<float> (index) / static_cast<float> (unisonCount_ - 1)) * 2.0f - 1.0f;
        return norm * spread;
    }

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
    SynthParams params_{};
    bool active_ = false;
    int note_ = 0;
    float velocity_ = 0.0f;
    float targetFreq_ = 440.0f;
    float currentFreq_ = 440.0f;
    float driftSeed_ = 0.0f;
    double driftPhase_ = 0.0;
    double subPhase_ = 0.0;

    std::size_t unisonCount_ = 1;
    std::array<dsp::PolyBlepOscillator, 5> osc1Stack_{};
    dsp::PolyBlepOscillator osc2_{};
    dsp::ZdfLowpass filter_{};
    dsp::AdsrEnvelope ampEnv_{};
    dsp::AdsrEnvelope filtEnv_{};
};

} // namespace pluggva::engine
