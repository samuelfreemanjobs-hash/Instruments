#pragma once
// DiskLordz OVO / Freebandz engine — portable C++20 (no JUCE in the DSP core).
// 40 Underwater downsampler · melodic 808 glide · stereo perc · sub-preserving bus split.
#include "CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>

namespace disklordz::ovofreebandz
{

struct DrakeFutureParams
{
    // 0 = Future/Metro bright · 1 = Drake/OVO underwater
    float ovoUnderwaterMacro = 0.35f;

    float subFreq = 45.0f;
    float punchFreq = 155.0f;
    float punchDecayMs = 26.0f;
    float subDecayMs = 620.0f;
    float gritDrive = 2.4f;
    float glideTimeMs = 45.0f;

    float rimToneHz = 435.0f;
    float rimDecayMs = 48.0f;
    float trapSnareBodyHz = 210.0f;
    float trapSnareDecayMs = 120.0f;
    float hatDecayMs = 34.0f;
    float stereoWidth = 0.35f;

    float outputGain = 0.95f;
};

/** Noah "40"-style ZOH downsampler + reconstruction LPF. Macro 0 = bypass. */
class OvoDownsampler
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        antiAlias_.setParams(fs_, 3200.0, 1.2, dsp::TptSvf::Mode::LowPass);
        reset();
    }

    void setAmount(float macro01) noexcept
    {
        macro_ = std::clamp(macro01, 0.0f, 1.0f);
        const float targetRate = lerp(static_cast<float>(fs_), 6500.0f, macro_);
        stepSize_ = std::max(1.0f, static_cast<float>(fs_) / std::max(targetRate, 1.0f));
        const float cutoff = lerp(18000.0f, 1800.0f, macro_);
        antiAlias_.setParams(fs_, static_cast<double>(cutoff), 1.1, dsp::TptSvf::Mode::LowPass);
    }

    float process(float input) noexcept
    {
        if (macro_ <= 0.01f)
            return input;

        counter_ += 1.0f;
        if (counter_ >= stepSize_)
        {
            counter_ -= stepSize_;
            held_ = input;
        }
        const float filtered = antiAlias_.process(held_);
        return lerp(input, filtered, macro_);
    }

    void reset() noexcept
    {
        counter_ = 0.0f;
        held_ = 0.0f;
        antiAlias_.reset();
    }

private:
    static float lerp(float a, float b, float t) noexcept { return a + (b - a) * t; }

    double fs_ = 44100.0;
    float macro_ = 0.0f;
    float stepSize_ = 1.0f;
    float counter_ = 0.0f;
    float held_ = 0.0f;
    dsp::TptSvf antiAlias_;
};

struct DrumVoiceState
{
    bool active = false;
    float timeSec = 0.0f;
    float velocity = 1.0f;
    double phase = 0.0;
    std::array<double, 6> hatPhases {};
    float currentNoteFreq = 45.0f;
    float targetNoteFreq = 45.0f;
};

enum class VoiceID : uint8_t
{
    Voice808 = 0,    // MIDI 36 + melodic 48–72
    VoiceOvoRim = 1, // 37
    VoiceSnare = 2,  // 38 / 40
    VoiceHat = 3,    // 42 / 44
    Count = 4
};

inline float midiNoteToHz(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline DrakeFutureParams applyUnderwaterMacro(DrakeFutureParams p) noexcept
{
    const float m = std::clamp(p.ovoUnderwaterMacro, 0.0f, 1.0f);
    // Bright Future ←→ dark OVO: grit, implied filter via downsampler; keep musical params
    p.gritDrive = p.gritDrive * (1.0f - 0.45f * m) + 1.25f * m;
    return p;
}

/** Named factory presets (data-driven — LookAndFeel comes after playable shell). */
inline DrakeFutureParams makePreset(const char* name) noexcept
{
    DrakeFutureParams p {};
    // Default hash by first letter clusters — explicit branches for known names
    if (name != nullptr)
    {
        // "40 Underwater"
        if (name[0] == '4')
        {
            p.ovoUnderwaterMacro = 0.92f;
            p.subFreq = 46.25f;
            p.punchFreq = 130.0f;
            p.punchDecayMs = 32.0f;
            p.subDecayMs = 680.0f;
            p.gritDrive = 1.35f;
            p.glideTimeMs = 55.0f;
            p.rimToneHz = 420.0f;
            p.rimDecayMs = 55.0f;
            p.trapSnareBodyHz = 185.0f;
            p.trapSnareDecayMs = 140.0f;
            p.hatDecayMs = 40.0f;
            p.stereoWidth = 0.22f;
            return p;
        }
        // "Metro Dark Spinz" / Metro
        if (name[0] == 'M' || name[0] == 'm')
        {
            p.ovoUnderwaterMacro = 0.18f;
            p.subFreq = 41.2f;
            p.punchFreq = 175.0f;
            p.punchDecayMs = 18.0f;
            p.subDecayMs = 720.0f;
            p.gritDrive = 3.1f;
            p.glideTimeMs = 70.0f;
            p.trapSnareBodyHz = 220.0f;
            p.trapSnareDecayMs = 100.0f;
            p.hatDecayMs = 28.0f;
            p.stereoWidth = 0.42f;
            return p;
        }
        // "Boi-1da Dancehall Bounce"
        if (name[0] == 'B' || name[0] == 'b')
        {
            p.ovoUnderwaterMacro = 0.40f;
            p.subFreq = 49.0f;
            p.punchFreq = 160.0f;
            p.punchDecayMs = 22.0f;
            p.subDecayMs = 480.0f;
            p.gritDrive = 2.0f;
            p.glideTimeMs = 25.0f;
            p.rimToneHz = 460.0f;
            p.rimDecayMs = 40.0f;
            p.trapSnareBodyHz = 200.0f;
            p.hatDecayMs = 36.0f;
            p.stereoWidth = 0.38f;
            return p;
        }
        // "Southside 808 Mafia"
        if (name[0] == 'S' || name[0] == 's')
        {
            p.ovoUnderwaterMacro = 0.08f;
            p.subFreq = 43.65f;
            p.punchFreq = 195.0f;
            p.punchDecayMs = 14.0f;
            p.subDecayMs = 520.0f;
            p.gritDrive = 3.4f;
            p.glideTimeMs = 40.0f;
            p.trapSnareBodyHz = 230.0f;
            p.trapSnareDecayMs = 90.0f;
            p.hatDecayMs = 24.0f;
            p.stereoWidth = 0.30f;
            return p;
        }
    }
    // Future / Freebandz default
    p.ovoUnderwaterMacro = 0.12f;
    p.gritDrive = 2.8f;
    return p;
}

class DrakeFutureEngine
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        downsampler_.prepare(fs_);
        dcBlock_.prepare(fs_, 22.0);
        snareHp_.setParams(fs_, 165.0, 0.707, dsp::TptSvf::Mode::HighPass);
        // Peak via BP with mild Q as presence boost approximation
        snarePeak_.setParams(fs_, 4200.0, 1.6, dsp::TptSvf::Mode::BandPass);
        hatHp_.setParams(fs_, 7400.0, 0.707, dsp::TptSvf::Mode::HighPass);
        ovoRimLp_.setParams(fs_, 1650.0, 1.4, dsp::TptSvf::Mode::LowPass);
        subLp_.setParams(fs_, 850.0, 0.707, dsp::TptSvf::Mode::LowPass);
        hatLp_.setParams(fs_, 14000.0, 0.707, dsp::TptSvf::Mode::LowPass);
        rng_.reseed(0x40FEEE01ULL, 7ULL);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = DrumVoiceState {};
        downsampler_.reset();
        dcBlock_.reset();
        snareHp_.reset();
        snarePeak_.reset();
        hatHp_.reset();
        ovoRimLp_.reset();
        subLp_.reset();
        hatLp_.reset();
    }

    void setParams(const DrakeFutureParams& p) noexcept
    {
        params_ = applyUnderwaterMacro(p);
        downsampler_.setAmount(params_.ovoUnderwaterMacro);
        const float m = params_.ovoUnderwaterMacro;
        const float subLpHz = lerp(2500.0f, 220.0f, m);
        const float hatLpHz = lerp(16000.0f, 2800.0f, m);
        const float rimLpHz = lerp(4200.0f, 1400.0f, m);
        subLp_.setParams(fs_, static_cast<double>(subLpHz), 0.707, dsp::TptSvf::Mode::LowPass);
        hatLp_.setParams(fs_, static_cast<double>(hatLpHz), 0.707, dsp::TptSvf::Mode::LowPass);
        ovoRimLp_.setParams(fs_, static_cast<double>(rimLpHz), 1.4, dsp::TptSvf::Mode::LowPass);
    }

    /** C1=808 root, C2–C4 melodic 808, C#1 rim, D1/E1 snare, F#1/G#1 hat. */
    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        if (velocity01 <= 0.0f)
            return;
        const float vel = std::clamp(velocity01, 0.05f, 1.0f);

        if (midiNote == 36 || (midiNote >= 48 && midiNote <= 72))
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::Voice808)];
            const float noteHz = (midiNote == 36) ? params_.subFreq : midiNoteToHz(midiNote - 24);
            if (!v.active)
                v.currentNoteFreq = noteHz;
            v.targetNoteFreq = noteHz;
            v.active = true;
            v.timeSec = 0.0f;
            v.velocity = vel;
            return;
        }

        int id = -1;
        if (midiNote == 37)
            id = static_cast<int>(VoiceID::VoiceOvoRim);
        else if (midiNote == 38 || midiNote == 40)
            id = static_cast<int>(VoiceID::VoiceSnare);
        else if (midiNote == 42 || midiNote == 44)
            id = static_cast<int>(VoiceID::VoiceHat);

        if (id < 0)
            return;

        auto& v = voices_[static_cast<size_t>(id)];
        v.active = true;
        v.timeSec = 0.0f;
        v.velocity = vel;
        v.phase = 0.0;
        if (id == static_cast<int>(VoiceID::VoiceHat))
            v.hatPhases = {};
    }

    void noteOff(int /*midiNote*/) noexcept
    {
        // One-shots: ignore note-off (drums are envelope-driven)
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        const float dt = 1.0f / static_cast<float>(fs_);
        float subBus = 0.0f;
        float percL = 0.0f;
        float percR = 0.0f;
        const auto& p = params_;

        // 808 — mono, glide, saturate, LP, DC block (never downsampled)
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::Voice808)];
            if (v.active)
            {
                const float glideMs = std::max(1.0f, p.glideTimeMs);
                const float glideCoeff = std::exp(-1.0f / (glideMs * 0.001f * static_cast<float>(fs_)));
                v.currentNoteFreq = glideCoeff * v.currentNoteFreq + (1.0f - glideCoeff) * v.targetNoteFreq;

                const float punchTau = p.punchDecayMs * 0.001f;
                const float freq = v.currentNoteFreq
                    + (p.punchFreq - v.currentNoteFreq) * std::exp(-v.timeSec / punchTau);
                v.phase = std::fmod(v.phase + static_cast<double>(freq) / fs_, 1.0);

                const float sine = std::sin(static_cast<float>(v.phase * dsp::kTwoPi));
                const float env = std::exp(-v.timeSec / (p.subDecayMs * 0.001f));
                const float drive = lerp(p.gritDrive, 1.25f, p.ovoUnderwaterMacro);
                float sat = std::tanh(sine * drive) * env * v.velocity;
                sat = subLp_.process(sat);
                subBus = dcBlock_.process(sat);

                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // OVO rim
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceOvoRim)];
            if (v.active)
            {
                const float env = std::exp(-v.timeSec / (p.rimDecayMs * 0.001f));
                const float pitch = p.rimToneHz * (1.0f + 0.65f * std::exp(-v.timeSec / 0.007f));
                v.phase = std::fmod(v.phase + static_cast<double>(pitch) / fs_, 1.0);
                const float body = std::sin(static_cast<float>(v.phase * dsp::kTwoPi)) * 0.75f;
                const float click = (v.timeSec < 0.0035f) ? rng_.nextSigned() * 0.5f : 0.0f;
                float hit = std::tanh((body + click) * 1.7f) * env * v.velocity;
                hit = ovoRimLp_.process(hit);
                percL += hit;
                percR += hit * (1.0f - p.stereoWidth * 0.15f);
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // Future / Metro snare
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceSnare)];
            if (v.active)
            {
                const float tau = p.trapSnareDecayMs * 0.001f;
                const float bodyFreq = p.trapSnareBodyHz * (0.65f + 0.85f * std::exp(-v.timeSec / 0.015f));
                v.phase = std::fmod(v.phase + static_cast<double>(bodyFreq) / fs_, 1.0);
                const float body = std::sin(static_cast<float>(v.phase * dsp::kTwoPi))
                    * std::exp(-v.timeSec / (tau * 0.45f));
                const float noiseL = rng_.nextSigned() * std::exp(-v.timeSec / tau);
                const float noiseRRaw = rng_.nextSigned() * std::exp(-v.timeSec / tau);
                const float noiseR = lerp(noiseL, noiseRRaw, p.stereoWidth);

                float hitL = snareHp_.process(body * 0.45f + noiseL * 0.65f);
                hitL = hitL + 0.35f * snarePeak_.process(hitL);
                const float hitR = body * 0.45f + noiseR * 0.65f;

                percL += std::tanh(hitL * 1.5f) * v.velocity;
                percR += std::tanh(hitR * 1.5f) * v.velocity;
                v.timeSec += dt;
                if (v.timeSec > tau * 6.0f)
                    v.active = false;
            }
        }

        // Dual-style hat
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceHat)];
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
                float hit = hatHp_.process(sum);
                hit = hatLp_.process(hit) * env * v.velocity;
                percL += hit * (1.0f + p.stereoWidth * 0.25f);
                percR += hit * (1.0f - p.stereoWidth * 0.25f);
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // 40 downsampler on percussion only — keep 808 sub solid
        percL = downsampler_.process(percL);
        percR = downsampler_.process(percR);

        outL = std::tanh((subBus + percL) * p.outputGain);
        outR = std::tanh((subBus + percR) * p.outputGain);
    }

    /** Offline one-shot into interleaved stereo float buffer (L,R,L,R…). */
    void renderOneShot(VoiceID id, const DrakeFutureParams& p, float* interleavedLR, int numFrames,
                       int midiNoteFor808 = 36) noexcept
    {
        setParams(p);
        reset();
        if (id == VoiceID::Voice808)
            handleMidiNote(midiNoteFor808, 1.0f);
        else if (id == VoiceID::VoiceOvoRim)
            handleMidiNote(37, 1.0f);
        else if (id == VoiceID::VoiceSnare)
            handleMidiNote(38, 1.0f);
        else
            handleMidiNote(42, 1.0f);

        for (int i = 0; i < numFrames; ++i)
        {
            float l = 0.0f, r = 0.0f;
            renderTick(l, r);
            interleavedLR[i * 2] = l;
            interleavedLR[i * 2 + 1] = r;
        }
    }

    bool anyActive() const noexcept
    {
        for (const auto& v : voices_)
            if (v.active)
                return true;
        return false;
    }

private:
    static float lerp(float a, float b, float t) noexcept { return a + (b - a) * std::clamp(t, 0.0f, 1.0f); }

    double fs_ = 44100.0;
    DrakeFutureParams params_ {};
    std::array<DrumVoiceState, static_cast<size_t>(VoiceID::Count)> voices_ {};
    OvoDownsampler downsampler_;
    dsp::OnePoleDcBlocker dcBlock_;
    dsp::TptSvf snareHp_, snarePeak_, hatHp_, ovoRimLp_, subLp_, hatLp_;
    dsp::DeterministicRng rng_;
};

} // namespace disklordz::ovofreebandz
