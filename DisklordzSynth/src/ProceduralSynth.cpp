#include "disklordz/ProceduralSynth.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace disklordz::synth
{

namespace
{
constexpr double kPi = 3.14159265358979323846;
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

std::vector<float> renderBell (std::size_t frames, float inharmonicity, float decay)
{
    std::vector<float> out (frames, 0.0f);
    const double partials[] = { 1.0, 2.12, 3.25, 4.55, 5.9 };
    const double gains[] = { 1.0, 0.5, 0.32, 0.2, 0.12 };
    for (std::size_t i = 0; i < frames; ++i)
    {
        const double t = static_cast<double> (i) / static_cast<double> (frames);
        double sum = 0.0;
        for (int p = 0; p < 5; ++p)
        {
            const double f = partials[p] * (1.0 + inharmonicity * 0.015 * static_cast<double> (p));
            sum += gains[p] * std::sin (2.0 * kPi * f * t) * std::exp (-decay * t * (1.0 + 0.25 * p));
        }
        out[i] = static_cast<float> (sum);
    }
    normalizePeak (out);
    return out;
}

std::vector<float> renderKeys (std::size_t frames, float decay, float brightness)
{
    std::vector<float> out (frames, 0.0f);
    for (std::size_t i = 0; i < frames / 10; ++i)
        out[i] = (static_cast<float> (std::rand()) / RAND_MAX) * 2.0f - 1.0f;
    for (std::size_t i = 1; i < frames; ++i)
    {
        const float prev = out[i - 1];
        const float n = out[i > 1 ? i - 2 : frames - 1];
        out[i] = (0.5f * (prev + n)) * decay;
    }
    for (std::size_t i = 0; i < frames; ++i)
        out[i] += brightness * 0.15f * std::sin (2.0f * static_cast<float> (kPi) * static_cast<float> (i) / 48.0f);
    normalizePeak (out, 0.88f);
    return out;
}

std::vector<float> renderPad (std::size_t frames, float lpCoeff, bool deterministicSeed)
{
    if (deterministicSeed)
        std::srand (4242);

    std::vector<float> out (frames, 0.0f);
    float lp = 0.0f;
    for (std::size_t i = 0; i < frames; ++i)
    {
        const float white = (static_cast<float> (std::rand()) / RAND_MAX) * 2.0f - 1.0f;
        lp += lpCoeff * (white - lp);
        out[i] = lp;
    }
    normalizePeak (out, 0.7f);
    return out;
}

std::vector<float> renderSub808 (std::size_t frames, float punch)
{
    std::vector<float> out (frames, 0.0f);
    for (std::size_t i = 0; i < frames; ++i)
    {
        const double t = static_cast<double> (i) / static_cast<double> (frames);
        const double env = std::exp (-3.0 * t) * (1.0 - std::exp (-punch * 40.0 * t));
        const double s = std::sin (2.0 * kPi * t * 2.0) * env;
        out[i] = static_cast<float> (s);
    }
    normalizePeak (out, 0.95f);
    return out;
}

const char* engineIdName (EngineId id)
{
    switch (id)
    {
        case EngineId::bell: return "bell";
        case EngineId::keys: return "keys";
        case EngineId::pad: return "pad";
        case EngineId::sub808: return "sub808";
    }
    return "unknown";
}

std::vector<float> renderEngine (EngineId id, std::size_t frames, float paramA, float paramB)
{
    switch (id)
    {
        case EngineId::bell: return renderBell (frames, paramA, paramB);
        case EngineId::keys: return renderKeys (frames, paramA, paramB);
        case EngineId::pad: return renderPad (frames, paramA, true);
        case EngineId::sub808: return renderSub808 (frames, paramA);
    }
    return {};
}

} // namespace disklordz::synth
