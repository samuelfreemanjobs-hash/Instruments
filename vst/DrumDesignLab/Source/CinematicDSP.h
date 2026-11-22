#pragma once

#include "DSP/CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz::cinematic
{

enum class VoiceType : uint8_t
{
    Bass808 = 0,
    Kick,
    Snare,
    Clap,
    ClosedHat,
    OpenHat,
    CinematicPerc,
    Count
};

enum class Mood : uint8_t
{
    Dark = 0,
    Luxury,
    Haunted,
    Aggressive,
    Atmospheric,
    Count
};

inline constexpr std::array<const char*, 7> kVoiceNames {
    "808_Bass", "Kick", "Snare", "Clap", "Closed_Hat", "Open_Hat", "Cinematic_Perc"
};

inline constexpr std::array<const char*, 5> kMoodNames {
    "Dark", "Luxury", "Haunted", "Aggressive", "Atmospheric"
};

struct MoodProfile
{
    float subHz      = 46.25f;
    float pitchMult  = 3.4f;
    float decayScale = 1.0f;
    float drive      = 2.2f;
    float brightness = 0.62f;
    float fmAmount   = 1.4f;
    float spaceMix   = 0.10f;
};

inline constexpr std::array<MoodProfile, 5> kMoodProfiles {{
    { 43.6535f, 3.1f, 1.15f, 2.6f, 0.44f, 1.2f, 0.14f }, // Dark (F1)
    { 48.9994f, 2.7f, 1.05f, 1.6f, 0.72f, 0.9f, 0.12f }, // Luxury (G1)
    { 46.2493f, 3.0f, 1.32f, 2.3f, 0.54f, 2.6f, 0.24f }, // Haunted (F#1)
    { 41.2034f, 4.1f, 0.86f, 4.4f, 0.85f, 2.2f, 0.06f }, // Aggressive (E1)
    { 55.0000f, 2.4f, 1.48f, 1.5f, 0.66f, 1.8f, 0.28f }  // Atmospheric (A1)
}};

// Deterministic sample-by-sample baseline engine (7 voices x 5 moods = 35 designs).
class VoiceDSP
{
public:
    void trigger(
        VoiceType type,
        Mood      mood,
        double    sampleRate,
        float     velocity   = 0.92f,
        uint64_t  seed       = 0xC1AE4A71ULL,
        float     rootHzOverride = 0.0f) noexcept
    {
        type_       = type;
        mood_       = mood;
        sr_         = std::max(8000.0, sampleRate);
        velocity_   = std::clamp(velocity, 0.05f, 1.0f);
        prof_       = kMoodProfiles[static_cast<size_t>(mood) % 5u];
        if (rootHzOverride > 20.0f)
            prof_.subHz = rootHzOverride;

        rng_.reseed(seed, static_cast<uint64_t>(type) * 13ULL + static_cast<uint64_t>(mood) + 1ULL);
        sampleIdx_ = 0;
        phase1_ = phase2_ = modPhase_ = 0.0;

        static constexpr std::array<double, 7> kBaseDurations { 1.65, 0.46, 0.48, 0.44, 0.16, 0.68, 0.78 };
        const double durSec = kBaseDurations[static_cast<size_t>(type) % 7u] * prof_.decayScale;
        totalSamples_       = std::max<int64_t>(128, static_cast<int64_t>(std::ceil(sr_ * durSec)));

        const double nyqCap = sr_ * 0.44;
        lpFilter_.setParams(sr_, std::min(nyqCap, 1400.0 + 13000.0 * prof_.brightness), 0.75, dsp::TptSvf::Mode::LowPass);
        hpFilter_.setParams(sr_, (type == VoiceType::ClosedHat || type == VoiceType::OpenHat) ? std::min(nyqCap, 6800.0) : 1150.0, 0.8, dsp::TptSvf::Mode::HighPass);
        lpFilter_.reset();
        hpFilter_.reset();
        shaper_.prepare(sr_);
        dc_.prepare(sr_, 22.0);
        active_ = true;
    }

    bool isActive() const noexcept { return active_; }
    float rootFrequencyHz() const noexcept { return (type_ == VoiceType::Bass808) ? prof_.subHz : 0.0f; }
    int64_t totalSamples() const noexcept { return totalSamples_; }

    float renderSample() noexcept
    {
        if (!active_ || sampleIdx_ >= totalSamples_)
        {
            active_ = false;
            return 0.0f;
        }

        const double t     = static_cast<double>(sampleIdx_) / sr_;
        const float  noise = rng_.nextSigned();
        double       raw   = 0.0;

        switch (type_)
        {
            case VoiceType::Bass808:
            {
                const double startHz = prof_.subHz * prof_.pitchMult;
                const double instHz  = prof_.subHz + (startHz - prof_.subHz) * std::exp(-t / 0.026);
                phase1_ += instHz / sr_;
                if (phase1_ >= 1.0) phase1_ -= 1.0;
                const double atk  = std::min(1.0, t / 0.0012);
                const double env  = atk * std::exp(-t / (0.58 * prof_.decayScale));
                const double osc  = 0.88 * std::sin(dsp::kTwoPi * phase1_) + 0.14 * std::sin(2.0 * dsp::kTwoPi * phase1_);
                const double snap = static_cast<double>(hpFilter_.process(noise)) * std::exp(-t / 0.0025) * 0.14;
                raw               = osc * env + snap;
                break;
            }
            case VoiceType::Kick:
            {
                const double instHz = 52.0 + 165.0 * prof_.brightness * std::exp(-t / 0.015);
                phase1_ += instHz / sr_;
                if (phase1_ >= 1.0) phase1_ -= 1.0;
                const double env   = std::min(1.0, t / 0.0004) * std::exp(-t / (0.14 * prof_.decayScale));
                const double click = static_cast<double>(hpFilter_.process(noise)) * std::exp(-t / 0.0022) * 0.28;
                raw                = std::sin(dsp::kTwoPi * phase1_) * env + click;
                break;
            }
            case VoiceType::Snare:
            {
                const double f1 = 188.0 + 85.0 * std::exp(-t / 0.010);
                const double f2 = 332.0 + 70.0 * std::exp(-t / 0.008);
                phase1_ = std::fmod(phase1_ + f1 / sr_, 1.0);
                phase2_ = std::fmod(phase2_ + f2 / sr_, 1.0);
                const double body  = (std::sin(dsp::kTwoPi * phase1_) + 0.52 * std::sin(dsp::kTwoPi * phase2_)) * std::exp(-t / 0.045);
                const double wires = static_cast<double>(lpFilter_.process(hpFilter_.process(noise))) * std::exp(-t / (0.095 * prof_.decayScale));
                raw                = 0.58 * body + 0.72 * wires;
                break;
            }
            case VoiceType::Clap:
            {
                double burst = 0.0;
                for (int b = 0; b < 4; ++b)
                {
                    const double onset = b * 0.0105;
                    if (t >= onset)
                        burst += std::exp(-(t - onset) / (b == 3 ? (0.075 * prof_.decayScale) : 0.0055));
                }
                raw = static_cast<double>(lpFilter_.process(hpFilter_.process(noise))) * burst * 0.58;
                break;
            }
            case VoiceType::ClosedHat:
            case VoiceType::OpenHat:
            {
                static constexpr std::array<double, 6> kRatios { 205.3, 304.4, 369.6, 522.7, 540.5, 800.0 };
                double metal = 0.0;
                for (size_t i = 0; i < kRatios.size(); ++i)
                {
                    const double f = std::min(kRatios[i] * (0.9 + 0.3 * prof_.brightness) * 4.2, sr_ * 0.43);
                    metal += (0.68 / static_cast<double>(i + 1)) * std::sin(dsp::kTwoPi * f * t + static_cast<double>(i) * 0.5);
                }
                const double tau = (type_ == VoiceType::ClosedHat ? 0.028 : 0.22) * prof_.decayScale;
                const double env = std::min(1.0, t / 0.0003) * std::exp(-t / tau);
                raw              = (0.48 * metal + 0.52 * static_cast<double>(hpFilter_.process(noise))) * env;
                break;
            }
            case VoiceType::CinematicPerc:
            case VoiceType::Count:
            {
                const double carHz = 310.0 + 420.0 * prof_.brightness * std::exp(-t / 0.06);
                modPhase_          = std::fmod(modPhase_ + (carHz * 2.718) / sr_, 1.0);
                const double mod   = std::sin(dsp::kTwoPi * modPhase_) * prof_.fmAmount * std::exp(-t / 0.07);
                phase1_            = std::fmod(phase1_ + carHz / sr_, 1.0);
                const double bell  = std::sin(dsp::kTwoPi * phase1_ + mod) * std::exp(-t / (0.24 * prof_.decayScale));
                const double mode2 = 0.35 * std::sin(dsp::kTwoPi * std::min(carHz * 1.58, sr_ * 0.43) * t) * std::exp(-t / 0.14);
                raw                = bell + mode2;
                break;
            }
        }

        float shaped = shaper_.process(static_cast<float>(raw), prof_.drive, dsp::OversampledWaveshaper4x::Curve::AsymmetricClip);
        shaped       = dc_.process(shaped);

        // Smooth 6 ms terminal anti-click fade
        const int64_t rem      = totalSamples_ - sampleIdx_;
        const int64_t fadeSamp = std::max<int64_t>(32, static_cast<int64_t>(0.006 * sr_));
        const float   tailFade = (rem < fadeSamp) ? (static_cast<float>(rem) / static_cast<float>(fadeSamp)) : 1.0f;

        ++sampleIdx_;
        if (sampleIdx_ >= totalSamples_)
            active_ = false;
        return std::clamp(shaped * tailFade * velocity_ * 0.78f, -0.98f, 0.98f);
    }

    static std::vector<float> renderOneShot(VoiceType type, Mood mood, double sampleRate = 48000.0, uint64_t seed = 0xC1AE4A71ULL)
    {
        VoiceDSP v;
        v.trigger(type, mood, sampleRate, 0.95f, seed);
        std::vector<float> out(static_cast<size_t>(v.totalSamples()), 0.0f);
        for (size_t i = 0; i < out.size(); ++i)
            out[i] = v.renderSample();
        return out;
    }

private:
    VoiceType                     type_         = VoiceType::Bass808;
    Mood                          mood_         = Mood::Dark;
    MoodProfile                   prof_ {};
    double                        sr_           = 48000.0;
    float                         velocity_     = 0.9f;
    int64_t                       totalSamples_ = 0;
    int64_t                       sampleIdx_    = 0;
    double                        phase1_       = 0.0;
    double                        phase2_       = 0.0;
    double                        modPhase_     = 0.0;
    bool                          active_       = false;
    dsp::DeterministicRng         rng_;
    dsp::TptSvf                   lpFilter_, hpFilter_;
    dsp::OversampledWaveshaper4x  shaper_;
    dsp::OnePoleDcBlocker         dc_;
};

} // namespace disklordz::cinematic
