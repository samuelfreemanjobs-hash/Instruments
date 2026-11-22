#pragma once
// DiskLordz Purps / 808 Mafia — portable C++20 (no JUCE).
// Hard-clipped 808 · hybrid DMX snare · 165 BPM half/double-time bounce · dark filtered perc.
#include "CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz::purps
{

struct PurpsParams
{
    float subFreq = 45.0f;
    float knockHz = 180.0f;
    float knockMs = 21.0f;
    float subDecayMs = 880.0f;
    float hardClip = 0.82f;
    float preDrive = 2.3f;

    float snareBodyHz = 210.0f;
    float snareStartHz = 330.0f;
    float snareNoiseHz = 1900.0f;
    float snareNoiseQ = 1.4f;
    float snareDecayMs = 85.0f;
    float snareFmAmt = 0.12f;
    float snareClip = 1.5f;

    float hatDecayMs = 28.0f;
    float openHatDecayMs = 160.0f;
    float rimHz = 680.0f;
    float rimDecayMs = 38.0f;
    float darkLpHz = 3200.0f; // underwater pocket for rim / open accents
    float darkLpQ = 0.9f;

    float stereoWidth = 0.26f;
    float outputGain = 0.90f;
    float bpm = 165.0f;
};

struct PurpsVoiceState
{
    bool active = false;
    float timeSec = 0.0f;
    float velocity = 1.0f;
    double phase = 0.0;
    double phase2 = 0.0;
    float noteFreq = 45.0f;
    std::array<double, 6> hatPhases {};
};

enum class VoiceID : uint8_t
{
    Voice808 = 0,       // 36 + melodic 48–72
    VoiceSnare = 1,     // 38 / 40
    VoiceClosedHat = 2, // 42 / 44
    VoiceOpenHat = 3,   // 46
    VoiceRim = 4,       // 37
    VoiceCrash = 5,     // 49
    Count = 6
};

inline float midiToHz(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline float hardClip(float x, float limit) noexcept
{
    return std::clamp(x, -limit, limit);
}

inline PurpsParams presetGrapeful() noexcept
{
    PurpsParams p {};
    p.subFreq = 45.0f;
    p.knockHz = 180.0f;
    p.hardClip = 0.82f;
    p.preDrive = 2.3f;
    p.bpm = 165.0f;
    return p;
}

inline PurpsParams presetRubberz() noexcept
{
    PurpsParams p {};
    p.subFreq = 47.0f;
    p.knockMs = 19.0f;
    p.snareBodyHz = 205.0f;
    p.snareNoiseHz = 1850.0f;
    p.darkLpHz = 2800.0f;
    p.bpm = 160.0f;
    return p;
}

struct PatternHit
{
    int step = 0;
    int midiNote = 36;
    float velocity = 0.9f;
};

/** 4-bar Purps / 808 Mafia bounce at cfg.bpm (default 165). */
inline void makeFourBarMafiaBounce(const PurpsParams& p, std::vector<PatternHit>& out) noexcept
{
    out.clear();
    auto add = [&](int step, int note, float vel) {
        PatternHit h;
        h.step = step;
        h.midiNote = note;
        h.velocity = vel;
        out.push_back(h);
    };

    for (int bar = 0; bar < 4; ++bar)
    {
        const int off = bar * 16;
        // Hard 808 Mafia bounce
        for (int s : { 0, 3, 6, 10, 14 })
            add(off + s, 36, 0.96f);
        // Snare on step 8 (double-time pocket) + odd-bar ghost on 15
        add(off + 8, 38, 0.95f);
        if ((bar % 2) == 1)
            add(off + 15, 38, 0.55f);
        // Open hat accent
        add(off + 6, 46, 0.62f);
        // Driving 8ths + stutter before snare
        for (int s = 0; s < 16; s += 2)
            add(off + s, 42, 0.58f);
        add(off + 7, 42, 0.48f);
        // Dark filtered rim pockets
        add(off + 2, 37, 0.45f);
        add(off + 11, 37, 0.40f);
        if (bar == 0)
            add(off + 0, 49, 0.60f);
    }
    (void)p;
}

class Purps808MafiaEngine
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        snareBp_.setParams(fs_, 1900.0, 1.4, dsp::TptSvf::Mode::BandPass);
        hatHp_.setParams(fs_, 7600.0, 0.707, dsp::TptSvf::Mode::HighPass);
        openHp_.setParams(fs_, 6200.0, 0.707, dsp::TptSvf::Mode::HighPass);
        crashHp_.setParams(fs_, 4800.0, 0.707, dsp::TptSvf::Mode::HighPass);
        darkLp_.setParams(fs_, 3200.0, 0.9, dsp::TptSvf::Mode::LowPass);
        subLp_.setParams(fs_, 95.0, 0.707, dsp::TptSvf::Mode::LowPass);
        dc_.prepare(fs_, 22.0);
        tube_.prepare(fs_);
        rng_.reseed(0x8084AF1AULL, 9ULL);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = PurpsVoiceState {};
        snareBp_.reset();
        hatHp_.reset();
        openHp_.reset();
        crashHp_.reset();
        darkLp_.reset();
        subLp_.reset();
        dc_.reset();
        tube_.reset();
        pattern_.clear();
        patternActive_ = false;
        sampleCursor_ = 0;
        nextHit_ = 0;
    }

    void setParams(const PurpsParams& p) noexcept
    {
        params_ = p;
        snareBp_.setParams(fs_, p.snareNoiseHz, p.snareNoiseQ, dsp::TptSvf::Mode::BandPass);
        darkLp_.setParams(fs_, p.darkLpHz, p.darkLpQ, dsp::TptSvf::Mode::LowPass);
    }

    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        if (velocity01 <= 0.0f)
            return;
        const float vel = std::clamp(velocity01, 0.05f, 1.0f);
        int id = -1;
        float freq = params_.subFreq;

        if (midiNote == 36 || (midiNote >= 48 && midiNote <= 72))
        {
            id = static_cast<int>(VoiceID::Voice808);
            if (midiNote >= 48)
                freq = midiToHz(midiNote - 24);
        }
        else if (midiNote == 38 || midiNote == 40)
            id = static_cast<int>(VoiceID::VoiceSnare);
        else if (midiNote == 42 || midiNote == 44)
            id = static_cast<int>(VoiceID::VoiceClosedHat);
        else if (midiNote == 46)
            id = static_cast<int>(VoiceID::VoiceOpenHat);
        else if (midiNote == 37)
            id = static_cast<int>(VoiceID::VoiceRim);
        else if (midiNote == 49 || midiNote == 51)
            id = static_cast<int>(VoiceID::VoiceCrash);

        if (id < 0)
            return;

        if (id == static_cast<int>(VoiceID::VoiceClosedHat))
            voices_[static_cast<size_t>(VoiceID::VoiceOpenHat)].active = false;

        auto& v = voices_[static_cast<size_t>(id)];
        v.active = true;
        v.timeSec = 0.0f;
        v.velocity = vel;
        v.phase = 0.0;
        v.phase2 = 0.0;
        v.noteFreq = freq;
        if (id == static_cast<int>(VoiceID::VoiceClosedHat)
            || id == static_cast<int>(VoiceID::VoiceOpenHat)
            || id == static_cast<int>(VoiceID::VoiceCrash))
            v.hatPhases = {};
    }

    void startMafiaPattern() noexcept
    {
        makeFourBarMafiaBounce(params_, pattern_);
        patternActive_ = !pattern_.empty();
        sampleCursor_ = 0;
        nextHit_ = 0;
        const double stepSec = (60.0 / static_cast<double>(params_.bpm)) / 4.0;
        stepSamples_ = static_cast<int>(stepSec * fs_);
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        if (patternActive_ && nextHit_ < pattern_.size())
        {
            while (nextHit_ < pattern_.size())
            {
                const int due = pattern_[nextHit_].step * stepSamples_;
                if (due > sampleCursor_)
                    break;
                handleMidiNote(pattern_[nextHit_].midiNote, pattern_[nextHit_].velocity);
                ++nextHit_;
            }
            if (nextHit_ >= pattern_.size())
                patternActive_ = false;
        }
        ++sampleCursor_;

        const auto& p = params_;
        const float dt = 1.0f / static_cast<float>(fs_);
        float low = 0.f, percL = 0.f, percR = 0.f;

        // Hard-clipped 808 Mafia sub
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::Voice808)];
            if (v.active)
            {
                const float pitchEnv = p.knockHz * std::exp(-v.timeSec / (p.knockMs * 0.001f));
                const float freq = v.noteFreq + pitchEnv;
                v.phase = std::fmod(v.phase + static_cast<double>(freq) / fs_, 1.0);
                const float raw = std::sin(static_cast<float>(v.phase * dsp::kTwoPi));
                const float amp = std::exp(-v.timeSec / (p.subDecayMs * 0.001f));
                float s = hardClip(raw * p.preDrive, p.hardClip) * amp * v.velocity;
                s = dc_.process(subLp_.process(s) * 0.55f + s * 0.45f);
                low += s;
                v.timeSec += dt;
                if (amp < 0.0004f)
                    v.active = false;
            }
        }

        // Hybrid trap / DMX snare (triangle body + BP noise + light FM)
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceSnare)];
            if (v.active)
            {
                const float bodyHz =
                    p.snareBodyHz + (p.snareStartHz - p.snareBodyHz) * std::exp(-v.timeSec / 0.014f);
                v.phase = std::fmod(v.phase + static_cast<double>(bodyHz) / fs_, 1.0);
                v.phase2 = std::fmod(v.phase2 + static_cast<double>(bodyHz * 2.7) / fs_, 1.0);
                const float ph = static_cast<float>(v.phase);
                float tri = (4.0f * std::fabs(ph - 0.5f) - 1.0f) * std::exp(-v.timeSec / 0.048f);
                tri += p.snareFmAmt * std::sin(static_cast<float>(v.phase2 * dsp::kTwoPi))
                    * std::exp(-v.timeSec / 0.030f);
                const float snap =
                    snareBp_.process(rng_.nextSigned()) * std::exp(-v.timeSec / (p.snareDecayMs * 0.001f));
                float s = std::tanh((tri * 0.6f + snap * 0.9f) * p.snareClip) * v.velocity;
                percL += s;
                percR += s * (1.0f - p.stereoWidth * 0.12f);
                v.timeSec += dt;
                if (v.timeSec > p.snareDecayMs * 0.007f)
                    v.active = false;
            }
        }

        // Closed hat
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceClosedHat)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] =
                        std::fmod(v.hatPhases[static_cast<size_t>(k)] + ratios[k] / fs_, 1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / (p.hatDecayMs * 0.001f));
                const float hit = hatHp_.process(sum) * env * v.velocity * 0.78f;
                percL += hit * 1.05f;
                percR += hit * 0.95f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // Open hat (dark LP pocket blend)
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceOpenHat)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] =
                        std::fmod(v.hatPhases[static_cast<size_t>(k)] + ratios[k] * 0.96 / fs_, 1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / (p.openHatDecayMs * 0.001f));
                float hit = openHp_.process(sum) * env * v.velocity * 0.65f;
                hit = hit * 0.55f + darkLp_.process(hit) * 0.45f;
                percL += hit * 0.9f;
                percR += hit * 1.1f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // Dark filtered rim
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceRim)];
            if (v.active)
            {
                const float env = std::exp(-v.timeSec / (p.rimDecayMs * 0.001f));
                v.phase = std::fmod(v.phase + static_cast<double>(p.rimHz) / fs_, 1.0);
                float hit = std::sin(static_cast<float>(v.phase * dsp::kTwoPi)) * env;
                hit += (v.timeSec < 0.0025f ? rng_.nextSigned() * 0.3f : 0.f);
                hit = darkLp_.process(std::tanh(hit * 1.6f)) * v.velocity * 0.6f;
                percL += hit;
                percR += hit;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // Crash
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceCrash)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] =
                        std::fmod(v.hatPhases[static_cast<size_t>(k)] + ratios[k] * 0.88 / fs_, 1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / 0.40f);
                const float hit =
                    crashHp_.process(sum + rng_.nextSigned() * 0.18f) * env * v.velocity * 0.48f;
                percL += hit;
                percR += hit;
                v.timeSec += dt;
                if (env < 0.0004f)
                    v.active = false;
            }
        }

        float busL = (low + percL) * p.outputGain;
        float busR = (low + percR) * p.outputGain;
        outL = tube_.process(busL, 1.0f + p.preDrive * 0.08f,
                             dsp::OversampledWaveshaper4x::Curve::SoftTanh, 0.0f);
        outR = tube_.process(busR, 1.0f + p.preDrive * 0.08f,
                             dsp::OversampledWaveshaper4x::Curve::SoftTanh, 0.0f);
    }

    void renderOneShot(VoiceID id, const PurpsParams& p, float* interleavedLR, int numFrames,
                       int midiNote = -1) noexcept
    {
        setParams(p);
        reset();
        int note = midiNote;
        if (note < 0)
        {
            switch (id)
            {
            case VoiceID::Voice808: note = 36; break;
            case VoiceID::VoiceSnare: note = 38; break;
            case VoiceID::VoiceClosedHat: note = 42; break;
            case VoiceID::VoiceOpenHat: note = 46; break;
            case VoiceID::VoiceRim: note = 37; break;
            case VoiceID::VoiceCrash: note = 49; break;
            default: note = 36; break;
            }
        }
        handleMidiNote(note, 0.95f);
        for (int i = 0; i < numFrames; ++i)
        {
            float l = 0.f, r = 0.f;
            renderTick(l, r);
            interleavedLR[i * 2] = l;
            interleavedLR[i * 2 + 1] = r;
        }
    }

private:
    double fs_ = 48000.0;
    PurpsParams params_ {};
    std::array<PurpsVoiceState, static_cast<size_t>(VoiceID::Count)> voices_ {};
    dsp::TptSvf snareBp_, hatHp_, openHp_, crashHp_, darkLp_, subLp_;
    dsp::OnePoleDcBlocker dc_;
    dsp::OversampledWaveshaper4x tube_;
    dsp::DeterministicRng rng_;
    std::vector<PatternHit> pattern_;
    bool patternActive_ = false;
    int sampleCursor_ = 0;
    int nextHit_ = 0;
    int stepSamples_ = 1;
};

} // namespace disklordz::purps
