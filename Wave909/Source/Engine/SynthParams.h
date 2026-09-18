#pragma once

#include "Engine/SynthParams.h"

namespace wave909
{

struct SynthParams
{
    float outputGainDb = -3.0f;
    float wtPosition = 0.35f;
    float wtScan = 0.0f;
    float filterCutoff = 0.72f;
    float filterRes = 0.25f;
    bool filterDestructive = false;
    float drive = 0.2f;
    float crush = 0.0f;
    float subLevel = 0.35f;
    float detuneCents = 0.0f;
    float glideMs = 0.0f;
    float tapeWobble = 0.0f;
    float ampAttack = 0.01f;
    float ampDecay = 0.15f;
    float ampSustain = 0.85f;
    float ampRelease = 0.35f;
    float filtAttack = 0.005f;
    float filtDecay = 0.4f;
    float filtSustain = 0.3f;
    float filtRelease = 0.5f;
    float filtEnvAmt = 0.55f;
    float stereoWidth = 0.15f;
};

} // namespace wave909
