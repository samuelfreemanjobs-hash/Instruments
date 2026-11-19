#pragma once

namespace pluggva
{

enum class Waveform : int
{
    saw = 0,
    square = 1,
    pulse = 2,
    triangle = 3
};

struct SynthParams
{
    float outputGainDb = -3.0f;
    Waveform osc1Wave = Waveform::saw;
    Waveform osc2Wave = Waveform::saw;
    float osc1Level = 0.85f;
    float osc2Level = 0.55f;
    float osc2DetuneCents = 7.0f;
    int osc2Octave = 0;
    float pulseWidth = 0.42f;
    float subLevel = 0.35f;
    float unisonAmount = 0.35f;
    float unisonSpreadCents = 18.0f;
    float filterCutoff = 0.68f;
    float filterRes = 0.22f;
    float filtEnvAmt = 0.5f;
    float drive = 0.15f;
    float crush = 0.0f;
    float chorus = 0.2f;
    float drift = 0.0f;
    float glideMs = 0.0f;
    float ampAttack = 0.01f;
    float ampDecay = 0.15f;
    float ampSustain = 0.85f;
    float ampRelease = 0.35f;
    float filtAttack = 0.005f;
    float filtDecay = 0.4f;
    float filtSustain = 0.3f;
    float filtRelease = 0.5f;
    float stereoWidth = 0.2f;
};

} // namespace pluggva
