#include "DrumRender.h"

#include <cmath>

namespace trapforge
{
namespace
{

float adsr (float t, float a, float d, float s, float r, float gate)
{
    if (t < a) return a > 0.0f ? t / a : 1.0f;
    if (t < a + d) return 1.0f - (1.0f - s) * ((t - a) / d);
    if (t < gate) return s;
    if (t < gate + r) return r > 0.0f ? s * (1.0f - (t - gate) / r) : 0.0f;
    return 0.0f;
}

std::vector<float> renderLayeredKick (const TrapForgePreset& p, double sr, float velocity)
{
    const float dur = juce::jmax (0.15f, p.ampDecay + p.ampRelease + 0.05f);
    const int n = static_cast<int> (std::ceil (dur * sr));
    std::vector<float> mono (static_cast<std::size_t> (n), 0.0f);
    float phase = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        const float t = static_cast<float> (i / sr);
        const float ae = adsr (t, p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease, dur * 0.95f);
        const float beater =
            t < 0.006f ? std::exp (-t / 0.00075f) * std::sin (2.0f * juce::MathConstants<float>::pi * 3200.0f * t) * ae : 0.0f;
        const float pitchEnv = std::exp (-t / p.pitchDecay);
        const float f0 = p.rootHz * std::pow (2.0f, (p.pitchMod * pitchEnv) / 12.0f);
        phase += (2.0f * juce::MathConstants<float>::pi * f0) / static_cast<float> (sr);
        const float body = std::sin (phase) * ae * 0.82f;
        const float sub = std::sin (phase * 0.5f) * ae * 0.45f;
        mono[static_cast<std::size_t> (i)] = (beater + body + sub) * velocity;
    }
    return mono;
}

std::vector<float> render808 (const TrapForgePreset& p, double sr, float velocity)
{
    const float dur = 1.8f;
    const int n = static_cast<int> (std::ceil (dur * sr));
    std::vector<float> mono (static_cast<std::size_t> (n), 0.0f);
    const float f0 = p.rootHz;
    const float glideT = p.glideMs / 1000.0f;
    const float target = f0 * std::pow (2.0f, p.glideSemi / 12.0f);
    float phase = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        const float t = static_cast<float> (i / sr);
        const float ae = adsr (t, p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease, dur * 0.95f);
        float freq = f0;
        if (glideT > 0.0f && std::abs (target - f0) > 0.01f)
        {
            const float u = juce::jmin (1.0f, t / glideT);
            freq = f0 + (target - f0) * std::pow (u, p.glideExponent);
        }
        phase += (2.0f * juce::MathConstants<float>::pi * freq) / static_cast<float> (sr);
        mono[static_cast<std::size_t> (i)] = std::sin (phase) * ae * velocity;
    }
    return mono;
}

std::vector<float> renderSnare (const TrapForgePreset& p, double sr, float velocity)
{
    const float dur = 0.32f;
    const int n = static_cast<int> (std::ceil (dur * sr));
    std::vector<float> mono (static_cast<std::size_t> (n), 0.0f);
    juce::Random rng (0x50010001);
    for (int i = 0; i < n; ++i)
    {
        const float t = static_cast<float> (i / sr);
        const float ae = adsr (t, p.ampAttack, p.ampDecay, p.ampSustain, p.ampRelease, dur * 0.85f);
        const float noise = (rng.nextFloat() * 2.0f - 1.0f) * std::exp (-t / 0.045f);
        mono[static_cast<std::size_t> (i)] = noise * ae * velocity * 0.9f;
    }
    return mono;
}

} // namespace

std::vector<float> renderDrumMono (const TrapForgePreset& preset, double sampleRate, float velocity)
{
    switch (preset.category)
    {
        case DrumCategory::sub808: return render808 (preset, sampleRate, velocity);
        case DrumCategory::snare:
        case DrumCategory::clap: return renderSnare (preset, sampleRate, velocity);
        case DrumCategory::kick:
        case DrumCategory::hihat:
        case DrumCategory::perc:
        default: return renderLayeredKick (preset, sampleRate, velocity);
    }
}

} // namespace trapforge
