#include "Presets/FactoryPresets.h"

namespace prophetrev2::presets
{

namespace
{
SynthParams base() { return {}; }
} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = {
        // --- Bass (synthetic subs & slides — no acoustic) ---
        { "Traproom Sub", "Bass", [] {
              auto p = base();
              p.osc1Level = 1.0f;
              p.osc2Level = 0.35f;
              p.osc2DetuneCents = -12.0f;
              p.oscMix = 0.2f;
              p.filterCutoff = 0.28f;
              p.filterRes = 0.12f;
              p.filtEnvAmt = 0.35f;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.45f;
              p.ampSustain = 0.92f;
              p.ampRelease = 0.3f;
              p.filtAttack = 0.001f;
              p.filtDecay = 0.55f;
              p.filtSustain = 0.15f;
              p.filtRelease = 0.35f;
              return p;
          }() },
        { "Gucci Glide Bass", "Bass", [] {
              auto p = base();
              p.osc1Level = 0.95f;
              p.osc2Level = 0.55f;
              p.osc2DetuneCents = 5.0f;
              p.filterCutoff = 0.32f;
              p.filterRes = 0.18f;
              p.filtEnvAmt = 0.55f;
              p.ampAttack = 0.008f;
              p.ampDecay = 0.35f;
              p.ampSustain = 0.88f;
              p.ampRelease = 0.55f;
              return p;
          }() },
        { "Shawty Dark Bass", "Bass", [] {
              auto p = base();
              p.osc1Level = 0.88f;
              p.osc2Level = 0.72f;
              p.osc2DetuneCents = -7.0f;
              p.oscMix = 0.55f;
              p.filterCutoff = 0.25f;
              p.filterRes = 0.28f;
              p.filtEnvAmt = 0.42f;
              p.ampAttack = 0.001f;
              p.ampSustain = 0.95f;
              p.ampRelease = 0.25f;
              return p;
          }() },

        // --- Lead ---
        { "Pierre Dream Lead", "Lead", [] {
              auto p = base();
              p.osc1Level = 0.75f;
              p.osc2Level = 0.85f;
              p.osc2DetuneCents = 14.0f;
              p.oscMix = 0.62f;
              p.filterCutoff = 0.68f;
              p.filterRes = 0.32f;
              p.filtEnvAmt = 0.62f;
              p.ampAttack = 0.012f;
              p.ampDecay = 0.28f;
              p.ampSustain = 0.72f;
              p.ampRelease = 0.45f;
              p.filtDecay = 0.22f;
              p.filtSustain = 0.45f;
              return p;
          }() },
        { "Jeezy Square Lead", "Lead", [] {
              auto p = base();
              p.osc1Level = 0.92f;
              p.osc2Level = 0.48f;
              p.osc2DetuneCents = 0.0f;
              p.filterCutoff = 0.58f;
              p.filterRes = 0.25f;
              p.filtEnvAmt = 0.48f;
              p.ampAttack = 0.004f;
              p.ampSustain = 0.8f;
              return p;
          }() },
        { "London Bright Lead", "Lead", [] {
              auto p = base();
              p.osc2Level = 0.9f;
              p.osc2DetuneCents = 19.0f;
              p.filterCutoff = 0.78f;
              p.filterRes = 0.38f;
              p.filtEnvAmt = 0.7f;
              p.ampDecay = 0.18f;
              p.filtAttack = 0.002f;
              p.filtDecay = 0.15f;
              return p;
          }() },

        // --- Pad ---
        { "Mike Dean Haze", "Pad", [] {
              auto p = base();
              p.osc1Level = 0.7f;
              p.osc2Level = 0.7f;
              p.osc2DetuneCents = 11.0f;
              p.oscMix = 0.5f;
              p.filterCutoff = 0.52f;
              p.filterRes = 0.2f;
              p.filtEnvAmt = 0.35f;
              p.ampAttack = 0.35f;
              p.ampDecay = 0.8f;
              p.ampSustain = 0.85f;
              p.ampRelease = 1.2f;
              p.filtAttack = 0.25f;
              p.filtRelease = 0.9f;
              return p;
          }() },
        { "Dean Analog Pad", "Pad", [] {
              auto p = base();
              p.osc2DetuneCents = -9.0f;
              p.filterCutoff = 0.48f;
              p.ampAttack = 0.55f;
              p.ampRelease = 1.5f;
              p.filtEnvAmt = 0.28f;
              return p;
          }() },

        // --- Pluck ---
        { "Mike Will Pluck", "Pluck", [] {
              auto p = base();
              p.filterCutoff = 0.72f;
              p.filterRes = 0.42f;
              p.filtEnvAmt = 0.82f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.11f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.22f;
              p.filtAttack = 0.001f;
              p.filtDecay = 0.14f;
              p.filtSustain = 0.0f;
              p.filtRelease = 0.18f;
              return p;
          }() },
        { "Wheezy Short Pluck", "Pluck", [] {
              auto p = base();
              p.osc2DetuneCents = 23.0f;
              p.filterCutoff = 0.66f;
              p.filtEnvAmt = 0.88f;
              p.ampDecay = 0.08f;
              p.ampRelease = 0.15f;
              p.filtDecay = 0.09f;
              return p;
          }() },

        // --- Keys ---
        { "Redd Key Stab", "Keys", [] {
              auto p = base();
              p.osc1Level = 0.82f;
              p.osc2Level = 0.78f;
              p.osc2DetuneCents = 6.0f;
              p.filterCutoff = 0.62f;
              p.filtEnvAmt = 0.52f;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.35f;
              p.ampSustain = 0.55f;
              p.ampRelease = 0.4f;
              return p;
          }() },
        { "Bourne Glass Keys", "Keys", [] {
              auto p = base();
              p.osc2DetuneCents = 16.0f;
              p.filterCutoff = 0.74f;
              p.filterRes = 0.3f;
              p.ampAttack = 0.006f;
              p.ampDecay = 0.42f;
              p.ampSustain = 0.48f;
              p.filtEnvAmt = 0.58f;
              return p;
          }() },

        // --- Synth FX ---
        { "Wheezy Siren", "Synth FX", [] {
              auto p = base();
              p.osc2DetuneCents = 33.0f;
              p.filterCutoff = 0.85f;
              p.filterRes = 0.55f;
              p.filtEnvAmt = 0.95f;
              p.ampAttack = 0.08f;
              p.ampSustain = 0.65f;
              p.filtAttack = 0.05f;
              p.filtDecay = 0.65f;
              p.filtSustain = 0.55f;
              return p;
          }() },
        { "Wire Sweep", "Synth FX", [] {
              auto p = base();
              p.osc1Level = 0.6f;
              p.osc2Level = 0.95f;
              p.filterCutoff = 0.4f;
              p.filterRes = 0.48f;
              p.filtEnvAmt = 1.0f;
              p.ampAttack = 0.02f;
              p.ampRelease = 0.8f;
              p.filtAttack = 0.015f;
              p.filtDecay = 1.1f;
              p.filtSustain = 0.1f;
              p.filtRelease = 0.7f;
              return p;
          }() },
    };

    return kPresets;
}

} // namespace prophetrev2::presets
