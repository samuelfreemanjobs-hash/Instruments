#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace disklordz::dsp
{

inline constexpr double kPi    = std::numbers::pi_v<double>;
inline constexpr double kTwoPi = 2.0 * std::numbers::pi_v<double>;

struct StereoSample
{
    float left  = 0.0f;
    float right = 0.0f;

    constexpr StereoSample& operator+=(const StereoSample& rhs) noexcept
    {
        left += rhs.left;
        right += rhs.right;
        return *this;
    }

    constexpr StereoSample operator*(float scalar) const noexcept
    {
        return { left * scalar, right * scalar };
    }
};

// Deterministic zero-allocation PRNG with independent layer-namespace mixing.
class DeterministicRng
{
public:
    constexpr explicit DeterministicRng(uint64_t seed = 0xD15C108DULL, uint64_t streamNamespace = 1ULL) noexcept
    {
        reseed(seed, streamNamespace);
    }

    constexpr void reseed(uint64_t seed, uint64_t streamNamespace = 1ULL) noexcept
    {
        uint64_t z = (seed ^ (streamNamespace * 0x9E3779B97F4A7C15ULL)) + 0x9E3779B97F4A7C15ULL;
        for (auto& word : state_)
        {
            z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
            z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
            word = static_cast<uint32_t>(z ^ (z >> 31));
            if (word == 0u)
                word = 0x6D2B79F5u;
            z += 0x9E3779B97F4A7C15ULL;
        }
    }

    constexpr uint32_t nextU32() noexcept
    {
        const uint32_t result = rotl(state_[0] + state_[3], 7) + state_[0];
        const uint32_t t      = state_[1] << 9;
        state_[2] ^= state_[0];
        state_[3] ^= state_[1];
        state_[1] ^= state_[2];
        state_[0] ^= state_[3];
        state_[2] ^= t;
        state_[3] = rotl(state_[3], 11);
        return result;
    }

    // Uniform float in [-1.0f, +1.0f]
    float nextSigned() noexcept
    {
        return static_cast<float>(static_cast<double>(nextU32()) * (2.0 / 4294967295.0) - 1.0);
    }

    // Uniform float in [0.0f, 1.0f]
    float nextUnit() noexcept
    {
        return static_cast<float>(static_cast<double>(nextU32()) * (1.0 / 4294967295.0));
    }

    float nextRange(float lo, float hi) noexcept
    {
        return lo + (hi - lo) * nextUnit();
    }

private:
    static constexpr uint32_t rotl(uint32_t x, int k) noexcept
    {
        return (x << k) | (x >> (32 - k));
    }

    std::array<uint32_t, 4> state_ { 1u, 2u, 3u, 4u };
};

inline double polyBlep(double phase, double dt) noexcept
{
    if (dt <= 0.0)
        return 0.0;
    if (phase < dt)
    {
        const double t = phase / dt;
        return t + t - t * t - 1.0;
    }
    if (phase > 1.0 - dt)
    {
        const double t = (phase - 1.0) / dt;
        return t * t + t + t + 1.0;
    }
    return 0.0;
}

inline float blepSaw(double phase, double dt) noexcept
{
    return static_cast<float>((2.0 * phase - 1.0) - polyBlep(phase, dt));
}

inline float blepPulse(double phase, double dt, double width = 0.5) noexcept
{
    const double w   = std::clamp(width, 0.05, 0.95);
    double       val = (phase < w) ? 1.0 : -1.0;
    val += polyBlep(phase, dt);
    double shifted = phase - w;
    if (shifted < 0.0)
        shifted += 1.0;
    val -= polyBlep(shifted, dt);
    return static_cast<float>(val);
}

// Unconditionally stable Zero-Delay-Feedback Topology-Preserving State-Variable Filter.
class TptSvf
{
public:
    enum class Mode : uint8_t
    {
        LowPass,
        BandPass,
        HighPass
    };

    void reset() noexcept
    {
        ic1eq_ = 0.0;
        ic2eq_ = 0.0;
    }

    void setParams(double sampleRate, double cutoffHz, double q = 0.707, Mode mode = Mode::LowPass) noexcept
    {
        const double sr      = std::max(8000.0, sampleRate);
        const double clamped = std::clamp(cutoffHz, 16.0, sr * 0.46);
        const double safeQ   = std::clamp(q, 0.25, 14.0);
        g_                   = std::tan(kPi * clamped / sr);
        k_                   = 1.0 / safeQ;
        a1_                  = 1.0 / (1.0 + g_ * (g_ + k_));
        a2_                  = g_ * a1_;
        a3_                  = g_ * a2_;
        mode_                = mode;
    }

    float process(float input) noexcept
    {
        const double v3 = static_cast<double>(input) - ic2eq_;
        const double v1 = a1_ * ic1eq_ + a2_ * v3;
        const double v2 = ic2eq_ + a2_ * ic1eq_ + a3_ * v3;
        ic1eq_          = 2.0 * v1 - ic1eq_;
        ic2eq_          = 2.0 * v2 - ic2eq_;

        switch (mode_)
        {
            case Mode::LowPass:  return static_cast<float>(v2);
            case Mode::BandPass: return static_cast<float>(v1);
            case Mode::HighPass: return static_cast<float>(static_cast<double>(input) - k_ * v1 - v2);
        }
        return static_cast<float>(v2);
    }

private:
    double ic1eq_ = 0.0;
    double ic2eq_ = 0.0;
    double g_     = 0.1;
    double k_     = 1.414;
    double a1_    = 0.5;
    double a2_    = 0.1;
    double a3_    = 0.01;
    Mode   mode_  = Mode::LowPass;
};

class OnePoleDcBlocker
{
public:
    void prepare(double sampleRate, double cutoffHz = 20.0) noexcept
    {
        const double sr = std::max(8000.0, sampleRate);
        pole_           = std::clamp(1.0 - (kTwoPi * cutoffHz / sr), 0.96, 0.9995);
        reset();
    }

    void reset() noexcept
    {
        prevIn_  = 0.0;
        prevOut_ = 0.0;
    }

    float process(float x) noexcept
    {
        const double in  = static_cast<double>(x);
        const double out = in - prevIn_ + pole_ * prevOut_;
        prevIn_          = in;
        prevOut_         = out;
        return static_cast<float>(out);
    }

private:
    double pole_    = 0.997;
    double prevIn_  = 0.0;
    double prevOut_ = 0.0;
};

// Zero-allocation 4x oversampled nonlinear waveshaper (Soft, Asymmetric, Wavefolder).
class OversampledWaveshaper4x
{
public:
    enum class Curve : uint8_t
    {
        SoftTanh,
        AsymmetricClip,
        Wavefold
    };

    void prepare(double sampleRate) noexcept
    {
        const double osRate = std::max(8000.0, sampleRate) * 4.0;
        const double stopHz = std::max(8000.0, sampleRate) * 0.44;
        upFilter1_.setParams(osRate, stopHz, 0.707, TptSvf::Mode::LowPass);
        upFilter2_.setParams(osRate, stopHz, 0.707, TptSvf::Mode::LowPass);
        downFilter1_.setParams(osRate, stopHz, 0.707, TptSvf::Mode::LowPass);
        downFilter2_.setParams(osRate, stopHz, 0.707, TptSvf::Mode::LowPass);
        dc_.prepare(sampleRate, 22.0);
        reset();
    }

    void reset() noexcept
    {
        upFilter1_.reset();
        upFilter2_.reset();
        downFilter1_.reset();
        downFilter2_.reset();
        dc_.reset();
        prevSample_ = 0.0f;
    }

    float process(float input, float drive, Curve curve = Curve::AsymmetricClip, float foldAmount = 0.0f) noexcept
    {
        const float safeDrive = std::clamp(drive, 0.5f, 12.0f);
        const float norm      = std::max(0.35f, std::tanh(safeDrive));
        float       decimated = 0.0f;

        // 4x linear-seeded polyphase upsample + 4th-order anti-imaging SVF -> nonlinear -> 4th-order anti-aliasing SVF
        for (int k = 0; k < 4; ++k)
        {
            const float frac = static_cast<float>(k + 1) * 0.25f;
            float       up   = prevSample_ + (input - prevSample_) * frac;
            up               = upFilter2_.process(upFilter1_.process(up));

            const float d = up * safeDrive;
            float       y = 0.0f;
            if (curve == Curve::SoftTanh)
            {
                y = std::tanh(d) / norm;
            }
            else if (curve == Curve::AsymmetricClip)
            {
                y = (d >= 0.0f) ? std::tanh(d * 1.18f) : (std::tanh(d * 0.86f) * 0.92f);
                y /= norm;
            }
            else
            {
                const float asym   = (d >= 0.0f) ? std::tanh(d * 1.15f) : (std::tanh(d * 0.88f) * 0.94f);
                const float folded = std::sin(d * (1.0f + 1.5f * std::clamp(foldAmount, 0.0f, 1.0f)));
                y                  = ((1.0f - foldAmount) * asym + foldAmount * folded) / norm;
            }

            const float filtered = downFilter2_.process(downFilter1_.process(y));
            if (k == 3)
                decimated = filtered;
        }
        prevSample_ = input;
        return dc_.process(decimated);
    }

private:
    TptSvf           upFilter1_, upFilter2_, downFilter1_, downFilter2_;
    OnePoleDcBlocker dc_;
    float            prevSample_ = 0.0f;
};

// Bounded, real-time-safe stereo early-reflections + damped Schroeder room/hall/plate reverb.
class RoomReverb
{
public:
    enum class Preset : uint8_t
    {
        DryStudio = 0,
        SmallRoom,
        ConcertHall,
        LargeHall,
        DarkChamber,
        Arena,
        CinematicPlate
    };

    void prepare(double sampleRate) noexcept
    {
        sr_ = std::clamp(sampleRate, 8000.0, 192000.0);
        const std::array<double, 4> leftMs  { 23.3, 29.7, 37.1, 43.7 };
        const std::array<double, 4> rightMs { 24.7, 31.1, 38.9, 45.3 };
        for (size_t i = 0; i < 4; ++i)
        {
            combLenL_[i] = std::clamp(static_cast<int>(sr_ * leftMs[i] * 0.001), 16, static_cast<int>(kMaxComb - 1));
            combLenR_[i] = std::clamp(static_cast<int>(sr_ * rightMs[i] * 0.001), 16, static_cast<int>(kMaxComb - 1));
        }
        apLen1_ = std::clamp(static_cast<int>(sr_ * 0.0050), 8, static_cast<int>(kMaxAllpass - 1));
        apLen2_ = std::clamp(static_cast<int>(sr_ * 0.0017), 8, static_cast<int>(kMaxAllpass - 1));
        erTap1_ = std::clamp(static_cast<int>(sr_ * 0.0075), 4, static_cast<int>(kMaxPredelay - 1));
        erTap2_ = std::clamp(static_cast<int>(sr_ * 0.0142), 8, static_cast<int>(kMaxPredelay - 1));
        lowCutL_.setParams(sr_, 125.0, 0.707, TptSvf::Mode::HighPass);
        lowCutR_.setParams(sr_, 125.0, 0.707, TptSvf::Mode::HighPass);
        reset();
    }

    void reset() noexcept
    {
        for (auto& b : combL_) b.fill(0.0f);
        for (auto& b : combR_) b.fill(0.0f);
        combPosL_.fill(0);
        combPosR_.fill(0);
        combLpL_.fill(0.0f);
        combLpR_.fill(0.0f);
        ap1L_.fill(0.0f); ap1R_.fill(0.0f);
        ap2L_.fill(0.0f); ap2R_.fill(0.0f);
        preBuf_.fill(0.0f);
        apPos1_ = apPos2_ = prePos_ = 0;
        lowCutL_.reset();
        lowCutR_.reset();
    }

    StereoSample process(StereoSample in, float wetMix, float roomSize = 0.45f, float damping = 0.42f) noexcept
    {
        const float wet = std::clamp(wetMix, 0.0f, 1.0f);
        if (wet <= 0.0005f)
            return in;

        const float fb   = std::clamp(0.45f + 0.41f * std::clamp(roomSize, 0.0f, 1.0f), 0.25f, 0.86f);
        const float damp = std::clamp(damping, 0.08f, 0.78f);
        const float mono = 0.5f * (in.left + in.right);

        preBuf_[static_cast<size_t>(prePos_)] = mono;
        const float er1 = preBuf_[static_cast<size_t>((prePos_ + kMaxPredelay - erTap1_) % kMaxPredelay)];
        const float er2 = preBuf_[static_cast<size_t>((prePos_ + kMaxPredelay - erTap2_) % kMaxPredelay)];
        prePos_         = (prePos_ + 1) % static_cast<int>(kMaxPredelay);

        float sumL = 0.22f * er1;
        float sumR = 0.22f * er2;

        for (size_t i = 0; i < 4; ++i)
        {
            const float dl = combL_[i][static_cast<size_t>(combPosL_[i])];
            combLpL_[i]    = dl * (1.0f - damp) + combLpL_[i] * damp;
            combL_[i][static_cast<size_t>(combPosL_[i])] = mono + combLpL_[i] * fb;
            combPosL_[i]   = (combPosL_[i] + 1) % combLenL_[i];
            sumL += dl * 0.25f;

            const float dr = combR_[i][static_cast<size_t>(combPosR_[i])];
            combLpR_[i]    = dr * (1.0f - damp) + combLpR_[i] * damp;
            combR_[i][static_cast<size_t>(combPosR_[i])] = mono + combLpR_[i] * fb;
            combPosR_[i]   = (combPosR_[i] + 1) % combLenR_[i];
            sumR += dr * 0.25f;
        }

        sumL = stepAllpass(ap1L_, apPos1_, apLen1_, sumL, 0.5f, false);
        sumR = stepAllpass(ap1R_, apPos1_, apLen1_, sumR, 0.5f, true);
        sumL = stepAllpass(ap2L_, apPos2_, apLen2_, sumL, 0.5f, false);
        sumR = stepAllpass(ap2R_, apPos2_, apLen2_, sumR, 0.5f, true);

        const float wetL = lowCutL_.process(sumL);
        const float wetR = lowCutR_.process(sumR);
        return {
            in.left * (1.0f - wet * 0.35f) + wetL * wet,
            in.right * (1.0f - wet * 0.35f) + wetR * wet
        };
    }

private:
    template <size_t N>
    static float stepAllpass(std::array<float, N>& buf, int& pos, int len, float x, float g, bool advance) noexcept
    {
        const float delayed = buf[static_cast<size_t>(pos)];
        const float v       = x + g * delayed;
        buf[static_cast<size_t>(pos)] = v;
        if (advance)
            pos = (pos + 1) % len;
        return delayed - g * v;
    }

    static constexpr size_t kMaxComb     = 9600;
    static constexpr size_t kMaxAllpass  = 1200;
    static constexpr size_t kMaxPredelay = 4096;

    double                                    sr_ = 48000.0;
    std::array<std::array<float, kMaxComb>, 4> combL_ {}, combR_ {};
    std::array<int, 4>                        combLenL_ { 1024, 1300, 1600, 1900 };
    std::array<int, 4>                        combLenR_ { 1080, 1360, 1680, 1980 };
    std::array<int, 4>                        combPosL_ {}, combPosR_ {};
    std::array<float, 4>                      combLpL_ {}, combLpR_ {};
    std::array<float, kMaxAllpass>            ap1L_ {}, ap1R_ {}, ap2L_ {}, ap2R_ {};
    std::array<float, kMaxPredelay>           preBuf_ {};
    int                                       apLen1_ = 240, apLen2_ = 82, erTap1_ = 360, erTap2_ = 680;
    int                                       apPos1_ = 0, apPos2_ = 0, prePos_ = 0;
    TptSvf                                    lowCutL_, lowCutR_;
};

} // namespace disklordz::dsp
