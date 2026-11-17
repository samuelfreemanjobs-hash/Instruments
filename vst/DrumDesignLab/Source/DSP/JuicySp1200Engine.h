#pragma once
// DiskLordz Juicy J / Three 6 Mafia SP-1200–inspired engine (portable, no JUCE).
// 26.04 kHz ZOH · 12-bit quant · SSM2044 dual-bus · melodic 808 cowbell.
#include "CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>

namespace disklordz::juicysp
{

struct JuicySp1200Params
{
    float pitchDownSemis = -4.0f;
    float ssm2044CutoffHz = 3800.0f;
    float ssm2044Resonance = 1.45f;
    float tapeSaturation = 2.2f;

    float kickPunchHz = 135.0f;
    float subFreq = 46.0f;
    float subDecayMs = 540.0f;

    float snareBodyHz = 185.0f;
    float snareNoiseToneHz = 2200.0f;
    float snareDecayMs = 115.0f;

    float cowbellBaseHz = 540.0f;
    float cowbellDecayMs = 190.0f;

    float outputGain = 0.90f;
};

struct SpVoiceState
{
    bool active = false;
    float timeSec = 0.0f;
    float velocity = 1.0f;
    double phase1 = 0.0;
    double phase2 = 0.0;
    float pitchMultiplier = 1.0f;
    std::array<double, 6> hatPhases {};
};

enum class VoiceID : uint8_t
{
    VoiceSpKick808 = 0, // 36 — SSM filtered
    VoiceSpSnare = 1,   // 38/40 — SSM filtered
    VoiceSpHat = 2,     // 42/44 — unfiltered ch 7/8
    VoiceCowbell = 3,   // 56 or 60–84 melodic
    Count = 4
};

inline float midiToHz(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline float quantize12Bit(float x) noexcept
{
    const float c = std::clamp(x, -1.0f, 1.0f);
    return std::round(c * 2047.0f) / 2047.0f;
}

inline JuicySp1200Params presetBlacklandRadio() noexcept
{
    JuicySp1200Params p {};
    p.pitchDownSemis = -5.0f;
    p.ssm2044CutoffHz = 3200.0f;
    p.ssm2044Resonance = 1.55f;
    p.tapeSaturation = 2.6f;
    p.subFreq = 42.0f;
    p.snareBodyHz = 168.0f;
    p.snareNoiseToneHz = 1800.0f;
    return p;
}

inline JuicySp1200Params presetTripleSixCowbell() noexcept
{
    JuicySp1200Params p {};
    p.pitchDownSemis = -3.0f;
    p.cowbellBaseHz = 540.0f;
    p.cowbellDecayMs = 220.0f;
    p.tapeSaturation = 2.0f;
    p.ssm2044CutoffHz = 4000.0f;
    return p;
}

class JuicySp1200Engine
{
public:
    static constexpr double kSpClockHz = 26040.0;

    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        ssm1_.setParams(fs_, 3800.0, 1.2, dsp::TptSvf::Mode::LowPass);
        ssm2_.setParams(fs_, 3800.0, 0.85, dsp::TptSvf::Mode::LowPass);
        cowbellBp_.setParams(fs_, 780.0, 2.4, dsp::TptSvf::Mode::BandPass);
        snareBp_.setParams(fs_, 2200.0, 1.1, dsp::TptSvf::Mode::BandPass);
        hatHp_.setParams(fs_, 6800.0, 0.707, dsp::TptSvf::Mode::HighPass);
        dcBlock_.prepare(fs_, 22.0);
        rng_.reseed(0x12001200ULL, 6ULL);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = SpVoiceState {};
        ssm1_.reset();
        ssm2_.reset();
        cowbellBp_.reset();
        snareBp_.reset();
        hatHp_.reset();
        dcBlock_.reset();
        clockPhase_ = 0.0;
        heldFiltered_ = 0.0f;
        heldUnfiltered_ = 0.0f;
    }

    void setParams(const JuicySp1200Params& p) noexcept
    {
        params_ = p;
        const float cut = std::clamp(p.ssm2044CutoffHz, 200.0f, 12000.0f);
        const float q = std::clamp(p.ssm2044Resonance, 0.5f, 4.0f);
        ssm1_.setParams(fs_, cut, q, dsp::TptSvf::Mode::LowPass);
        ssm2_.setParams(fs_, cut, 0.85, dsp::TptSvf::Mode::LowPass);
        snareBp_.setParams(fs_, p.snareNoiseToneHz, 1.1, dsp::TptSvf::Mode::BandPass);
    }

    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        if (velocity01 <= 0.0f)
            return;
        const float vel = std::clamp(velocity01, 0.05f, 1.0f);
        int id = -1;
        float pitchMult = 1.0f;

        if (midiNote == 36)
            id = static_cast<int>(VoiceID::VoiceSpKick808);
        else if (midiNote == 38 || midiNote == 40)
            id = static_cast<int>(VoiceID::VoiceSpSnare);
        else if (midiNote == 42 || midiNote == 44)
            id = static_cast<int>(VoiceID::VoiceSpHat);
        else if (midiNote == 56 || (midiNote >= 60 && midiNote <= 84))
        {
            id = static_cast<int>(VoiceID::VoiceCowbell);
            if (midiNote >= 60)
                pitchMult = midiToHz(midiNote) / 540.0f;
        }

        if (id < 0)
            return;

        auto& v = voices_[static_cast<size_t>(id)];
        v.active = true;
        v.timeSec = 0.0f;
        v.velocity = vel;
        v.phase1 = 0.0;
        v.phase2 = 0.0;
        v.pitchMultiplier = pitchMult;
        if (id == static_cast<int>(VoiceID::VoiceSpHat))
            v.hatPhases = {};
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        const auto& p = params_;
        const float pitchDownRatio = std::pow(2.0f, p.pitchDownSemis / 12.0f);
        const double effectiveClock = kSpClockHz * static_cast<double>(std::max(0.45f, pitchDownRatio));
        const double clockStep = fs_ / std::max(1000.0, effectiveClock);

        float filteredBus = 0.0f;
        float unfilteredBus = 0.0f;
        renderVoices(p, pitchDownRatio, filteredBus, unfilteredBus);

        clockPhase_ += 1.0;
        if (clockPhase_ >= clockStep)
        {
            clockPhase_ -= clockStep;
            heldFiltered_ = quantize12Bit(filteredBus);
            heldUnfiltered_ = quantize12Bit(unfilteredBus);
        }

        float analogFiltered = ssm1_.process(heldFiltered_);
        analogFiltered = ssm2_.process(analogFiltered);
        const float summed = analogFiltered + heldUnfiltered_;
        const float master = dcBlock_.process(std::tanh(summed * p.tapeSaturation) * 0.85f * p.outputGain);
        outL = master;
        outR = master;
    }

    void renderOneShot(VoiceID id, const JuicySp1200Params& p, float* interleavedLR, int numFrames,
                       int midiNote = -1) noexcept
    {
        setParams(p);
        reset();
        int note = midiNote;
        if (note < 0)
        {
            switch (id)
            {
            case VoiceID::VoiceSpKick808: note = 36; break;
            case VoiceID::VoiceSpSnare: note = 38; break;
            case VoiceID::VoiceSpHat: note = 42; break;
            case VoiceID::VoiceCowbell: note = 56; break;
            default: note = 36; break;
            }
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
    void renderVoices(const JuicySp1200Params& p, float pitchRatio, float& outFiltered,
                      float& outUnfiltered) noexcept
    {
        const float dt = 1.0f / static_cast<float>(fs_);
        const float twoPi = static_cast<float>(dsp::kTwoPi);

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceSpKick808)];
            if (v.active)
            {
                const float freq = p.subFreq + (p.kickPunchHz - p.subFreq) * std::exp(-v.timeSec / 0.028f);
                v.phase1 = std::fmod(v.phase1 + static_cast<double>(freq) / fs_, 1.0);
                const float osc = std::sin(static_cast<float>(v.phase1) * twoPi);
                const float click = (v.timeSec < 0.005f) ? rng_.nextSigned() * 0.35f : 0.0f;
                const float env = std::exp(-v.timeSec / (p.subDecayMs * 0.001f));
                outFiltered += (osc + click) * env * v.velocity * 1.1f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceSpSnare)];
            if (v.active)
            {
                const float tau = p.snareDecayMs * 0.001f;
                const float bodyFreq =
                    (p.snareBodyHz * pitchRatio) * (0.65f + 0.75f * std::exp(-v.timeSec / 0.016f));
                v.phase1 = std::fmod(v.phase1 + static_cast<double>(bodyFreq) / fs_, 1.0);
                const float body = std::sin(static_cast<float>(v.phase1) * twoPi)
                    * std::exp(-v.timeSec / (tau * 0.45f));
                const float noise = rng_.nextSigned() * std::exp(-v.timeSec / tau);
                const float filteredNoise = snareBp_.process(noise);
                outFiltered += (body * 0.55f + filteredNoise * 0.75f) * v.velocity;
                v.timeSec += dt;
                if (v.timeSec > tau * 6.0f)
                    v.active = false;
            }
        }

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceSpHat)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.0f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] = std::fmod(
                        v.hatPhases[static_cast<size_t>(k)]
                            + (ratios[k] * static_cast<double>(pitchRatio)) / fs_,
                        1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / 0.034f);
                outUnfiltered += hatHp_.process(sum) * env * v.velocity * 0.85f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceCowbell)];
            if (v.active)
            {
                const float tau = p.cowbellDecayMs * 0.001f;
                const float fastEnv = std::exp(-v.timeSec / 0.018f);
                const float slowEnv = std::exp(-v.timeSec / tau);
                const float env = 0.45f * fastEnv + 0.55f * slowEnv;

                const double f1 = static_cast<double>(p.cowbellBaseHz * v.pitchMultiplier);
                const double f2 = f1 * 1.48148; // TR-808 540:800
                v.phase1 = std::fmod(v.phase1 + f1 / fs_, 1.0);
                v.phase2 = std::fmod(v.phase2 + f2 / fs_, 1.0);
                const float sq1 = (v.phase1 < 0.5) ? 0.5f : -0.5f;
                const float sq2 = (v.phase2 < 0.5) ? 0.5f : -0.5f;

                cowbellBp_.setParams(fs_, f1 * 1.4, 2.4, dsp::TptSvf::Mode::BandPass);
                const float bell = cowbellBp_.process(sq1 + sq2) * env * v.velocity * 1.3f;
                outUnfiltered += bell;
                v.timeSec += dt;
                if (slowEnv < 0.0005f)
                    v.active = false;
            }
        }
    }

    double fs_ = 44100.0;
    JuicySp1200Params params_ {};
    std::array<SpVoiceState, static_cast<size_t>(VoiceID::Count)> voices_ {};
    dsp::TptSvf ssm1_, ssm2_, cowbellBp_, snareBp_, hatHp_;
    dsp::OnePoleDcBlocker dcBlock_;
    dsp::DeterministicRng rng_;
    double clockPhase_ = 0.0;
    float heldFiltered_ = 0.0f;
    float heldUnfiltered_ = 0.0f;
};

} // namespace disklordz::juicysp
