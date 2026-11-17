#pragma once
// DiskLordz Shawty Redd / 2005 Atlanta Trap — portable C++20 (no JUCE).
// 4× OS tube · kick→808 duck · velocity accent snare · open/closed hat choke.
#include "CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>

namespace disklordz::shawtyredd
{

struct ShawtyReddParams
{
    float subFreq = 49.0f;
    float kickAttackHz = 175.0f;
    float kickDecayMs = 42.0f;
    float subDecayMs = 580.0f;
    float kickDuckAmount = 0.45f;
    float tubeDrive = 3.8f;
    float lowMidGrowlGain = 1.75f;

    float snareBodyHz = 205.0f;
    float snareSnapCutoff = 3800.0f;
    float snareDecayMs = 115.0f;
    float snareClapSpreadMs = 11.0f;

    float hatDecayMs = 28.0f;
    float openHatDecayMs = 240.0f;
    float stereoWidth = 0.35f;
    float outputGain = 0.92f;
};

struct ReddVoiceState
{
    bool active = false;
    float timeSec = 0.0f;
    float velocity = 1.0f;
    double phase1 = 0.0;
    double phase2 = 0.0;
    float noteFreq = 49.0f;
    std::array<double, 6> hatPhases {};
};

enum class VoiceID : uint8_t
{
    VoiceRedd808 = 0,
    VoiceReddSnare = 1,
    VoiceClosedHat = 2,
    VoiceOpenHat = 3,
    Count = 4
};

inline float midiToHz(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline float lerp(float a, float b, float t) noexcept
{
    return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
}

inline ShawtyReddParams presetTrapOrDie() noexcept
{
    ShawtyReddParams p {};
    p.tubeDrive = 4.6f;
    p.kickDuckAmount = 0.50f;
    p.subFreq = 49.0f;
    p.kickAttackHz = 185.0f;
    p.snareClapSpreadMs = 12.0f;
    return p;
}

inline ShawtyReddParams presetKing() noexcept
{
    ShawtyReddParams p {};
    p.tubeDrive = 3.4f;
    p.lowMidGrowlGain = 1.9f;
    p.subDecayMs = 640.0f;
    p.snareBodyHz = 198.0f;
    return p;
}

class ShawtyReddEngine
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        growlPeak_.setParams(fs_, 165.0, 1.3, dsp::TptSvf::Mode::BandPass);
        subLp_.setParams(fs_, 55.0, 0.8, dsp::TptSvf::Mode::LowPass);
        dc_.prepare(fs_, 24.0);
        snareHp_.setParams(fs_, 180.0, 0.707, dsp::TptSvf::Mode::HighPass);
        snarePeak_.setParams(fs_, 3800.0, 1.5, dsp::TptSvf::Mode::BandPass);
        hatHp_.setParams(fs_, 7600.0, 0.707, dsp::TptSvf::Mode::HighPass);
        openHatHp_.setParams(fs_, 6400.0, 0.707, dsp::TptSvf::Mode::HighPass);
        tubeL_.prepare(fs_);
        tubeR_.prepare(fs_);
        rng_.reseed(0x5A471EDDULL, 5ULL);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = ReddVoiceState {};
        growlPeak_.reset();
        subLp_.reset();
        dc_.reset();
        snareHp_.reset();
        snarePeak_.reset();
        hatHp_.reset();
        openHatHp_.reset();
        tubeL_.reset();
        tubeR_.reset();
    }

    void setParams(const ShawtyReddParams& p) noexcept
    {
        params_ = p;
        snarePeak_.setParams(fs_, p.snareSnapCutoff, 1.5, dsp::TptSvf::Mode::BandPass);
    }

    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        if (velocity01 <= 0.0f)
            return;
        const float vel = std::clamp(velocity01, 0.05f, 1.0f);
        int id = -1;
        float freq = params_.subFreq;

        if (midiNote == 36 || (midiNote >= 48 && midiNote <= 65))
        {
            id = static_cast<int>(VoiceID::VoiceRedd808);
            if (midiNote >= 48)
                freq = midiToHz(midiNote - 24);
        }
        else if (midiNote == 38 || midiNote == 40)
            id = static_cast<int>(VoiceID::VoiceReddSnare);
        else if (midiNote == 42 || midiNote == 44)
            id = static_cast<int>(VoiceID::VoiceClosedHat);
        else if (midiNote == 46)
            id = static_cast<int>(VoiceID::VoiceOpenHat);

        if (id < 0)
            return;

        // Closed hat chokes open hat
        if (id == static_cast<int>(VoiceID::VoiceClosedHat))
            voices_[static_cast<size_t>(VoiceID::VoiceOpenHat)].active = false;

        auto& v = voices_[static_cast<size_t>(id)];
        v.active = true;
        v.timeSec = 0.0f;
        v.velocity = vel;
        v.phase1 = 0.0; // phase-locked punch
        v.phase2 = 0.0;
        v.noteFreq = freq;
        if (id == static_cast<int>(VoiceID::VoiceClosedHat)
            || id == static_cast<int>(VoiceID::VoiceOpenHat))
            v.hatPhases = {};
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        const auto& p = params_;
        const float dt = 1.0f / static_cast<float>(fs_);
        float lowBus = 0.0f;
        float percL = 0.0f;
        float percR = 0.0f;

        // Kick + ducking 808
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceRedd808)];
            if (v.active)
            {
                const float kickEnv = std::exp(-v.timeSec / (p.kickDecayMs * 0.001f));
                const float kickFreq =
                    58.0f + (p.kickAttackHz - 58.0f) * std::exp(-v.timeSec / 0.016f);
                v.phase2 = std::fmod(v.phase2 + static_cast<double>(kickFreq) / fs_, 1.0);
                const float kickLayer =
                    std::sin(static_cast<float>(v.phase2 * dsp::kTwoPi)) * kickEnv * 0.9f;

                const float subEnv = std::exp(-v.timeSec / (p.subDecayMs * 0.001f));
                const float duckGain = 1.0f - (p.kickDuckAmount * kickEnv);
                v.phase1 = std::fmod(v.phase1 + static_cast<double>(v.noteFreq) / fs_, 1.0);
                const float subLayer =
                    std::sin(static_cast<float>(v.phase1 * dsp::kTwoPi)) * subEnv * duckGain;

                float combined = subLayer + kickLayer;
                combined = combined * 0.7f + 0.3f * growlPeak_.process(combined);
                combined = combined * 0.75f + 0.35f * subLp_.process(combined);
                lowBus = dc_.process(combined * (p.tubeDrive * 0.45f)) * v.velocity;

                v.timeSec += dt;
                if (subEnv < 0.0005f && kickEnv < 0.0005f)
                    v.active = false;
            }
        }

        // Snare + velocity accent
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceReddSnare)];
            if (v.active)
            {
                const bool isAccent = (v.velocity < 0.65f);
                const float pitchScale = isAccent ? 1.14f : 1.0f;
                const float decayScale = isAccent ? 0.62f : 1.0f;
                const float tau = (p.snareDecayMs * decayScale) * 0.001f;
                const float bodyFreq = (p.snareBodyHz * pitchScale)
                    * (0.65f + 0.8f * std::exp(-v.timeSec / 0.015f));
                v.phase1 = std::fmod(v.phase1 + static_cast<double>(bodyFreq) / fs_, 1.0);
                const float body = std::sin(static_cast<float>(v.phase1 * dsp::kTwoPi))
                    * std::exp(-v.timeSec / (tau * 0.45f));

                const float clapDelay = p.snareClapSpreadMs * 0.001f;
                float noiseEnv = std::exp(-v.timeSec / tau);
                if (!isAccent && v.timeSec >= clapDelay)
                    noiseEnv += 0.55f * std::exp(-(v.timeSec - clapDelay) / (tau * 0.85f));

                const float noiseL = rng_.nextSigned() * noiseEnv;
                const float noiseRRaw = rng_.nextSigned() * noiseEnv;
                const float noiseR = lerp(noiseL, noiseRRaw, p.stereoWidth);

                float hitL = snareHp_.process(body * 0.5f + noiseL * 0.65f);
                hitL = hitL + 0.4f * snarePeak_.process(hitL);
                const float hitR = body * 0.5f + noiseR * 0.65f;

                percL += std::tanh(hitL * 1.6f) * v.velocity;
                percR += std::tanh(hitR * 1.6f) * v.velocity;
                v.timeSec += dt;
                if (v.timeSec > tau * 6.5f)
                    v.active = false;
            }
        }

        // Closed hat
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceClosedHat)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.0f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] =
                        std::fmod(v.hatPhases[static_cast<size_t>(k)] + ratios[k] / fs_, 1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / (p.hatDecayMs * 0.001f));
                const float hit = hatHp_.process(sum) * env * v.velocity * 0.85f;
                percL += hit * (1.0f + p.stereoWidth * 0.2f);
                percR += hit * (1.0f - p.stereoWidth * 0.2f);
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // Open hat (choked by closed)
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceOpenHat)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.0f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] = std::fmod(
                        v.hatPhases[static_cast<size_t>(k)] + (ratios[k] * 0.96) / fs_, 1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / (p.openHatDecayMs * 0.001f));
                const float hit = openHatHp_.process(sum) * env * v.velocity * 0.75f;
                percL += hit * (1.0f - p.stereoWidth * 0.25f);
                percR += hit * (1.0f + p.stereoWidth * 0.25f);
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        float busL = (lowBus + percL) * p.outputGain;
        float busR = (lowBus + percR) * p.outputGain;
        // 4× oversampled asymmetric tube on master bus
        outL = tubeL_.process(busL, 1.0f + p.tubeDrive * 0.2f,
                              dsp::OversampledWaveshaper4x::Curve::AsymmetricClip, 0.0f);
        outR = tubeR_.process(busR, 1.0f + p.tubeDrive * 0.2f,
                              dsp::OversampledWaveshaper4x::Curve::AsymmetricClip, 0.0f);
    }

    void renderOneShot(VoiceID id, const ShawtyReddParams& p, float* interleavedLR, int numFrames,
                       int midiNote = -1) noexcept
    {
        setParams(p);
        reset();
        int note = midiNote;
        if (note < 0)
        {
            switch (id)
            {
            case VoiceID::VoiceRedd808: note = 36; break;
            case VoiceID::VoiceReddSnare: note = 38; break;
            case VoiceID::VoiceClosedHat: note = 42; break;
            case VoiceID::VoiceOpenHat: note = 46; break;
            default: note = 36; break;
            }
        }
        handleMidiNote(note, (id == VoiceID::VoiceReddSnare && midiNote < 0) ? 0.9f : 1.0f);
        for (int i = 0; i < numFrames; ++i)
        {
            float l = 0.f, r = 0.f;
            renderTick(l, r);
            interleavedLR[i * 2] = l;
            interleavedLR[i * 2 + 1] = r;
        }
    }

private:
    double fs_ = 44100.0;
    ShawtyReddParams params_ {};
    std::array<ReddVoiceState, static_cast<size_t>(VoiceID::Count)> voices_ {};
    dsp::TptSvf growlPeak_, subLp_, snareHp_, snarePeak_, hatHp_, openHatHp_;
    dsp::OnePoleDcBlocker dc_;
    dsp::OversampledWaveshaper4x tubeL_, tubeR_;
    dsp::DeterministicRng rng_;
};

} // namespace disklordz::shawtyredd
