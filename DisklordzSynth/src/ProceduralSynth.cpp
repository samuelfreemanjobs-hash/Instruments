#include "disklordz/ProceduralSynth.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace disklordz::synth
{

namespace
{
constexpr double kPi = 3.14159265358979323846;
constexpr std::size_t kWaveTableSize = 2048;

float clamp01 (float v)
{
    return std::max (0.0f, std::min (1.0f, v));
}

void buildWaveTableFixed (float position, float tilt, float* table, std::size_t n)
{
    position = clamp01 (position);
    const int maxHarm = static_cast<int> (2.0f + position * 48.0f);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (n);
        double sum = 0.0;
        for (int h = 1; h <= maxHarm; ++h)
        {
            const double roll = 1.0 / static_cast<double> (h * h);
            const double tiltGain = 1.0 / (1.0 + tilt * static_cast<double> (h));
            sum += roll * tiltGain * std::sin (2.0 * kPi * phase * static_cast<double> (h));
        }
        table[i] = static_cast<float> (sum);
    }
    std::vector<float> tmp (table, table + n);
    normalizePeak (tmp, 0.95f);
    for (std::size_t i = 0; i < n; ++i)
        table[i] = tmp[i];
}

float sampleTable (const float* table, std::size_t n, double phase01)
{
    phase01 -= std::floor (phase01);
    const double pos = phase01 * static_cast<double> (n);
    const auto i0 = static_cast<std::size_t> (pos) % n;
    const auto i1 = (i0 + 1) % n;
    const float frac = static_cast<float> (pos - std::floor (pos));
    return table[i0] + (table[i1] - table[i0]) * frac;
}

} // namespace

void normalizePeak (std::vector<float>& buffer, float targetPeak)
{
    float peak = 0.0f;
    for (float s : buffer)
        peak = std::max (peak, std::abs (s));
    if (peak > 1.0e-6f)
    {
        const float g = targetPeak / peak;
        for (float& s : buffer)
            s *= g;
    }
}

std::vector<float> renderSubtractive (std::size_t frames, const EngineParams& p)
{
    const float cutoff = 0.05f + clamp01 (p.a) * 0.45f;
    const float res = clamp01 (p.b) * 0.85f;
    const bool pulse = p.c > 0.5f;
    const float decay = 0.5f + clamp01 (p.d) * 4.0f;

    std::vector<float> out (frames, 0.0f);
    float lp = 0.0f;
    float bp = 0.0f;
    double phase = 0.0;
    const double phaseInc = 2.0 / static_cast<double> (frames);

    for (std::size_t i = 0; i < frames; ++i)
    {
        phase += phaseInc;
        if (phase > 1.0)
            phase -= 1.0;

        float osc = pulse ? (phase < 0.2 ? 1.0f : -0.2f) : static_cast<float> (2.0 * phase - 1.0);
        const float t = static_cast<float> (i) / static_cast<float> (frames);
        const float env = std::exp (-decay * t * 3.0f);

        lp += cutoff * (osc - lp);
        bp += cutoff * (lp - bp);
        const float filtered = lp + res * (lp - bp);

        out[i] = filtered * env;
    }
    normalizePeak (out, 0.92f);
    return out;
}

std::vector<float> renderAdditive (std::size_t frames, const EngineParams& p)
{
    const float brightness = clamp01 (p.a);
    const float inharm = clamp01 (p.b) * 0.6f;
    const float decay = 1.0f + clamp01 (p.c) * 8.0f;
    const int maxHarm = static_cast<int> (2.0f + brightness * 40.0f);

    std::vector<float> out (frames, 0.0f);
    for (std::size_t i = 0; i < frames; ++i)
    {
        const double t = static_cast<double> (i) / static_cast<double> (frames);
        double sum = 0.0;
        for (int h = 1; h <= maxHarm; ++h)
        {
            const double f = static_cast<double> (h) * (1.0 + inharm * 0.02 * static_cast<double> (h));
            sum += (1.0 / static_cast<double> (h * h))
                   * std::sin (2.0 * kPi * f * t)
                   * std::exp (-decay * t * (1.0 + 0.15 * static_cast<double> (h)));
        }
        out[i] = static_cast<float> (sum);
    }
    normalizePeak (out);
    return out;
}

std::vector<float> renderKarplus (std::size_t frames, const EngineParams& p, bool deterministicNoise)
{
    if (deterministicNoise)
        std::srand (9091);

    const float decay = 0.985f + clamp01 (p.a) * 0.014f;
    const float brightness = clamp01 (p.b);
    const float pluckRatio = 0.05f + clamp01 (p.d) * 0.2f;

    std::vector<float> out (frames, 0.0f);
    const std::size_t pluckLen = std::max<std::size_t> (8, static_cast<std::size_t> (frames * pluckRatio));
    for (std::size_t i = 0; i < pluckLen; ++i)
        out[i] = (static_cast<float> (std::rand()) / RAND_MAX) * 2.0f - 1.0f;

    for (std::size_t i = 1; i < frames; ++i)
    {
        const float prev = out[i - 1];
        const float n = out[i > 1 ? i - 2 : frames - 1];
        out[i] = (0.5f * (prev + n)) * decay;
    }

    for (std::size_t i = 0; i < frames; ++i)
        out[i] += brightness * 0.12f * std::sin (2.0f * static_cast<float> (kPi) * static_cast<float> (i) / 64.0f);

    normalizePeak (out, 0.88f);
    return out;
}

std::vector<float> renderWave (std::size_t frames, const EngineParams& p)
{
    float table[kWaveTableSize];
    buildWaveTableFixed (p.a, clamp01 (p.b) * 2.0f, table, kWaveTableSize);

    const bool loop = p.c > 0.5f;
    const float decay = 0.5f + clamp01 (p.d) * 5.0f;

    std::vector<float> out (frames, 0.0f);
    double phase = 0.0;
    const double phaseInc = loop ? 0.015 : 1.0 / static_cast<double> (frames);

    for (std::size_t i = 0; i < frames; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (frames);
        const float env = loop ? 1.0f : std::exp (-decay * t * 2.5f);
        out[i] = sampleTable (table, kWaveTableSize, phase) * env;
        phase += phaseInc;
    }
    normalizePeak (out, loop ? 0.75f : 0.9f);
    return out;
}

std::vector<float> renderEngine (EngineId id, std::size_t frames, const EngineParams& p)
{
    switch (id)
    {
        case EngineId::subtractive: return renderSubtractive (frames, p);
        case EngineId::additive: return renderAdditive (frames, p);
        case EngineId::karplus: return renderKarplus (frames, p, true);
        case EngineId::wave: return renderWave (frames, p);
    }
    return {};
}

std::vector<float> renderEngine (EngineId id, std::size_t frames, float paramA, float paramB)
{
    EngineParams p;
    p.a = paramA;
    p.b = paramB;
    return renderEngine (id, frames, p);
}

} // namespace disklordz::synth
