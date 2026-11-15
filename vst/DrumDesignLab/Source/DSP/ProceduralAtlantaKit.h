#pragma once
// DiskLordz Procedural Atlanta Kit — portable C++20 (no JUCE).
// Bakes 8 Trap Muzik pads once in prepare(); RT path only plays the buffers.
#include "CoreMathAndPRNG.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz::toomp
{

struct AtlantaPadBuffer
{
    std::vector<float> interleaved; // LRLR…
    int numFrames = 0;

    void clearAndResize(int frames) noexcept
    {
        numFrames = std::max(0, frames);
        interleaved.assign(static_cast<size_t>(numFrames) * 2u, 0.0f);
    }

    float sampleL(int i) const noexcept
    {
        if (i < 0 || i >= numFrames)
            return 0.0f;
        return interleaved[static_cast<size_t>(i) * 2u];
    }

    float sampleR(int i) const noexcept
    {
        if (i < 0 || i >= numFrames)
            return 0.0f;
        return interleaved[static_cast<size_t>(i) * 2u + 1u];
    }
};

enum class AtlantaPad : uint8_t
{
    Sub808 = 0,
    PunchKick = 1,
    TrapSnare = 2,
    LayeredClap = 3,
    ClosedHat = 4,
    OpenHat = 5,
    RimPerc = 6,
    Crash = 7,
    Count = 8
};

class ProceduralAtlantaKit
{
public:
    static constexpr int kPadCount = static_cast<int>(AtlantaPad::Count);

    /** Off-audio-thread / prepare(): synthesize all factory pads into padBuffers. */
    static void populateDefaultPads(std::array<AtlantaPadBuffer, kPadCount>& padBuffers,
                                    double sampleRate) noexcept
    {
        const double sr = sampleRate > 0.0 ? sampleRate : 44100.0;
        synthesize808Sub(padBuffers[0], sr);
        synthesizePunchKick(padBuffers[1], sr);
        synthesizeTrapSnare(padBuffers[2], sr);
        synthesizeLayeredClap(padBuffers[3], sr);
        synthesizeHiHat(padBuffers[4], sr, 0.065);
        synthesizeHiHat(padBuffers[5], sr, 0.380);
        synthesizeRimPerc(padBuffers[6], sr);
        synthesizeCrash(padBuffers[7], sr);
    }

    static int midiToPad(int midiNote) noexcept
    {
        switch (midiNote)
        {
        case 36: return 0;
        case 37: return 1;
        case 38:
        case 40: return 2;
        case 39: return 3;
        case 42:
        case 44: return 4;
        case 46: return 5;
        case 47: return 6;
        case 49:
        case 51: return 7;
        default: return -1;
        }
    }

private:
    static void writeStereo(AtlantaPadBuffer& buf, int i, float mono) noexcept
    {
        if (i < 0 || i >= buf.numFrames)
            return;
        buf.interleaved[static_cast<size_t>(i) * 2u] = mono;
        buf.interleaved[static_cast<size_t>(i) * 2u + 1u] = mono;
    }

    static void writeStereoLR(AtlantaPadBuffer& buf, int i, float l, float r) noexcept
    {
        if (i < 0 || i >= buf.numFrames)
            return;
        buf.interleaved[static_cast<size_t>(i) * 2u] = l;
        buf.interleaved[static_cast<size_t>(i) * 2u + 1u] = r;
    }

    // Pad 0: Deep TR-808 style sine sub with click + 2nd harmonic
    static void synthesize808Sub(AtlantaPadBuffer& buf, double sr) noexcept
    {
        const int length = static_cast<int>(sr * 2.2);
        buf.clearAndResize(length);
        double phase = 0.0;
        for (int i = 0; i < length; ++i)
        {
            const double t = static_cast<double>(i) / sr;
            const double freq = 46.25 + 115.0 * std::exp(-t / 0.028);
            const double ampEnv = std::exp(-t / 0.85) * std::min(1.0, t / 0.002);
            phase += (dsp::kTwoPi * freq) / sr;
            const double raw = std::sin(phase) + 0.14 * std::sin(2.0 * phase);
            writeStereo(buf, i, static_cast<float>(std::tanh(raw * 1.35) * ampEnv * 0.92));
        }
    }

    // Pad 1: Short chest-thumping top kick
    static void synthesizePunchKick(AtlantaPadBuffer& buf, double sr) noexcept
    {
        const int length = static_cast<int>(sr * 0.35);
        buf.clearAndResize(length);
        double phase = 0.0;
        for (int i = 0; i < length; ++i)
        {
            const double t = static_cast<double>(i) / sr;
            const double freq = 52.0 + 210.0 * std::exp(-t / 0.018);
            const double ampEnv = std::exp(-t / 0.085) * std::min(1.0, t / 0.001);
            phase += (dsp::kTwoPi * freq) / sr;
            writeStereo(buf, i,
                        static_cast<float>(std::tanh(std::sin(phase) * 1.8) * ampEnv * 0.90));
        }
    }

    // Pad 2: Dual-osc shell + high-passed snare wire
    static void synthesizeTrapSnare(AtlantaPadBuffer& buf, double sr) noexcept
    {
        const int length = static_cast<int>(sr * 0.42);
        buf.clearAndResize(length);
        dsp::DeterministicRng rng(1337ULL, 2ULL);
        double phase1 = 0.0, phase2 = 0.0;
        float prevNoise = 0.0f;
        for (int i = 0; i < length; ++i)
        {
            const double t = static_cast<double>(i) / sr;
            const double f1 = 185.0 + 140.0 * std::exp(-t / 0.014);
            const double f2 = 312.0 + 180.0 * std::exp(-t / 0.012);
            phase1 += (dsp::kTwoPi * f1) / sr;
            phase2 += (dsp::kTwoPi * f2) / sr;
            const double bodyEnv = std::exp(-t / 0.055);
            const double body = (std::sin(phase1) + 0.65 * std::sin(phase2)) * bodyEnv * 0.55;

            const float white = rng.nextSigned();
            const float hpNoise = white - 0.78f * prevNoise;
            prevNoise = white;
            const double wireEnv = std::exp(-t / 0.115);
            const double wires = static_cast<double>(hpNoise) * wireEnv * 0.65;
            writeStereo(buf, i, static_cast<float>(std::tanh((body + wires) * 1.4) * 0.88));
        }
    }

    // Pad 3: 3-burst + main tail TR-808 style clap
    static void synthesizeLayeredClap(AtlantaPadBuffer& buf, double sr) noexcept
    {
        const int length = static_cast<int>(sr * 0.45);
        buf.clearAndResize(length);
        dsp::DeterministicRng rng(4242ULL, 3ULL);
        float lp = 0.0f;
        float prev = 0.0f;
        for (int i = 0; i < length; ++i)
        {
            const double t = static_cast<double>(i) / sr;
            const float white = rng.nextSigned();
            const float hp = white - 0.85f * prev;
            prev = white;
            lp += 0.28f * (hp - lp);

            double env = 0.0;
            for (int b = 0; b < 3; ++b)
            {
                const double bt = t - (b * 0.011);
                if (bt >= 0.0)
                    env += 0.55 * std::exp(-bt / 0.008);
            }
            if (t >= 0.033)
                env += 0.85 * std::exp(-(t - 0.033) / 0.125);

            writeStereo(buf, i, static_cast<float>(std::tanh(static_cast<double>(lp) * env * 2.2) * 0.85));
        }
    }

    // Pads 4 & 5: 6-osc metallic square cluster
    static void synthesizeHiHat(AtlantaPadBuffer& buf, double sr, double decaySec) noexcept
    {
        const int length = static_cast<int>(sr * (decaySec * 5.0));
        buf.clearAndResize(std::max(length, 64));
        constexpr std::array<double, 6> ratios { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
        std::array<double, 6> phases {};
        float prev = 0.0f;
        for (int i = 0; i < buf.numFrames; ++i)
        {
            const double t = static_cast<double>(i) / sr;
            double metallic = 0.0;
            for (size_t k = 0; k < ratios.size(); ++k)
            {
                phases[k] += (dsp::kTwoPi * ratios[k]) / sr;
                metallic += (std::sin(phases[k]) >= 0.0 ? 1.0 : -1.0);
            }
            metallic /= 6.0;
            const float hp = static_cast<float>(metallic) - 0.92f * prev;
            prev = static_cast<float>(metallic);
            const double env = std::exp(-t / decaySec) * std::min(1.0, t / 0.0008);
            writeStereo(buf, i, static_cast<float>(hp * env * 0.72));
        }
    }

    // Pad 6: Rim / woodblock click
    static void synthesizeRimPerc(AtlantaPadBuffer& buf, double sr) noexcept
    {
        const int length = static_cast<int>(sr * 0.18);
        buf.clearAndResize(length);
        double p1 = 0.0, p2 = 0.0;
        for (int i = 0; i < length; ++i)
        {
            const double t = static_cast<double>(i) / sr;
            p1 += (dsp::kTwoPi * 445.0) / sr;
            p2 += (dsp::kTwoPi * 1680.0) / sr;
            const double env = std::exp(-t / 0.022);
            writeStereo(buf, i,
                        static_cast<float>(
                            std::tanh((std::sin(p1) + 0.7 * std::sin(p2)) * 1.6) * env * 0.80));
        }
    }

    // Pad 7: Long shimmer crash (+ slight stereo)
    static void synthesizeCrash(AtlantaPadBuffer& buf, double sr) noexcept
    {
        const int length = static_cast<int>(sr * 1.8);
        buf.clearAndResize(length);
        dsp::DeterministicRng rngL(9090ULL, 7ULL);
        dsp::DeterministicRng rngR(9091ULL, 8ULL);
        float prevL = 0.0f, prevR = 0.0f;
        for (int i = 0; i < length; ++i)
        {
            const double t = static_cast<double>(i) / sr;
            const float wL = rngL.nextSigned();
            const float wR = rngR.nextSigned();
            const float hpL = wL - 0.88f * prevL;
            const float hpR = wR - 0.88f * prevR;
            prevL = wL;
            prevR = wR;
            const double env = std::exp(-t / 0.52) * std::min(1.0, t / 0.003);
            writeStereoLR(buf, i, static_cast<float>(hpL * env * 0.65),
                          static_cast<float>(hpR * env * 0.65));
        }
    }
};

} // namespace disklordz::toomp
