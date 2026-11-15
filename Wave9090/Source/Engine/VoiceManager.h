#pragma once

#include "Engine/Voice.h"
#include "DSP/WavetableBank.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>

namespace wave9090::engine
{

class VoiceManager final
{
public:
    static constexpr int kMaxVoices = 16;

    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
        if (bank_ == nullptr)
            bank_ = std::make_unique<dsp::WavetableBank>();
        bank_->rebuild();
        for (auto& v : voices_)
            v.prepare (sampleRate, *bank_);
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
        std::fill (outL, outL + numSamples, 0.0f);
        std::fill (outR, outR + numSamples, 0.0f);
        for (auto& v : voices_)
        {
            if (v.isRendering())
                v.renderBlock (outL, outR, numSamples);
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
        Voice* candidate = nullptr;
        float quietest = 1.0e9f;
        for (auto& v : voices_)
        {
            if (! v.isActive())
                return &v;
            const float level = v.ampLevel();
            if (level < quietest)
            {
                quietest = level;
                candidate = &v;
            }
        }
        if (candidate != nullptr)
        {
            candidate->stop (false);
            return candidate;
        }
        return &voices_[0];
    }

    double sampleRate_ = 44100.0;
    std::unique_ptr<dsp::WavetableBank> bank_;
    SynthParams params_{};
    std::array<Voice, kMaxVoices> voices_{};
};

} // namespace wave9090::engine
