#pragma once

#include "Assets/RawRomBank.h"
#include "disklordz/RawRomFormat.h"
#include "DSP/AdsrEnvelope.h"
#include "Engine/RomplerParams.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace disklordz::rompler::engine
{

class RomplerVoice final
{
public:
    void prepare (double sampleRate, const assets::RawRomLibrary* library) noexcept
    {
        sampleRate_ = sampleRate;
        library_ = library;
        ampEnv_.setSampleRate (sampleRate);
    }

    void reset() noexcept
    {
        active_ = false;
        note_ = 0;
        for (auto& layer : layers_)
            layer = {};
        ampEnv_.reset();
    }

    bool isActive() const noexcept { return active_; }
    int getNote() const noexcept { return note_; }

    void start (int midiNote, float velocity, const RomplerParams& p) noexcept
    {
        note_ = midiNote;
        velocity_ = std::clamp (velocity, 0.0f, 1.0f);
        params_ = p;

        for (std::uint8_t t = 0; t < rawrom::kRomplerToneLayers; ++t)
        {
            layers_[t].wave = library_ != nullptr
                                  ? library_->selectForTone (p.toneRomBank[static_cast<std::size_t> (t)],
                                                             t,
                                                             p.toneProgram[static_cast<std::size_t> (t)],
                                                             static_cast<std::uint8_t> (midiNote))
                                  : assets::WaveView{};
            layers_[t].phase = 0.0;
        }

        ampEnv_.setParams (p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease);
        ampEnv_.noteOn();
        active_ = true;
    }

    void stop() noexcept { ampEnv_.noteOff(); }

    void setParams (const RomplerParams& p) noexcept
    {
        params_ = p;
        ampEnv_.setParams (p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease);
    }

    void render (float& outL, float& outR) noexcept
    {
        if (! active_ && ! ampEnv_.isActive())
            return;

        float mix = 0.0f;
        const float brightness = 0.5f + params_.macroBrightness * 0.5f;
        const float drive = params_.macroDrive * 2.5f;
        const float crushMix = params_.macroCrush;

        for (std::uint8_t t = 0; t < rawrom::kRomplerToneLayers; ++t)
        {
            auto& layer = layers_[t];
            if (layer.wave.samples == nullptr || layer.wave.frameCount == 0)
                continue;

            const float level = params_.toneLevel[static_cast<std::size_t> (t)];
            if (level <= 1.0e-5f)
                continue;

            const float bend = std::pow (2.0f, params_.pitchBendSemis / 12.0f);
            const float noteDelta = static_cast<float> (note_) - static_cast<float> (layer.wave.rootNote);
            const float rate = std::pow (2.0f, noteDelta / 12.0f) * bend;

            mix += sampleLayer (layer, rate) * level;
        }

        const float cutoff = 200.0f + params_.filterCutoff * 12000.0f;
        const float fc = std::min (cutoff / static_cast<float> (sampleRate_), 0.45f);
        lpState_ += fc * (mix - lpState_);
        mix = mix * (1.0f - brightness * 0.35f) + lpState_ * (brightness * 0.35f);

        mix = std::tanh (mix * (1.0f + drive));
        if (crushMix > 0.001f)
        {
            const float steps = 2.0f + (1.0f - crushMix) * 62.0f;
            mix = std::round (mix * steps) / steps;
        }

        const float amp = ampEnv_.process() * velocity_;
        mix *= amp;

        if (! ampEnv_.isActive())
            active_ = false;

        const float width = params_.macroWidth * 0.4f;
        const float pan = (note_ % 2 == 0 ? -1.0f : 1.0f) * width;
        outL += mix * (1.0f - pan);
        outR += mix * (1.0f + pan);
    }

private:
    struct LayerState final
    {
        assets::WaveView wave{};
        double phase = 0.0;
    };

    float sampleLayer (LayerState& layer, float rate) noexcept
    {
        const auto& view = layer.wave;
        const auto i0 = static_cast<std::uint32_t> (layer.phase) % view.frameCount;
        const auto i1 = (i0 + 1) % view.frameCount;
        const float frac = static_cast<float> (layer.phase - std::floor (layer.phase));
        const float s = view.samples[i0] + (view.samples[i1] - view.samples[i0]) * frac;

        layer.phase += static_cast<double> (rate);
        if (view.looped && view.loopEnd > view.loopStart)
        {
            if (layer.phase >= static_cast<double> (view.loopEnd))
                layer.phase = static_cast<double> (view.loopStart)
                              + std::fmod (layer.phase - static_cast<double> (view.loopStart),
                                           static_cast<double> (view.loopEnd - view.loopStart));
        }
        else if (layer.phase >= static_cast<double> (view.frameCount))
        {
            layer.phase = static_cast<double> (view.frameCount - 1);
        }

        return s;
    }

    double sampleRate_ = 44100.0;
    const assets::RawRomLibrary* library_ = nullptr;
    RomplerParams params_{};
    std::array<LayerState, rawrom::kRomplerToneLayers> layers_{};
    dsp::AdsrEnvelope ampEnv_{};
    float lpState_ = 0.0f;
    bool active_ = false;
    int note_ = 0;
    float velocity_ = 0.0f;
};

} // namespace disklordz::rompler::engine
