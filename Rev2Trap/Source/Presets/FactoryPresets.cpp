#include "Presets/FactoryPresets.h"

#include <cmath>

namespace rev2trap::presets
{

namespace
{
float laneBias (int lane, float jeezy, float shawty, float gucci, float neutral)
{
    switch (lane)
    {
        case 0: return jeezy;
        case 1: return shawty;
        case 2: return gucci;
        default: return neutral;
    }
}
} // namespace

SynthParams getFactoryPreset (int programIndex) noexcept
{
    const int i = programIndex < 0 ? 0 : (programIndex >= kNumFactoryPresets ? kNumFactoryPresets - 1 : programIndex);
    const int lane = i % 4;
    const int category = (i / 4) % 8;

    SynthParams p;
    const float t = static_cast<float> (i) / static_cast<float> (kNumFactoryPresets - 1);

    p.oscMix = 0.45f + 0.35f * laneBias (lane, 0.5f, 0.65f, 0.4f, 0.55f);
    p.osc2DetuneCents = laneBias (lane, 12.0f, 22.0f, -16.0f, 7.0f) + static_cast<float> ((i % 5) - 2);
    p.subLevel = 0.25f + 0.35f * laneBias (lane, 0.35f, 0.15f, 0.45f, 0.25f);

    switch (category)
    {
        case 0: // trap_lead
            p.cutoff = 0.62f + 0.2f * t;
            p.resonance = 0.22f + 0.15f * laneBias (lane, 0.0f, 0.25f, 0.1f, 0.05f);
            p.ampAttack = 0.002f;
            p.ampDecay = 0.28f;
            p.ampSustain = 0.35f;
            break;
        case 1: // dark_pad
            p.cutoff = 0.35f + 0.15f * t;
            p.resonance = 0.18f;
            p.ampAttack = 0.04f;
            p.ampDecay = 0.8f;
            p.ampSustain = 0.72f;
            p.filterEnv = 0.35f;
            break;
        case 2: // brass_stab
            p.cutoff = 0.7f;
            p.resonance = 0.35f;
            p.filterEnv = 0.75f;
            p.ampAttack = 0.001f;
            p.ampDecay = 0.22f;
            p.ampSustain = 0.0f;
            break;
        case 3: // sub_808
            p.subLevel = 0.65f;
            p.cutoff = 0.28f;
            p.oscMix = 0.35f;
            p.ampDecay = 0.55f;
            p.ampSustain = 0.5f;
            break;
        case 4: // pluck
            p.cutoff = 0.55f + 0.2f * t;
            p.ampAttack = 0.001f;
            p.ampDecay = 0.18f;
            p.ampSustain = 0.0f;
            p.filtDecay = 0.25f;
            break;
        case 5: // choir
            p.osc2DetuneCents += 14.0f;
            p.cutoff = 0.48f;
            p.ampAttack = 0.06f;
            p.ampSustain = 0.8f;
            break;
        case 6: // bell
            p.cutoff = 0.75f;
            p.resonance = 0.4f;
            p.ampDecay = 0.65f;
            p.drive = 1.05f;
            break;
        default: // noise_rise
            p.cutoff = 0.4f + 0.45f * t;
            p.filterEnv = 0.85f;
            p.filtAttack = 0.2f;
            p.ampAttack = 0.08f;
            break;
    }

    p.drive = laneBias (lane, 1.05f, 1.2f, 1.1f, 1.0f) * p.drive;
    return p;
}

} // namespace rev2trap::presets
