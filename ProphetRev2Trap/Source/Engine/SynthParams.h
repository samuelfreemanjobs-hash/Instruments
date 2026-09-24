#pragma once

namespace prophetrev2
{

struct SynthParams
{
    float osc1Level = 0.85f;
    float osc2Level = 0.65f;
    float osc2DetuneCents = 7.0f;
    float oscMix = 0.45f;
    float filterCutoff = 0.55f;
    float filterRes = 0.22f;
    float filtEnvAmt = 0.5f;
    float keyTrack = 0.45f;
    float circuitDrive = 0.15f;
    float filterDrive = 0.2f;
    float unisonVoices = 1.0f;
    float unisonSpread = 12.0f;
    float monoMode = 0.0f;
    float legatoMode = 1.0f;
    float glideMs = 80.0f;

    float ampAttack = 0.01f;
    float ampDecay = 0.2f;
    float ampSustain = 0.75f;
    float ampRelease = 0.35f;

    float filtAttack = 0.005f;
    float filtDecay = 0.35f;
    float filtSustain = 0.25f;
    float filtRelease = 0.45f;
};

} // namespace prophetrev2
