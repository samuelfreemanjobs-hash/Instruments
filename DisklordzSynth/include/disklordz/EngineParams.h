#pragma once

namespace disklordz::synth
{

/** Primary offline engine types (sellable as separate products). */
enum class EngineId
{
    subtractive,
    additive,
    karplus,
    wave
};

const char* engineIdName (EngineId id);

/** Parse CLI name; accepts legacy aliases (bell, keys, pad, sub808). */
EngineId parseEngineId (const char* name);

/**
 * Generic render knobs (0–1 unless noted). Meaning per engine:
 *
 * subtractive: a=cutoff, b=resonance, c=waveform (0=saw 1=pulse), d=decay seconds scale
 * additive:    a=brightness (harm count), b=inharmonicity, c=decay, d=detune cents/100
 * karplus:     a=decay, b=brightness, c=damping, d=pluck length ratio
 * wave:        a=table position (morph), b=spectral tilt, c=loop vs one-shot (0/1), d=decay
 */
struct EngineParams final
{
    float a = 0.5f;
    float b = 0.5f;
    float c = 0.5f;
    float d = 0.5f;
};

} // namespace disklordz::synth
