#pragma once
// Hitmaker Multi-808 Fusion Reactor — Clean Sub + Punch + Harmonic Dirt (phase-aware).
// Portable C++20 · no heap in renderTick · 4x OS waveshape on dirt path only.
#include "CoreMathAndPRNG.h"
#include <cmath>
#include <cstdint>

namespace disklordz::hitmaker
{

struct FusionParams
{
    float rootHz = 46.25f;
    float punchStartHz = 180.0f;
    float punchSweepMs = 22.0f;
    float subDecayMs = 620.0f;
    float punchDecayMs = 140.0f;
    float dirtDecayMs = 280.0f;

    float cleanLevel = 0.85f;
    float punchLevel = 0.55f;
    float dirtLevel = 0.40f;

    float sineTriMorph = 0.22f; // 0=sine 1=triangle
    float gritDrive = 3.2f;
    float wavefold = 0.18f;
    float crossoverHz = 120.0f; // dirt high-passed above this
    float glideMs = 35.0f;
    float polarityPunch = 1.0f; // ±1
    float polarityDirt = 1.0f;
    float outputGain = 0.90f;
};

struct FusionVoice
{
    bool active = false;
    float timeSec = 0.0f;
    float velocity = 1.0f;
    double subPhase = 0.0;
    double punchPhase = 0.0;
    double dirtPhase = 0.0;
    float currentRootHz = 46.25f;
    float targetRootHz = 46.25f;
};

inline FusionParams makeFusionFamily(int familyIndex) noexcept
{
    FusionParams p {};
    switch (familyIndex % 16)
    {
    case 0: // Earthquake
        p.rootHz = 41.2f; p.punchStartHz = 210.f; p.subDecayMs = 720.f; p.cleanLevel = 0.9f;
        p.punchLevel = 0.7f; p.dirtLevel = 0.55f; p.gritDrive = 4.0f; p.wavefold = 0.28f; break;
    case 1: // Trunk Rattle
        p.rootHz = 43.65f; p.subDecayMs = 800.f; p.punchDecayMs = 100.f; p.dirtLevel = 0.45f;
        p.gritDrive = 3.5f; break;
    case 2: // Triple Fusion
        p.cleanLevel = 0.8f; p.punchLevel = 0.75f; p.dirtLevel = 0.65f; p.gritDrive = 3.8f;
        p.wavefold = 0.32f; break;
    case 3: // Dirty Atlanta
        p.gritDrive = 4.5f; p.dirtLevel = 0.7f; p.wavefold = 0.4f; p.crossoverHz = 100.f; break;
    case 4: // Short Punch
        p.subDecayMs = 280.f; p.punchDecayMs = 70.f; p.dirtDecayMs = 90.f; p.punchLevel = 0.85f; break;
    case 5: // Club Bass
        p.punchStartHz = 195.f; p.sineTriMorph = 0.35f; p.gritDrive = 3.0f; break;
    case 6: // Dark Hollow
        p.rootHz = 36.71f; p.dirtLevel = 0.35f; p.gritDrive = 2.4f; p.wavefold = 0.5f; break;
    case 7: // Long Tail
        p.subDecayMs = 980.f; p.dirtDecayMs = 500.f; p.cleanLevel = 0.95f; break;
    case 8: // Clean Heavy
        p.dirtLevel = 0.08f; p.gritDrive = 1.4f; p.wavefold = 0.0f; p.cleanLevel = 1.0f; break;
    case 9: // Distorted Machine
        p.gritDrive = 5.5f; p.wavefold = 0.55f; p.dirtLevel = 0.8f; break;
    case 10: // Rubber
        p.sineTriMorph = 0.55f; p.glideMs = 70.f; p.punchSweepMs = 40.f; break;
    case 11: // Future Glide
        p.glideMs = 95.f; p.subDecayMs = 700.f; p.dirtLevel = 0.3f; break;
    case 12: // Hard Knock
        p.punchStartHz = 240.f; p.punchSweepMs = 12.f; p.punchLevel = 0.9f; break;
    case 13: // Melodic
        p.dirtLevel = 0.15f; p.gritDrive = 1.8f; p.sineTriMorph = 0.12f; break;
    case 14: // Nuclear
        p.gritDrive = 6.0f; p.wavefold = 0.6f; p.dirtLevel = 0.85f; p.punchLevel = 0.8f; break;
    default: // Deep Street
        p.rootHz = 46.25f; p.subDecayMs = 560.f; p.gritDrive = 3.2f; break;
    }
    return p;
}

class Multi808FusionReactor
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        dirtHp_.setParams(fs_, 120.0, 0.707, dsp::TptSvf::Mode::HighPass);
        subLp_.setParams(fs_, 140.0, 0.707, dsp::TptSvf::Mode::LowPass);
        punchBp_.setParams(fs_, 150.0, 1.0, dsp::TptSvf::Mode::BandPass);
        dc_.prepare(fs_, 22.0);
        dirtShaper_.prepare(fs_);
        reset();
    }

    void reset() noexcept
    {
        voice_ = FusionVoice {};
        dirtHp_.reset();
        subLp_.reset();
        punchBp_.reset();
        dc_.reset();
        dirtShaper_.reset();
    }

    void setParams(const FusionParams& p) noexcept
    {
        params_ = p;
        dirtHp_.setParams(fs_, std::clamp(static_cast<double>(p.crossoverHz), 40.0, 800.0), 0.707,
                          dsp::TptSvf::Mode::HighPass);
        subLp_.setParams(fs_, std::clamp(static_cast<double>(p.crossoverHz + 20.0f), 60.0, 400.0), 0.707,
                         dsp::TptSvf::Mode::LowPass);
    }

    void noteOn(float rootHz, float velocity01) noexcept
    {
        const float hz = std::clamp(rootHz, 20.0f, 200.0f);
        if (!voice_.active)
            voice_.currentRootHz = hz;
        voice_.targetRootHz = hz;
        voice_.active = true;
        voice_.timeSec = 0.0f;
        voice_.velocity = std::clamp(velocity01, 0.05f, 1.0f);
        // Align phases at trigger for coherent fusion
        voice_.subPhase = 0.0;
        voice_.punchPhase = 0.0;
        voice_.dirtPhase = 0.0;
    }

    void noteOnMidi(int midiNote, float velocity01, float defaultRootHz) noexcept
    {
        const float hz = (midiNote == 36)
                             ? defaultRootHz
                             : 440.0f * std::pow(2.0f, (static_cast<float>(midiNote - 24) - 69.0f) / 12.0f);
        noteOn(hz, velocity01);
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        outL = outR = 0.0f;
        if (!voice_.active)
            return;

        const auto& p = params_;
        const float dt = 1.0f / static_cast<float>(fs_);
        const float glideCoeff =
            std::exp(-1.0f / (std::max(1.0f, p.glideMs) * 0.001f * static_cast<float>(fs_)));
        voice_.currentRootHz =
            glideCoeff * voice_.currentRootHz + (1.0f - glideCoeff) * voice_.targetRootHz;

        const float t = voice_.timeSec;
        const float subEnv = std::exp(-t / (p.subDecayMs * 0.001f));
        const float punchEnv = std::exp(-t / (p.punchDecayMs * 0.001f));
        const float dirtEnv = std::exp(-t / (p.dirtDecayMs * 0.001f));

        // CORE A — clean sub (LP + DC)
        voice_.subPhase =
            std::fmod(voice_.subPhase + static_cast<double>(voice_.currentRootHz) / fs_, 1.0);
        const float ph = static_cast<float>(voice_.subPhase * dsp::kTwoPi);
        const float sine = std::sin(ph);
        const float tri = (2.0f / static_cast<float>(dsp::kPi))
            * std::asin(std::clamp(sine, -1.0f, 1.0f));
        float clean = sine * (1.0f - p.sineTriMorph) + tri * p.sineTriMorph;
        clean = subLp_.process(clean) * p.cleanLevel * subEnv * voice_.velocity;

        // CORE B — punch body (pitch sweep, BP presence, polarity)
        const float punchFreq = voice_.currentRootHz
            + (p.punchStartHz - voice_.currentRootHz) * std::exp(-t / (p.punchSweepMs * 0.001f));
        voice_.punchPhase =
            std::fmod(voice_.punchPhase + static_cast<double>(punchFreq) / fs_, 1.0);
        float punch = std::sin(static_cast<float>(voice_.punchPhase * dsp::kTwoPi));
        punch = punch * 0.65f + 0.35f * punchBp_.process(punch);
        punch *= p.punchLevel * p.polarityPunch * punchEnv * voice_.velocity;

        // CORE C — harmonic dirt (HP above crossover, 4x OS waveshape)
        voice_.dirtPhase =
            std::fmod(voice_.dirtPhase + static_cast<double>(voice_.currentRootHz * 1.0f) / fs_, 1.0);
        float dirt = std::sin(static_cast<float>(voice_.dirtPhase * dsp::kTwoPi));
        dirt = dirtHp_.process(dirt);
        const auto curve = (p.wavefold > 0.15f) ? dsp::OversampledWaveshaper4x::Curve::Wavefold
                                                : dsp::OversampledWaveshaper4x::Curve::AsymmetricClip;
        dirt = dirtShaper_.process(dirt, p.gritDrive, curve, p.wavefold);
        dirt *= p.dirtLevel * p.polarityDirt * dirtEnv * voice_.velocity;

        // Phase-aware sum: clean + punch share polarity; dirt is parallel
        float fused = clean + punch + dirt;
        fused = dc_.process(fused) * p.outputGain;
        fused = std::tanh(fused); // soft bus glue, not hard clip

        outL = fused;
        outR = fused;

        voice_.timeSec += dt;
        if (subEnv < 0.0004f && punchEnv < 0.0004f && dirtEnv < 0.0004f)
            voice_.active = false;
    }

    void renderOneShot(const FusionParams& p, float* interleavedLR, int numFrames,
                       float rootHz = 0.0f, float velocity = 0.95f) noexcept
    {
        setParams(p);
        reset();
        noteOn(rootHz > 20.0f ? rootHz : p.rootHz, velocity);
        for (int i = 0; i < numFrames; ++i)
        {
            float l = 0.0f, r = 0.0f;
            renderTick(l, r);
            interleavedLR[i * 2] = l;
            interleavedLR[i * 2 + 1] = r;
        }
    }

    bool isActive() const noexcept { return voice_.active; }

private:
    double fs_ = 48000.0;
    FusionParams params_ {};
    FusionVoice voice_ {};
    dsp::TptSvf dirtHp_, subLp_, punchBp_;
    dsp::OnePoleDcBlocker dc_;
    dsp::OversampledWaveshaper4x dirtShaper_;
};

} // namespace disklordz::hitmaker
