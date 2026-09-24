#include "Presets/FoundationPresets.h"

namespace prophetrev2::presets
{

namespace
{
SynthParams base() { return {}; }
} // namespace

const std::array<FoundationPatch, kFoundationPresetCount>& getFoundationPresets() noexcept
{
    static const std::array<FoundationPatch, kFoundationPresetCount> kFoundations = { {
        // Bass (4)
        { "Traproom Sub", "Bass", [] {
              auto p = base();
              p.osc1Level = 1.0f; p.osc2Level = 0.35f; p.osc2DetuneCents = -12.0f; p.oscMix = 0.2f;
              p.filterCutoff = 0.28f; p.filterRes = 0.12f; p.filtEnvAmt = 0.35f;
              p.ampAttack = 0.002f; p.ampSustain = 0.92f; p.filtDecay = 0.55f;
              return p;
          }() },
        { "Gucci Glide Bass", "Bass", [] {
              auto p = base();
              p.glideMs = 120.0f; p.filterCutoff = 0.32f; p.filtEnvAmt = 0.55f; p.ampSustain = 0.88f;
              return p;
          }() },
        { "Shawty Dark Bass", "Bass", [] {
              auto p = base();
              p.osc2DetuneCents = -7.0f; p.filterCutoff = 0.25f; p.filterRes = 0.28f; p.ampSustain = 0.95f;
              return p;
          }() },
        { "Low Circuit", "Bass", [] {
              auto p = base();
              p.circuitDrive = 0.35f; p.filterDrive = 0.3f; p.filterCutoff = 0.22f; p.ampDecay = 0.55f;
              return p;
          }() },
        { "Wavephonk Knock", "Bass", [] {
              auto p = base();
              p.glideMs = 95.0f; p.osc2DetuneCents = -5.0f; p.filterCutoff = 0.24f; p.circuitDrive = 0.48f;
              p.filterDrive = 0.42f; p.ampAttack = 0.004f; p.ampSustain = 0.94f; p.filtDecay = 0.42f;
              return p;
          }() },
        { "Cloud Phonk Mist", "Bass", [] {
              auto p = base();
              p.osc2Level = 0.55f; p.osc2DetuneCents = -19.0f; p.oscMix = 0.35f; p.unisonVoices = 2.0f;
              p.unisonSpread = 8.0f; p.filterCutoff = 0.26f; p.filterRes = 0.08f; p.ampSustain = 0.9f;
              return p;
          }() },

        // Synth (6) — poly chords / beds
        { "Night Circuit", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.unisonSpread = 14.0f;
              p.osc2DetuneCents = 11.0f; p.filterCutoff = 0.58f; p.ampAttack = 0.045f; p.ampSustain = 0.82f;
              return p;
          }() },
        { "Vaporwave Cruise", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.ampAttack = 0.12f; p.ampSustain = 0.88f; p.osc2DetuneCents = 18.0f;
              return p;
          }() },
        { "Jeezy Stack Choir", "Synth", [] {
              auto p = base();
              p.unisonVoices = 3.0f; p.unisonSpread = 10.0f; p.filterCutoff = 0.5f; p.ampSustain = 0.86f;
              return p;
          }() },
        { "Neon Hall Chords", "Synth", [] {
              auto p = base();
              p.keyTrack = 0.55f; p.filterCutoff = 0.62f; p.ampSustain = 0.84f;
              return p;
          }() },
        { "Redd Key Stab", "Synth", [] {
              auto p = base();
              p.ampDecay = 0.35f; p.ampSustain = 0.55f; p.filterCutoff = 0.62f;
              return p;
          }() },
        { "Bourne Glass Keys", "Synth", [] {
              auto p = base();
              p.osc2DetuneCents = 16.0f; p.filterCutoff = 0.74f; p.filtEnvAmt = 0.58f;
              return p;
          }() },
        { "Wave Horizon", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.unisonSpread = 18.0f; p.osc2DetuneCents = 22.0f; p.ampAttack = 0.18f;
              p.ampSustain = 0.9f; p.filterCutoff = 0.52f; p.filtEnvAmt = 0.35f;
              return p;
          }() },
        { "Vapor Trap Glass", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.filterCutoff = 0.68f; p.filterRes = 0.32f; p.filtEnvAmt = 0.72f;
              p.filtDecay = 0.22f; p.ampDecay = 0.28f; p.ampSustain = 0.62f;
              return p;
          }() },
        { "Hardware Juno Stack", "Synth", [] {
              auto p = base();
              p.unisonVoices = 3.0f; p.unisonSpread = 11.0f; p.osc2DetuneCents = 9.0f; p.filterCutoff = 0.48f;
              p.keyTrack = 0.62f; p.ampSustain = 0.88f; p.circuitDrive = 0.12f;
              return p;
          }() },

        // Lead (4)
        { "Pierre Dream Lead", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.glideMs = 65.0f; p.osc2DetuneCents = 14.0f; p.filterCutoff = 0.68f;
              return p;
          }() },
        { "Jeezy Square Lead", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.filterCutoff = 0.58f; p.ampSustain = 0.8f;
              return p;
          }() },
        { "London Bright Lead", "Lead", [] {
              auto p = base();
              p.filterCutoff = 0.78f; p.filtEnvAmt = 0.7f; p.ampDecay = 0.18f;
              return p;
          }() },
        { "Wheezy Siren", "Lead", [] {
              auto p = base();
              p.osc2DetuneCents = 33.0f; p.filtEnvAmt = 0.95f; p.filterRes = 0.55f;
              return p;
          }() },
        { "Wavephonk Bell", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.glideMs = 45.0f; p.osc2DetuneCents = 24.0f; p.filterCutoff = 0.72f;
              p.filtEnvAmt = 0.88f; p.filterRes = 0.38f; p.ampDecay = 0.24f; p.circuitDrive = 0.22f;
              return p;
          }() },

        // Pad (3)
        { "Mike Dean Haze", "Pad", [] {
              auto p = base();
              p.ampAttack = 0.35f; p.ampRelease = 1.2f; p.filtAttack = 0.25f; p.ampSustain = 0.85f;
              return p;
          }() },
        { "Dean Analog Pad", "Pad", [] {
              auto p = base();
              p.ampAttack = 0.55f; p.ampRelease = 1.5f; p.filtEnvAmt = 0.28f;
              return p;
          }() },
        { "Void Circuit Pad", "Pad", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.filterCutoff = 0.42f; p.ampAttack = 0.45f; p.circuitDrive = 0.08f;
              return p;
          }() },
        { "Chillwave Drift", "Pad", [] {
              auto p = base();
              p.ampAttack = 0.62f; p.ampRelease = 2.1f; p.filtAttack = 0.4f; p.filtRelease = 1.8f;
              p.filterCutoff = 0.38f; p.filtEnvAmt = 0.22f; p.ampSustain = 0.82f; p.osc2DetuneCents = 14.0f;
              return p;
          }() },
        { "Ambient Phonk Void", "Pad", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.unisonSpread = 16.0f; p.filterCutoff = 0.32f; p.filterRes = 0.18f;
              p.ampAttack = 0.48f; p.ampRelease = 2.4f; p.circuitDrive = 0.18f; p.ampSustain = 0.78f;
              return p;
          }() },

        // Pluck/Keys (4)
        { "Mike Will Pluck", "Pluck/Keys", [] {
              auto p = base();
              p.ampAttack = 0.001f; p.ampDecay = 0.11f; p.ampSustain = 0.0f; p.filtEnvAmt = 0.82f;
              return p;
          }() },
        { "Wheezy Short Pluck", "Pluck/Keys", [] {
              auto p = base();
              p.ampDecay = 0.08f; p.filtDecay = 0.09f; p.osc2DetuneCents = 23.0f;
              return p;
          }() },
        { "Trap Wire Pluck", "Pluck/Keys", [] {
              auto p = base();
              p.filterCutoff = 0.7f; p.filterRes = 0.4f; p.circuitDrive = 0.25f;
              return p;
          }() },
        { "Staccato Click", "Pluck/Keys", [] {
              auto p = base();
              p.ampRelease = 0.12f; p.filtEnvAmt = 0.75f; p.filterCutoff = 0.66f;
              return p;
          }() },
        { "Vapor Trap Click", "Pluck/Keys", [] {
              auto p = base();
              p.ampAttack = 0.001f; p.ampDecay = 0.07f; p.ampSustain = 0.0f; p.filtDecay = 0.06f;
              p.filtEnvAmt = 0.9f; p.filterCutoff = 0.74f; p.osc2DetuneCents = 19.0f;
              return p;
          }() },
    } };

    return kFoundations;
}

} // namespace prophetrev2::presets
