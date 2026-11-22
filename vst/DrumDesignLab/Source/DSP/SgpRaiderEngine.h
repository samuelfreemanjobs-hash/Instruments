#pragma once
// DiskLordz SpaceGhostPurrp / Raider Klan engine — portable C++20.
// Murky Miami 808 · dark Memphis snare · metallic triangle · 12-bit cassette bus + dark hall.
#include "CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>

namespace disklordz::sgp
{

struct SgpParams
{
    float subFreq = 44.0f;
    float knockFreq = 125.0f;
    float knockDecayMs = 32.0f;
    float subDecayMs = 560.0f;
    float subWarmth = 2.1f;

    float snareBodyHz = 155.0f;
    float snareToneCutoff = 1350.0f;
    float snareDecayMs = 130.0f;
    float darkReverbMix = 0.28f;

    float triangleFreqHz = 1850.0f;
    float triangleDecayMs = 68.0f;
    float hatDecayMs = 36.0f;

    float cassetteGrit = 0.55f;
    float outputGain = 0.90f;
};

struct SgpVoiceState
{
    bool active = false;
    float timeSec = 0.0f;
    float velocity = 1.0f;
    double phase = 0.0;
    std::array<double, 6> hatPhases {};
};

enum class VoiceID : uint8_t
{
    Voice808 = 0,
    VoiceDarkSnare = 1,
    VoiceLoFiHat = 2,
    VoiceTriangle = 3,
    Count = 4
};

inline float lerp(float a, float b, float t) noexcept
{
    return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
}

inline SgpParams presetMysteriousPhonk() noexcept
{
    SgpParams p {};
    p.cassetteGrit = 0.65f;
    p.snareBodyHz = 148.0f;
    p.darkReverbMix = 0.35f;
    p.subFreq = 42.0f;
    return p;
}

inline SgpParams presetRaiderTriangleBounce() noexcept
{
    SgpParams p {};
    p.triangleFreqHz = 1920.0f;
    p.triangleDecayMs = 55.0f;
    p.hatDecayMs = 34.0f;
    p.cassetteGrit = 0.50f;
    p.darkReverbMix = 0.22f;
    return p;
}

class SgpRaiderEngine
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        subLp_.setParams(fs_, 320.0, 1.1, dsp::TptSvf::Mode::LowPass);
        dcBlock_.prepare(fs_, 22.0);
        snareBp_.setParams(fs_, 1350.0, 1.3, dsp::TptSvf::Mode::BandPass);
        snareHp_.setParams(fs_, 180.0, 0.8, dsp::TptSvf::Mode::HighPass); // mild body lift via blend
        hatHp_.setParams(fs_, 6200.0, 0.707, dsp::TptSvf::Mode::HighPass);
        triHp_.setParams(fs_, 1200.0, 0.707, dsp::TptSvf::Mode::HighPass);
        cassetteLpL_.setParams(fs_, 9500.0, 0.9, dsp::TptSvf::Mode::LowPass);
        cassetteLpR_.setParams(fs_, 9500.0, 0.9, dsp::TptSvf::Mode::LowPass);
        reverb_.prepare(fs_);
        rng_.reseed(0x5C9BAD10ULL, 4ULL);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = SgpVoiceState {};
        subLp_.reset();
        dcBlock_.reset();
        snareBp_.reset();
        snareHp_.reset();
        hatHp_.reset();
        triHp_.reset();
        cassetteLpL_.reset();
        cassetteLpR_.reset();
        reverb_.reset();
        crushCounter_ = 0.0f;
        heldL_ = heldR_ = 0.0f;
    }

    void setParams(const SgpParams& p) noexcept
    {
        params_ = p;
        snareBp_.setParams(fs_, p.snareToneCutoff, 1.3, dsp::TptSvf::Mode::BandPass);
        const float cassetteCutoff = lerp(16000.0f, 8200.0f, p.cassetteGrit);
        cassetteLpL_.setParams(fs_, cassetteCutoff, 0.9, dsp::TptSvf::Mode::LowPass);
        cassetteLpR_.setParams(fs_, cassetteCutoff, 0.9, dsp::TptSvf::Mode::LowPass);
    }

    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        if (velocity01 <= 0.0f)
            return;
        const float vel = std::clamp(velocity01, 0.05f, 1.0f);
        int id = -1;
        if (midiNote == 36)
            id = static_cast<int>(VoiceID::Voice808);
        else if (midiNote == 38 || midiNote == 40)
            id = static_cast<int>(VoiceID::VoiceDarkSnare);
        else if (midiNote == 42 || midiNote == 44)
            id = static_cast<int>(VoiceID::VoiceLoFiHat);
        else if (midiNote == 49 || midiNote == 51)
            id = static_cast<int>(VoiceID::VoiceTriangle);
        if (id < 0)
            return;
        auto& v = voices_[static_cast<size_t>(id)];
        v.active = true;
        v.timeSec = 0.0f;
        v.velocity = vel;
        v.phase = 0.0;
        if (id == static_cast<int>(VoiceID::VoiceLoFiHat))
            v.hatPhases = {};
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        const auto& p = params_;
        const float dt = 1.0f / static_cast<float>(fs_);
        float dryL = 0.0f, dryR = 0.0f, revSend = 0.0f;

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::Voice808)];
            if (v.active)
            {
                const float freq =
                    p.subFreq + (p.knockFreq - p.subFreq) * std::exp(-v.timeSec / (p.knockDecayMs * 0.001f));
                v.phase = std::fmod(v.phase + static_cast<double>(freq) / fs_, 1.0);
                const float osc = std::sin(static_cast<float>(v.phase * dsp::kTwoPi));
                const float env = std::exp(-v.timeSec / (p.subDecayMs * 0.001f));
                float warm = subLp_.process(std::tanh(osc * p.subWarmth));
                warm = dcBlock_.process(warm) * env * v.velocity;
                dryL += warm;
                dryR += warm;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceDarkSnare)];
            if (v.active)
            {
                const float tau = p.snareDecayMs * 0.001f;
                const float bodyFreq = p.snareBodyHz * (0.6f + 0.7f * std::exp(-v.timeSec / 0.018f));
                v.phase = std::fmod(v.phase + static_cast<double>(bodyFreq) / fs_, 1.0);
                const float body = std::sin(static_cast<float>(v.phase * dsp::kTwoPi))
                    * std::exp(-v.timeSec / (tau * 0.5f));
                const float noise = rng_.nextSigned() * std::exp(-v.timeSec / tau);
                const float filteredNoise = snareBp_.process(noise);
                float hit = (body * 0.55f + filteredNoise * 0.65f) * v.velocity;
                hit = hit * 0.85f + 0.15f * snareHp_.process(hit);
                dryL += hit;
                dryR += hit;
                revSend += hit * 0.85f;
                v.timeSec += dt;
                if (v.timeSec > tau * 6.0f)
                    v.active = false;
            }
        }

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceLoFiHat)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.0f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] = std::fmod(
                        v.hatPhases[static_cast<size_t>(k)] + (ratios[k] * 0.92) / fs_, 1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / (p.hatDecayMs * 0.001f));
                const float hit = hatHp_.process(sum) * env * v.velocity * 0.9f;
                dryL += hit * 0.92f;
                dryR += hit * 1.08f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceTriangle)];
            if (v.active)
            {
                const float tau = p.triangleDecayMs * 0.001f;
                const float env = std::exp(-v.timeSec / tau);
                v.phase = std::fmod(v.phase + static_cast<double>(p.triangleFreqHz) / fs_, 1.0);
                const float p1 = std::sin(static_cast<float>(v.phase * dsp::kTwoPi));
                const float p2 = std::sin(static_cast<float>(std::fmod(v.phase * 2.76, 1.0) * dsp::kTwoPi))
                    * 0.55f * std::exp(-v.timeSec / (tau * 0.5f));
                const float p3 = std::sin(static_cast<float>(std::fmod(v.phase * 5.40, 1.0) * dsp::kTwoPi))
                    * 0.30f * std::exp(-v.timeSec / (tau * 0.25f));
                const float hit = triHp_.process(p1 + p2 + p3) * env * v.velocity * 0.65f;
                dryL += hit * 1.15f;
                dryR += hit * 0.85f;
                revSend += hit * 0.45f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        const auto wet = reverb_.process({ revSend, revSend }, p.darkReverbMix, 0.68f, 0.85f);
        float l = dryL + wet.left;
        float r = dryR + wet.right;

        const float decimationStep = 1.0f + p.cassetteGrit * 1.15f;
        crushCounter_ += 1.0f;
        if (crushCounter_ >= decimationStep)
        {
            crushCounter_ -= decimationStep;
            heldL_ = l;
            heldR_ = r;
        }
        float procL = lerp(l, heldL_, p.cassetteGrit);
        float procR = lerp(r, heldR_, p.cassetteGrit);

        if (p.cassetteGrit > 0.05f)
        {
            constexpr float bitLevels = 2048.0f;
            const float qL = std::round(procL * bitLevels) / bitLevels;
            const float qR = std::round(procR * bitLevels) / bitLevels;
            procL = lerp(procL, qL, p.cassetteGrit);
            procR = lerp(procR, qR, p.cassetteGrit);
        }

        procL = std::tanh(cassetteLpL_.process(procL) * (1.0f + p.cassetteGrit * 0.6f) * p.outputGain);
        procR = std::tanh(cassetteLpR_.process(procR) * (1.0f + p.cassetteGrit * 0.6f) * p.outputGain);
        outL = procL;
        outR = procR;
    }

    void renderOneShot(VoiceID id, const SgpParams& p, float* interleavedLR, int numFrames) noexcept
    {
        setParams(p);
        reset();
        int note = 36;
        switch (id)
        {
        case VoiceID::Voice808: note = 36; break;
        case VoiceID::VoiceDarkSnare: note = 38; break;
        case VoiceID::VoiceLoFiHat: note = 42; break;
        case VoiceID::VoiceTriangle: note = 49; break;
        default: break;
        }
        handleMidiNote(note, 1.0f);
        for (int i = 0; i < numFrames; ++i)
        {
            float l = 0.0f, r = 0.0f;
            renderTick(l, r);
            interleavedLR[i * 2] = l;
            interleavedLR[i * 2 + 1] = r;
        }
    }

private:
    double fs_ = 44100.0;
    SgpParams params_ {};
    std::array<SgpVoiceState, static_cast<size_t>(VoiceID::Count)> voices_ {};
    dsp::TptSvf subLp_, snareBp_, snareHp_, hatHp_, triHp_, cassetteLpL_, cassetteLpR_;
    dsp::OnePoleDcBlocker dcBlock_;
    dsp::RoomReverb reverb_;
    dsp::DeterministicRng rng_;
    float crushCounter_ = 0.0f;
    float heldL_ = 0.0f, heldR_ = 0.0f;
};

} // namespace disklordz::sgp
