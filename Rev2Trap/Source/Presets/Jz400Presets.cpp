#include "Presets/Jz400Presets.h"

namespace rev2trap::presets
{

SynthParams getJz400Preset (int index0to399) noexcept
{
    const int i = index0to399 < 0 ? 0 : (index0to399 >= kNumJz400Presets ? kNumJz400Presets - 1 : index0to399);
    const int archetype = i % 8;
    const int albumSlot = (i / 8) % 50;

    SynthParams p = getFactoryPreset (i % kNumFactoryPresets);
    p.trapMacro = 0.35f + 0.45f * static_cast<float> (archetype) / 7.0f;
    p.driftAmount = 0.08f + 0.12f * static_cast<float> (albumSlot % 7) / 6.0f;
    p.wavetableBlend = 0.25f + 0.5f * static_cast<float> ((i + 3) % 5) / 4.0f;
    p.hardSyncAmount = archetype <= 2 ? 0.55f : 0.15f;
    p.filterFm = 0.1f + 0.35f * static_cast<float> (archetype % 4) / 3.0f;

    switch (archetype)
    {
        case 0: p.cutoff = 0.68f; p.ampDecay = 0.25f; break;
        case 1: p.cutoff = 0.42f; p.ampSustain = 0.7f; break;
        case 2: p.subLevel = 0.62f; p.cutoff = 0.32f; break;
        case 3: p.resonance = 0.38f; p.filterEnv = 0.7f; break;
        case 4: p.osc2DetuneCents = 19.0f; p.drive = 1.18f; break;
        case 5: p.wavetableBlend = 0.75f; break;
        case 6: p.hardSyncAmount = 0.75f; break;
        default: p.filterFm = 0.45f; break;
    }
    return p;
}

} // namespace rev2trap::presets
