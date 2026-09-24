#pragma once

#include <algorithm>
#include <cmath>

namespace prophetrev2::dsp
{

/** Soft saturation (output stage / “circuit” warmth). */
inline float softClip (float sample, float drive01) noexcept
{
    const float drive = 1.0f + drive01 * 5.0f;
    const float y = std::tanh (sample * drive) / std::tanh (drive);
    return std::clamp (y, -1.0f, 1.0f);
}

/** Pre-filter input saturation (transistor ladder-ish). */
inline float filterInputSat (float sample, float drive01) noexcept
{
    const float amount = drive01 * 2.5f;
    return sample / (1.0f + amount * std::abs (sample));
}

/** Cutoff Hz with key tracking (midi note, center 60, amount 0–1 ≈ 0–2 oct). */
inline float keyTrackedCutoffHz (float baseHz, int midiNote, float keyTrack01) noexcept
{
    const float semis = static_cast<float> (midiNote - 60);
    const float octaves = (semis / 12.0f) * keyTrack01 * 2.0f;
    return baseHz * std::pow (2.0f, octaves);
}

} // namespace prophetrev2::dsp
