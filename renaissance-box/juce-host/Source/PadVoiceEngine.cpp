#include "PadVoiceEngine.h"

#include <cmath>

namespace renaissance
{
void PadVoiceEngine::prepare (double sampleRate)
{
    sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    for (int i = 0; i < kNumPads; ++i)
        levels_[static_cast<std::size_t> (i)] = 0.85f;
}

void PadVoiceEngine::triggerPad (int padIndex, float velocity)
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return;
    auto& v = voices_[static_cast<std::size_t> (padIndex)];
    const float hz = 110.0f + static_cast<float> (padIndex) * 18.0f;
    v.inc = hz / sampleRate_;
    v.phase = 0.0;
    v.env = juce::jlimit (0.0f, 1.0f, velocity);
    v.active = true;
}

void PadVoiceEngine::setPadLevel (int padIndex, float level0to1)
{
    if (padIndex >= 0 && padIndex < kNumPads)
        levels_[static_cast<std::size_t> (padIndex)] = juce::jlimit (0.0f, 1.0f, level0to1);
}

float PadVoiceEngine::padLevel (int padIndex) const noexcept
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return 0.0f;
    return levels_[static_cast<std::size_t> (padIndex)];
}

void PadVoiceEngine::render (juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    const int chans = buffer.getNumChannels();
    for (int i = 0; i < numSamples; ++i)
    {
        float mix = 0.0f;
        for (int p = 0; p < kNumPads; ++p)
        {
            auto& v = voices_[static_cast<std::size_t> (p)];
            if (! v.active)
                continue;
            const float s = static_cast<float> (std::sin (v.phase * juce::MathConstants<double>::twoPi));
            mix += s * v.env * levels_[static_cast<std::size_t> (p)] * 0.12f;
            v.phase += v.inc;
            v.env *= 0.9992f;
            if (v.env < 0.001f)
                v.active = false;
        }
        for (int c = 0; c < chans; ++c)
            buffer.setSample (c, startSample + i, mix);
    }
}

} // namespace renaissance
