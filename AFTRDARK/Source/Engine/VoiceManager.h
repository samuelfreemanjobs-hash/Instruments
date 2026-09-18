#pragma once

#include "Engine/Voice.h"
#include "DSP/WavetableBank.h"

#include <array>
#include <cmath>

namespace aftrdark::engine
{

class VoiceManager final
{
public:
    static constexpr int kMaxVoices = 16;

    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
        bank_.rebuild();
        for (auto& v : voices_)
            v.prepare (sampleRate, bank_);
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v.reset();
    }

    void setParams (const SynthParams& p) noexcept
    {
        params_ = p;
        for (auto& v : voices_)
            if (v.isActive())
                v.applyParams (params_);
    }

    void noteOn (int note, float velocity) noexcept
    {
        Voice* v = findFreeVoice();
        if (v == nullptr)
            v = stealVoice();

        const float freq = static_cast<float> (440.0 * std::pow (2.0, (note - 69) / 12.0));
        v->start (note, velocity, params_, freq);
    }

    void noteOff (int note) noexcept
    {
        for (auto& v : voices_)
            if (v.isActive() && v.getNote() == note)
                v.stop (true);
    }

    void render (float* outL, float* outR, int numSamples) noexcept
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float l = 0.0f;
            float r = 0.0f;
            for (auto& v : voices_)
                v.render (l, r);

            outL[i] += l;
            outR[i] += r;
        }
    }

private:
    Voice* findFreeVoice() noexcept
    {
        for (auto& v : voices_)
            if (! v.isActive())
                return &v;
        return nullptr;
    }

    Voice* stealVoice() noexcept
    {
        return &voices_[0];
    }

    double sampleRate_ = 44100.0;
    dsp::WavetableBank bank_{};
    SynthParams params_{};
    std::array<Voice, kMaxVoices> voices_{};
};

} // namespace aftrdark::engine
