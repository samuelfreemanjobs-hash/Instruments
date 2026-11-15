#pragma once
// DiskLordz Tay Keith Memphis — portable C++20 (no JUCE).
// Boomy overdriven 808 · stacked snare+clap · 8th hats + pre-snare stutters · rim accents.
#include "CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz::taykeith
{

struct TayKeithParams
{
    float subFreq = 45.0f;
    float knockHz = 175.0f;
    float knockMs = 24.0f;
    float subDecayMs = 950.0f;
    float drive = 2.1f;
    float evenHarmonic = 0.12f; // x*x warmth in 2nd harmonic region

    float snareBodyHz = 195.0f;
    float snareStartHz = 310.0f;
    float snareNoiseHz = 1600.0f;
    float snareDecayMs = 95.0f;
    float snareClip = 1.4f;

    float hatDecayMs = 30.0f;
    float openHatDecayMs = 180.0f;
    float rimHz = 650.0f;
    float rimDecayMs = 40.0f;
    float crashHpHz = 4500.0f;
    float crashDecayMs = 420.0f;

    float stereoWidth = 0.28f;
    float outputGain = 0.92f;
    float bpm = 148.0f;
};

struct TkVoiceState
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
    Voice808 = 0,      // 36 + melodic 48–72
    VoiceSnareStack = 1, // 38 / 40
    VoiceClosedHat = 2,  // 42 / 44
    VoiceOpenHat = 3,    // 46
    VoiceRim = 4,        // 37
    VoiceCrash = 5,      // 49
    Count = 6
};

inline float midiToHz(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline TayKeithParams presetNonstopClub() noexcept
{
    TayKeithParams p {};
    p.subFreq = 44.0f;
    p.knockHz = 175.0f;
    p.drive = 2.2f;
    p.bpm = 150.0f;
    return p;
}

inline TayKeithParams presetLookAlive() noexcept
{
    TayKeithParams p {};
    p.subFreq = 46.25f;
    p.knockMs = 22.0f;
    p.snareBodyHz = 200.0f;
    p.bpm = 148.0f;
    return p;
}

/** 16-step Tay Keith bounce at cfg.bpm — returns (step, padMidi, velocity). */
struct PatternHit
{
    int step = 0;
    int midiNote = 36;
    float velocity = 0.9f;
};

inline void makeFourBarBounce(const TayKeithParams& p, std::vector<PatternHit>& out) noexcept
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
        // Boomy 808: 0, 3, 7, 10, 14
        for (int s : { 0, 3, 7, 10, 14 })
            add(off + s, 36, 0.98f);
        // Snare stack on 2 & 4
        add(off + 4, 38, 0.95f);
        add(off + 12, 38, 0.95f);
        // Open hat accent step 2
        add(off + 2, 46, 0.65f);
        // Driving 8ths
        for (int s = 0; s < 16; s += 2)
            add(off + s, 42, 0.58f);
        // Sharp roll before beat-4 snare
        add(off + 11, 42, 0.48f);
        add(off + 11, 42, 0.42f); // stutter density (same step; engine retriggers)
        // Rim off-beats
        add(off + 6, 37, 0.55f);
        add(off + 14, 37, 0.50f);
        // Crash on bar 1 downbeat
        if (bar == 0)
            add(off + 0, 49, 0.70f);
    }
    (void)p;
}

class TayKeithMemphisEngine
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        snareBp_.setParams(fs_, 1600.0, 1.1, dsp::TptSvf::Mode::BandPass);
        hatHp_.setParams(fs_, 7200.0, 0.707, dsp::TptSvf::Mode::HighPass);
        openHp_.setParams(fs_, 6000.0, 0.707, dsp::TptSvf::Mode::HighPass);
        crashHp_.setParams(fs_, 4500.0, 0.707, dsp::TptSvf::Mode::HighPass);
        growlBp_.setParams(fs_, 180.0, 1.0, dsp::TptSvf::Mode::BandPass);
        subLp_.setParams(fs_, 90.0, 0.707, dsp::TptSvf::Mode::LowPass);
        dc_.prepare(fs_, 22.0);
        tube_.prepare(fs_);
        rng_.reseed(0x7A71E174ULL, 8ULL);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = TkVoiceState {};
        snareBp_.reset();
        hatHp_.reset();
        openHp_.reset();
        crashHp_.reset();
        growlBp_.reset();
        subLp_.reset();
        dc_.reset();
        tube_.reset();
        pattern_.clear();
        patternActive_ = false;
        sampleCursor_ = 0;
        nextHit_ = 0;
    }

    void setParams(const TayKeithParams& p) noexcept
    {
        params_ = p;
        snareBp_.setParams(fs_, p.snareNoiseHz, 1.1, dsp::TptSvf::Mode::BandPass);
        crashHp_.setParams(fs_, p.crashHpHz, 0.707, dsp::TptSvf::Mode::HighPass);
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
            id = static_cast<int>(VoiceID::VoiceSnareStack);
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

    /** Arm the 4-bar bounce sequencer (sample-accurate via renderTick). */
    void startBouncePattern() noexcept
    {
        makeFourBarBounce(params_, pattern_);
        patternActive_ = !pattern_.empty();
        sampleCursor_ = 0;
        nextHit_ = 0;
        const double stepSec = (60.0 / static_cast<double>(params_.bpm)) / 4.0;
        stepSamples_ = static_cast<int>(stepSec * fs_);
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        // Pattern scheduler
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

        // Boomy 808
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::Voice808)];
            if (v.active)
            {
                const float pitchEnv = p.knockHz * std::exp(-v.timeSec / (p.knockMs * 0.001f));
                const float freq = v.noteFreq + pitchEnv;
                v.phase = std::fmod(v.phase + static_cast<double>(freq) / fs_, 1.0);
                const float raw = std::sin(static_cast<float>(v.phase * dsp::kTwoPi));
                const float amp = std::exp(-v.timeSec / (p.subDecayMs * 0.001f));
                // Asymmetric drive + even harmonic — sub stays via LP blend
                float driven = std::tanh(raw * p.drive) + p.evenHarmonic * raw * raw;
                float growl = growlBp_.process(driven);
                float sub = subLp_.process(driven);
                float s = dc_.process((sub * 0.7f + growl * 0.35f) * amp * v.velocity);
                low += s;
                v.timeSec += dt;
                if (amp < 0.0004f)
                    v.active = false;
            }
        }

        // Stacked snare + clap
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceSnareStack)];
            if (v.active)
            {
                const float bodyHz =
                    p.snareBodyHz + (p.snareStartHz - p.snareBodyHz) * std::exp(-v.timeSec / 0.015f);
                v.phase = std::fmod(v.phase + static_cast<double>(bodyHz) / fs_, 1.0);
                // Triangle body
                const float ph = static_cast<float>(v.phase);
                const float triBody =
                    (4.0f * std::fabs(ph - 0.5f) - 1.0f) * std::exp(-v.timeSec / 0.055f);
                const float clapNoise =
                    snareBp_.process(rng_.nextSigned()) * std::exp(-v.timeSec / (p.snareDecayMs * 0.001f));
                float s = std::tanh((triBody * 0.65f + clapNoise * 0.85f) * p.snareClip) * v.velocity;
                percL += s;
                percR += s * (1.0f - p.stereoWidth * 0.15f);
                v.timeSec += dt;
                if (v.timeSec > p.snareDecayMs * 0.006f)
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
                const float hit = hatHp_.process(sum) * env * v.velocity * 0.8f;
                percL += hit * 1.05f;
                percR += hit * 0.95f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // Open hat
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceOpenHat)];
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
                const float env = std::exp(-v.timeSec / (p.openHatDecayMs * 0.001f));
                const float hit = openHp_.process(sum) * env * v.velocity * 0.7f;
                percL += hit * 0.9f;
                percR += hit * 1.1f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // Dry rim / woodblock
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceRim)];
            if (v.active)
            {
                const float env = std::exp(-v.timeSec / (p.rimDecayMs * 0.001f));
                v.phase = std::fmod(v.phase + static_cast<double>(p.rimHz) / fs_, 1.0);
                float hit = std::sin(static_cast<float>(v.phase * dsp::kTwoPi)) * env;
                hit += (v.timeSec < 0.003f ? rng_.nextSigned() * 0.4f : 0.f);
                hit = std::tanh(hit * 1.8f) * v.velocity * 0.7f;
                percL += hit;
                percR += hit;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // High-passed crash
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceCrash)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] =
                        std::fmod(v.hatPhases[static_cast<size_t>(k)] + ratios[k] * 0.85 / fs_, 1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }
                const float env = std::exp(-v.timeSec / (p.crashDecayMs * 0.001f));
                const float hit = crashHp_.process(sum + rng_.nextSigned() * 0.2f) * env * v.velocity * 0.55f;
                percL += hit;
                percR += hit;
                v.timeSec += dt;
                if (env < 0.0004f)
                    v.active = false;
            }
        }

        float busL = (low + percL) * p.outputGain;
        float busR = (low + percR) * p.outputGain;
        outL = tube_.process(busL, 1.0f + p.drive * 0.15f,
                             dsp::OversampledWaveshaper4x::Curve::AsymmetricClip, 0.05f);
        outR = tube_.process(busR, 1.0f + p.drive * 0.15f,
                             dsp::OversampledWaveshaper4x::Curve::AsymmetricClip, 0.05f);
    }

    void renderOneShot(VoiceID id, const TayKeithParams& p, float* interleavedLR, int numFrames,
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
            case VoiceID::VoiceSnareStack: note = 38; break;
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
    TayKeithParams params_ {};
    std::array<TkVoiceState, static_cast<size_t>(VoiceID::Count)> voices_ {};
    dsp::TptSvf snareBp_, hatHp_, openHp_, crashHp_, growlBp_, subLp_;
    dsp::OnePoleDcBlocker dc_;
    dsp::OversampledWaveshaper4x tube_;
    dsp::DeterministicRng rng_;

    std::vector<PatternHit> pattern_;
    bool patternActive_ = false;
    int sampleCursor_ = 0;
    size_t nextHit_ = 0;
    int stepSamples_ = 1;
};

} // namespace disklordz::taykeith
