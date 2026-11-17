#pragma once

#include "CoreMathAndPRNG.h"

namespace disklordz::dsp
{

// Unconditionally stable coupled-form damped 2-pole modal oscillator (r <= 0.999995).
class ModalOscillator
{
public:
    void configure(double sampleRate, double freqHz, double decayTauSec, float amplitude = 1.0f) noexcept
    {
        const double sr      = std::max(8000.0, sampleRate);
        const double nyqSafe = sr * 0.45;
        if (freqHz <= 12.0 || freqHz >= nyqSafe || amplitude <= 0.0f)
        {
            active_ = false;
            return;
        }
        const double omega = kTwoPi * freqHz / sr;
        const double tau   = std::clamp(decayTauSec, 0.002, 12.0);
        const double r     = std::clamp(std::exp(-1.0 / (tau * sr)), 0.0, 0.999995);

        cosCoeff_ = r * std::cos(omega);
        sinCoeff_ = r * std::sin(omega);
        gain_     = amplitude;
        active_   = true;
    }

    void reset() noexcept
    {
        u_ = 0.0;
        v_ = 0.0;
    }

    void strike(float impulse) noexcept
    {
        if (active_)
            v_ += static_cast<double>(impulse * gain_);
    }

    float process(float excitation = 0.0f) noexcept
    {
        if (!active_)
            return 0.0f;
        const double nextU = cosCoeff_ * u_ - sinCoeff_ * v_;
        const double nextV = sinCoeff_ * u_ + cosCoeff_ * v_ + static_cast<double>(excitation * gain_) * 0.035;
        u_                 = nextU;
        v_                 = nextV;
        return static_cast<float>(v_);
    }

private:
    double cosCoeff_ = 0.99;
    double sinCoeff_ = 0.01;
    double u_        = 0.0;
    double v_        = 0.0;
    float  gain_     = 1.0f;
    bool   active_   = false;
};

class ModalBank
{
public:
    static constexpr size_t kMaxModes = 12;

    void reset() noexcept
    {
        for (auto& m : modes_)
            m.reset();
    }

    void configureBank(
        double                             sampleRate,
        double                             fundamentalHz,
        const std::array<double, kMaxModes>& ratios,
        const std::array<float, kMaxModes>&  amplitudes,
        double                             baseDecaySec,
        double                             highDampingSlope = 0.35,
        size_t                             activeCount      = 8) noexcept
    {
        count_ = std::min(activeCount, kMaxModes);
        for (size_t i = 0; i < count_; ++i)
        {
            const double modeDecay = baseDecaySec / (1.0 + highDampingSlope * static_cast<double>(i));
            modes_[i].configure(sampleRate, fundamentalHz * ratios[i], modeDecay, amplitudes[i]);
            modes_[i].reset();
        }
    }

    void strike(float impulse) noexcept
    {
        for (size_t i = 0; i < count_; ++i)
            modes_[i].strike(impulse);
    }

    float process(float continuousExcitation = 0.0f) noexcept
    {
        float sum = 0.0f;
        for (size_t i = 0; i < count_; ++i)
            sum += modes_[i].process(continuousExcitation);
        return sum;
    }

private:
    std::array<ModalOscillator, kMaxModes> modes_ {};
    size_t                                 count_ = 8;
};

// Circular drumhead model (Bessel inharmonic membrane ratios + strike-position modal weighting + tension bend).
class MembraneModel
{
public:
    void trigger(
        double sampleRate,
        float  fundamentalHz,
        float  decaySec,
        float  strikePosition, // 0.05 = center (sub-heavy), 0.85 = rim (overtone-rich)
        float  tensionBendSemitones,
        float  velocity) noexcept
    {
        sr_          = std::max(8000.0, sampleRate);
        baseHz_      = std::clamp(fundamentalHz, 24.0f, 1200.0f);
        bendRatio_   = std::pow(2.0f, std::clamp(tensionBendSemitones, 0.0f, 14.0f) / 12.0f) - 1.0f;
        bendEnv_     = 1.0f * std::clamp(velocity, 0.2f, 1.0f);
        bendDecay_   = static_cast<float>(std::exp(-1.0 / (0.042 * sr_)));
        fundPhase_   = 0.0;
        fundAmp_     = 1.0f;
        fundDecay_   = static_cast<float>(std::exp(-1.0 / (std::clamp(decaySec, 0.03f, 8.0f) * sr_)));

        // Circular membrane Bessel roots (0,1), (1,1), (2,1), (0,2), (3,1), (1,2), (4,1), (2,2)
        static constexpr std::array<double, ModalBank::kMaxModes> kMembraneRatios {
            1.000, 1.593, 2.136, 2.296, 2.653, 2.917, 3.156, 3.501, 3.600, 3.890, 4.150, 4.420
        };
        const float pos  = std::clamp(strikePosition, 0.05f, 0.92f);
        const float velB = 0.45f + 0.55f * std::clamp(velocity, 0.1f, 1.0f);

        std::array<float, ModalBank::kMaxModes> amps {};
        for (size_t m = 0; m < ModalBank::kMaxModes; ++m)
        {
            const double spatial = std::abs(std::sin(static_cast<double>(m + 1) * kPi * (1.0 - 0.65 * pos)));
            const double rolloff = std::pow(0.74 + 0.16 * velB, static_cast<double>(m));
            amps[m]              = static_cast<float>(spatial * rolloff * 0.24);
        }

        bank_.configureBank(sr_, baseHz_, kMembraneRatios, amps, decaySec * 0.85, 0.42, 8);
        bank_.strike(0.85f * velB);
    }

    float process(float exciterSample) noexcept
    {
        // Coupled nonlinear fundamental pitch bend after impact + upper membrane modal bank
        const double instHz = static_cast<double>(baseHz_ * (1.0f + bendRatio_ * bendEnv_));
        fundPhase_ += instHz / sr_;
        if (fundPhase_ >= 1.0)
            fundPhase_ -= 1.0;

        const float subMode   = static_cast<float>(std::sin(kTwoPi * fundPhase_)) * fundAmp_ * 0.62f;
        const float upperBank = bank_.process(exciterSample);

        bendEnv_ *= bendDecay_;
        fundAmp_ *= fundDecay_;
        return subMode + upperBank;
    }

private:
    double    sr_        = 48000.0;
    float     baseHz_    = 65.0f;
    float     bendRatio_ = 0.2f;
    float     bendEnv_   = 0.0f;
    float     bendDecay_ = 0.999f;
    double    fundPhase_ = 0.0;
    float     fundAmp_   = 0.0f;
    float     fundDecay_ = 0.9995f;
    ModalBank bank_;
};

// Resonant wooden/metal drum shell body coupling.
class ShellResonator
{
public:
    void configure(double sampleRate, float shellSize01, float resonance01, float coupling01) noexcept
    {
        coupling_ = std::clamp(coupling01, 0.0f, 1.0f);
        const double f1 = 290.0 - 175.0 * std::clamp(shellSize01, 0.0f, 1.0f); // large war drum ~115 Hz, snare shell ~230 Hz
        const double q  = 1.2 + 5.5 * std::clamp(resonance01, 0.0f, 1.0f);
        mode1_.setParams(sampleRate, f1, q, TptSvf::Mode::BandPass);
        mode2_.setParams(sampleRate, f1 * 1.84, q * 0.85, TptSvf::Mode::BandPass);
        mode1_.reset();
        mode2_.reset();
    }

    float process(float membraneIn) noexcept
    {
        if (coupling_ <= 0.001f)
            return membraneIn;
        const float shell = mode1_.process(membraneIn) * 0.72f + mode2_.process(membraneIn) * 0.42f;
        return membraneIn + coupling_ * shell;
    }

private:
    TptSvf mode1_, mode2_;
    float  coupling_ = 0.3f;
};

// Inharmonic metallic plates, tubes, bars, gongs, cymbals, and bells.
class MetallicResonator
{
public:
    enum class Material : uint8_t
    {
        Cymbal = 0,
        Gong,
        TubularBell,
        Plate,
        MetalBar
    };

    void trigger(double sampleRate, float baseHz, float decaySec, Material mat, float brightness, float velocity) noexcept
    {
        static constexpr std::array<std::array<double, ModalBank::kMaxModes>, 5> kMaterialRatios {{
            { 1.000, 1.374, 1.792, 2.311, 2.934, 3.671, 4.412, 5.289, 6.170, 7.120, 8.090, 9.140 }, // Cymbal
            { 1.000, 1.214, 1.541, 1.893, 2.387, 2.912, 3.488, 4.190, 4.910, 5.660, 6.480, 7.310 }, // Gong
            { 1.000, 2.000, 2.756, 4.071, 5.404, 6.984, 8.520, 9.810, 11.20, 12.60, 14.10, 15.60 }, // TubularBell
            { 1.000, 1.470, 2.090, 2.560, 3.180, 3.890, 4.620, 5.380, 6.190, 7.040, 7.910, 8.850 }, // Plate
            { 1.000, 2.758, 5.406, 8.933, 13.34, 18.64, 20.00, 22.00, 24.00, 26.00, 28.00, 30.00 }  // MetalBar
        }};

        const size_t idx  = std::min<size_t>(static_cast<size_t>(mat), 4u);
        const float  tilt = std::clamp(0.48f + 0.36f * brightness * velocity, 0.35f, 0.90f);

        std::array<float, ModalBank::kMaxModes> amps {};
        for (size_t i = 0; i < ModalBank::kMaxModes; ++i)
            amps[i] = static_cast<float>(std::pow(tilt, static_cast<double>(i)) * 0.20);

        bank_.configureBank(sampleRate, std::clamp(baseHz, 85.0f, 4800.0f), kMaterialRatios[idx], amps, decaySec, 0.28, 10);
        bank_.strike(0.75f * std::clamp(velocity, 0.15f, 1.0f));
    }

    float process(float exciterSample) noexcept
    {
        return bank_.process(exciterSample);
    }

private:
    ModalBank bank_;
};

// 8 mathematical impact exciters (Soft/Hard Mallet, Wooden/Metal Stick, Felt Beater, Drumstick, Brush, MultiHit).
class ProceduralExciter
{
public:
    enum class Type : uint8_t
    {
        SoftMallet = 0,
        HardMallet,
        WoodenStick,
        MetalStick,
        FeltBeater,
        Drumstick,
        BrushNoise,
        MultiHitBurst
    };

    void trigger(
        double sampleRate,
        Type   type,
        float  hardness01,
        float  velocity01,
        float  clickPitchHz = 1200.0f) noexcept
    {
        sr_        = std::max(8000.0, sampleRate);
        type_      = type;
        hardness_  = std::clamp(hardness01, 0.0f, 1.0f);
        velocity_  = std::clamp(velocity01, 0.1f, 1.0f);
        sampleIdx_ = 0;
        phase_     = 0.0;

        const double baseMs = (type == Type::SoftMallet || type == Type::FeltBeater) ? 9.5
                            : (type == Type::BrushNoise ? 38.0 : (type == Type::MultiHitBurst ? 34.0 : 3.2));
        durationSamples_    = std::max(8, static_cast<int>(sr_ * baseMs * 0.001));
        env_                = 1.0f;
        envDecay_           = static_cast<float>(std::exp(-1.0 / (std::max(1.0, baseMs * 0.45) * 0.001 * sr_)));
        clickHz_            = std::clamp(clickPitchHz * (0.65f + 0.85f * hardness_ * velocity_), 180.0f, 9500.0f);

        const double cutoff = 550.0 + 9500.0 * static_cast<double>(hardness_ * (0.5f + 0.5f * velocity_));
        filter_.setParams(sr_, cutoff, (type == Type::MetalStick) ? 2.4 : 0.9, TptSvf::Mode::BandPass);
        filter_.reset();
    }

    float process(DeterministicRng& rng) noexcept
    {
        if (type_ != Type::MultiHitBurst && sampleIdx_ >= durationSamples_ * 4)
            return 0.0f;

        const double t = static_cast<double>(sampleIdx_) / sr_;
        float        out = 0.0f;

        if (type_ == Type::MultiHitBurst)
        {
            float burst = 0.0f;
            for (int b = 0; b < 4; ++b)
            {
                const double onset = static_cast<double>(b) * 0.0085;
                if (t >= onset)
                    burst += static_cast<float>(std::exp(-(t - onset) / (b == 3 ? 0.018 : 0.0045)));
            }
            if (t > 0.12)
                return 0.0f;
            out = filter_.process(rng.nextSigned() * burst);
        }
        else
        {
            // Half-sine mallet contact pulse + filtered broadband strike noise
            const float contact = (sampleIdx_ < durationSamples_)
                                    ? static_cast<float>(std::sin(kPi * static_cast<double>(sampleIdx_) / static_cast<double>(durationSamples_)))
                                    : 0.0f;
            phase_ += static_cast<double>(clickHz_) / sr_;
            if (phase_ >= 1.0)
                phase_ -= 1.0;
            const float chirp = static_cast<float>(std::sin(kTwoPi * phase_)) * env_;
            const float noise = filter_.process(rng.nextSigned()) * env_;

            switch (type_)
            {
                case Type::SoftMallet:  out = 0.82f * contact + 0.18f * noise; break;
                case Type::FeltBeater:  out = 0.72f * contact + 0.28f * chirp; break;
                case Type::HardMallet:  out = 0.48f * contact + 0.35f * chirp + 0.32f * noise; break;
                case Type::WoodenStick: out = 0.30f * contact + 0.52f * chirp + 0.42f * noise; break;
                case Type::MetalStick:  out = 0.20f * contact + 0.68f * chirp + 0.48f * noise; break;
                case Type::Drumstick:   out = 0.35f * contact + 0.40f * chirp + 0.55f * noise; break;
                case Type::BrushNoise:  out = 0.12f * contact + 0.88f * noise; break;
                case Type::MultiHitBurst: break;
            }
            env_ *= envDecay_;
        }

        ++sampleIdx_;
        return out * velocity_;
    }

private:
    double sr_              = 48000.0;
    Type   type_            = Type::Drumstick;
    float  hardness_        = 0.6f;
    float  velocity_        = 0.9f;
    float  clickHz_         = 1200.0f;
    float  env_             = 0.0f;
    float  envDecay_        = 0.99f;
    double phase_           = 0.0;
    int    sampleIdx_       = 0;
    int    durationSamples_ = 64;
    TptSvf filter_;
};

} // namespace disklordz::dsp
