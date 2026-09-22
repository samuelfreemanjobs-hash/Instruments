#pragma once

#include "DSP/AdsrEnvelope.h"
#include "Presets/FactoryPresets.h"

#include <juce_dsp/juce_dsp.h>

#include <algorithm>
#include <cmath>

namespace rev2trap::dsp
{

class PublicLadder final : public juce::dsp::LadderFilter<float>
{
public:
    float tick (float x) noexcept { return processSample (x, 0); }
};

class Rev2Voice final
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
        ampEnv_.setSampleRate (sampleRate);
        filtEnv_.setSampleRate (sampleRate);
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels = 1;
        ladder_.prepare (spec);
        ladder_.setMode (juce::dsp::LadderFilterMode::LPF24);
        ladder_.reset();
    }

    void reset() noexcept
    {
        active_ = false;
        phase1_ = phase2_ = subPhase_ = 0.0;
        ampEnv_.reset();
        filtEnv_.reset();
        ladder_.reset();
    }

    bool isActive() const noexcept { return active_ || ampEnv_.isActive(); }

    void start (int midiNote, float velocity, presets::SynthParams p) noexcept
    {
        note_ = midiNote;
        velocity_ = std::clamp (velocity, 0.0f, 1.0f);
        params_ = p;
        freqHz_ = 440.0f * std::pow (2.0f, (static_cast<float> (midiNote) - 69.0f) / 12.0f);
        phase1_ = phase2_ = subPhase_ = 0.0;
        ampEnv_.setParams (p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease);
        filtEnv_.setParams (p.filtAttack, p.filtDecay, p.filtSustain, p.filtRelease);
        updateFilter (0.0f);
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
        active_ = false;
    }

    float processSample() noexcept
    {
        if (! isActive())
            return 0.0f;

        const float fe = filtEnv_.process();
        updateFilter (fe);

        const double inc1 = freqHz_ / sampleRate_;
        const double inc2 = inc1 * std::pow (2.0, params_.osc2DetuneCents / 1200.0);
        phase1_ += inc1;
        phase2_ += inc2;
        subPhase_ += inc1 * 0.5;

        auto saw = [] (double ph)
        {
            const double x = ph / (2.0 * juce::MathConstants<double>::pi);
            return static_cast<float> (2.0 * (x - std::floor (x + 0.5)));
        };
        auto square = [] (double ph) { return std::sin (ph) >= 0.0 ? 1.0f : -1.0f; };

        const float osc1 = saw (phase1_);
        const float osc2 = square (phase2_);
        const float sub = std::sin (static_cast<float> (subPhase_));
        float raw = (osc1 * (1.0f - params_.oscMix) + osc2 * params_.oscMix) + sub * params_.subLevel;
        raw = std::tanh (raw * params_.drive);

        float filtered = ladder_.tick (raw);
        const float amp = ampEnv_.process() * velocity_;
        filtered *= amp;

        if (! ampEnv_.isActive() && amp <= 0.0001f)
            reset();

        return filtered;
    }

private:
    void updateFilter (float envAmt) noexcept
    {
        const float base = 120.0f + params_.cutoff * 9800.0f;
        const float mod = base * (1.0f + params_.filterEnv * envAmt * 2.5f);
        ladder_.setCutoffFrequencyHz (std::clamp (mod, 80.0f, 12000.0f));
        ladder_.setResonance (std::clamp (0.1f + params_.resonance * 0.85f, 0.1f, 0.95f));
    }

    PublicLadder ladder_;
    AdsrEnvelope ampEnv_, filtEnv_;
    presets::SynthParams params_;
    double sampleRate_ = 44100.0;
    double phase1_ = 0.0, phase2_ = 0.0, subPhase_ = 0.0;
    float freqHz_ = 440.0f;
    float velocity_ = 1.0f;
    int note_ = 60;
    bool active_ = false;
};

} // namespace rev2trap::dsp
