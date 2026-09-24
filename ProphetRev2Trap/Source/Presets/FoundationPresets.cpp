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
        { "Wavephonk Slump", "Bass", [] {
              auto p = base();
              p.glideMs = 140.0f; p.filterCutoff = 0.2f; p.circuitDrive = 0.55f; p.filterDrive = 0.38f;
              p.ampAttack = 0.006f; p.ampSustain = 0.96f; p.osc2DetuneCents = -8.0f;
              return p;
          }() },
        { "Vapor Trap Sub", "Bass", [] {
              auto p = base();
              p.osc2Level = 0.45f; p.filterCutoff = 0.3f; p.filtEnvAmt = 0.65f; p.filtDecay = 0.35f;
              p.ampDecay = 0.4f; p.circuitDrive = 0.28f;
              return p;
          }() },
        { "Cloud Phonk Low", "Bass", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.unisonSpread = 6.0f; p.glideMs = 70.0f; p.filterCutoff = 0.23f;
              p.osc2DetuneCents = -14.0f; p.ampSustain = 0.93f;
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
        { "Wave Midnight", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.unisonSpread = 20.0f; p.ampAttack = 0.22f; p.filterCutoff = 0.44f;
              p.osc2DetuneCents = 26.0f; p.filtEnvAmt = 0.4f; p.ampSustain = 0.86f;
              return p;
          }() },
        { "Chillwave VHS", "Synth", [] {
              auto p = base();
              p.ampAttack = 0.28f; p.ampRelease = 1.6f; p.filterCutoff = 0.4f; p.filterRes = 0.15f;
              p.osc2DetuneCents = 17.0f; p.circuitDrive = 0.1f; p.ampSustain = 0.84f;
              return p;
          }() },
        { "Ambient Phonk Layer", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.unisonSpread = 14.0f; p.filterCutoff = 0.36f; p.ampAttack = 0.32f;
              p.circuitDrive = 0.22f; p.filtRelease = 1.2f; p.ampSustain = 0.8f;
              return p;
          }() },
        { "Hardware Poly Drive", "Synth", [] {
              auto p = base();
              p.unisonVoices = 3.0f; p.circuitDrive = 0.32f; p.filterDrive = 0.25f; p.filterCutoff = 0.54f;
              p.keyTrack = 0.58f; p.ampDecay = 0.32f;
              return p;
          }() },
        { "Vapor Trap Stack", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.filterCutoff = 0.64f; p.filtEnvAmt = 0.8f; p.osc2DetuneCents = 21.0f;
              p.ampAttack = 0.02f; p.ampSustain = 0.7f;
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
        { "Wavephonk Memphis", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.glideMs = 55.0f; p.filterCutoff = 0.62f; p.circuitDrive = 0.38f;
              p.filtEnvAmt = 0.75f; p.ampSustain = 0.72f; p.osc2DetuneCents = 11.0f;
              return p;
          }() },
        { "Vapor Trap Hook", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.glideMs = 38.0f; p.filterCutoff = 0.76f; p.filtEnvAmt = 0.82f;
              p.ampDecay = 0.2f; p.filterRes = 0.42f;
              return p;
          }() },
        { "Hardware Solo Lead", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.legatoMode = 1.0f; p.filterCutoff = 0.66f; p.keyTrack = 0.7f;
              p.ampAttack = 0.008f; p.osc2DetuneCents = 8.0f;
              return p;
          }() },
        { "Cloud Phonk Air", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.unisonVoices = 2.0f; p.unisonSpread = 9.0f; p.filterCutoff = 0.58f;
              p.ampAttack = 0.015f; p.osc2DetuneCents = 20.0f; p.ampRelease = 0.55f;
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
        { "Chillwave Sunset", "Pad", [] {
              auto p = base();
              p.ampAttack = 0.72f; p.ampRelease = 2.6f; p.filterCutoff = 0.35f; p.filtAttack = 0.5f;
              p.osc2DetuneCents = 19.0f; p.ampSustain = 0.8f;
              return p;
          }() },
        { "Cloud Phonk Haze", "Pad", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.filterCutoff = 0.3f; p.ampAttack = 0.55f; p.ampRelease = 2.2f;
              p.osc2DetuneCents = -11.0f; p.circuitDrive = 0.14f;
              return p;
          }() },
        { "Ambient Phonk Smoke", "Pad", [] {
              auto p = base();
              p.filterCutoff = 0.28f; p.filterRes = 0.22f; p.ampAttack = 0.65f; p.filtEnvAmt = 0.18f;
              p.circuitDrive = 0.25f; p.ampRelease = 2.8f;
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
        { "Wave Tape Keys", "Pluck/Keys", [] {
              auto p = base();
              p.ampAttack = 0.002f; p.ampDecay = 0.14f; p.ampSustain = 0.05f; p.filterCutoff = 0.62f;
              p.osc2DetuneCents = 15.0f; p.circuitDrive = 0.12f;
              return p;
          }() },
        { "Cloud Phonk Bell", "Pluck/Keys", [] {
              auto p = base();
              p.ampDecay = 0.09f; p.filtDecay = 0.08f; p.filterCutoff = 0.78f; p.filtEnvAmt = 0.85f;
              p.osc2DetuneCents = 27.0f; p.filterRes = 0.35f;
              return p;
          }() },

        // Genre expansion batch 2 (26) — doubles genre foundation set
        { "Wavephonk Grave", "Bass", [] {
              auto p = base();
              p.filterCutoff = 0.19f; p.circuitDrive = 0.52f; p.glideMs = 110.0f; p.ampSustain = 0.97f;
              p.osc2DetuneCents = -6.0f; p.filtDecay = 0.48f;
              return p;
          }() },
        { "Wavephonk Stomp", "Bass", [] {
              auto p = base();
              p.ampAttack = 0.003f; p.filterDrive = 0.45f; p.filterCutoff = 0.27f; p.filtEnvAmt = 0.48f;
              p.circuitDrive = 0.44f;
              return p;
          }() },
        { "Ambient Phonk Crawl", "Bass", [] {
              auto p = base();
              p.glideMs = 85.0f; p.filterCutoff = 0.21f; p.ampDecay = 0.62f; p.circuitDrive = 0.3f;
              p.osc2Level = 0.5f; p.osc2DetuneCents = -16.0f;
              return p;
          }() },
        { "Cloud Phonk Pulse", "Bass", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.filterCutoff = 0.25f; p.filtEnvAmt = 0.42f; p.filtDecay = 0.38f;
              p.ampAttack = 0.005f; p.oscMix = 0.32f;
              return p;
          }() },
        { "Hardware Sub Drive", "Bass", [] {
              auto p = base();
              p.circuitDrive = 0.28f; p.keyTrack = 0.52f; p.filterCutoff = 0.29f; p.ampSustain = 0.91f;
              p.filterDrive = 0.22f;
              return p;
          }() },

        { "Wave Dawn", "Synth", [] {
              auto p = base();
              p.ampAttack = 0.25f; p.unisonSpread = 17.0f; p.osc2DetuneCents = 20.0f; p.filterCutoff = 0.5f;
              p.ampSustain = 0.87f; p.unisonVoices = 2.0f;
              return p;
          }() },
        { "Wave Shimmer", "Synth", [] {
              auto p = base();
              p.filterCutoff = 0.66f; p.filtEnvAmt = 0.55f; p.osc2DetuneCents = 25.0f; p.ampAttack = 0.14f;
              p.unisonVoices = 2.0f; p.unisonSpread = 19.0f;
              return p;
          }() },
        { "Vapor Trap Pulse", "Synth", [] {
              auto p = base();
              p.filtDecay = 0.18f; p.ampDecay = 0.25f; p.filterRes = 0.36f; p.filtEnvAmt = 0.78f;
              p.filterCutoff = 0.7f;
              return p;
          }() },
        { "Vapor Trap Mist", "Synth", [] {
              auto p = base();
              p.unisonVoices = 2.0f; p.ampAttack = 0.08f; p.filterCutoff = 0.56f; p.osc2DetuneCents = 18.0f;
              p.ampSustain = 0.75f; p.filtEnvAmt = 0.45f;
              return p;
          }() },
        { "Chillwave Pool", "Synth", [] {
              auto p = base();
              p.ampAttack = 0.32f; p.filterCutoff = 0.42f; p.filterRes = 0.12f; p.ampRelease = 1.4f;
              p.osc2DetuneCents = 13.0f;
              return p;
          }() },
        { "Chillwave Dim", "Synth", [] {
              auto p = base();
              p.filterCutoff = 0.37f; p.ampAttack = 0.38f; p.circuitDrive = 0.09f; p.filtSustain = 0.35f;
              p.ampSustain = 0.83f;
              return p;
          }() },
        { "Hardware Brass Stack", "Synth", [] {
              auto p = base();
              p.unisonVoices = 3.0f; p.filterCutoff = 0.6f; p.filtEnvAmt = 0.62f; p.keyTrack = 0.65f;
              p.ampDecay = 0.28f; p.unisonSpread = 9.0f;
              return p;
          }() },
        { "Ambient Phonk Choir", "Synth", [] {
              auto p = base();
              p.unisonVoices = 3.0f; p.unisonSpread = 15.0f; p.filterCutoff = 0.34f; p.ampAttack = 0.28f;
              p.circuitDrive = 0.2f; p.ampRelease = 1.5f;
              return p;
          }() },

        { "Wavephonk Chain", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.circuitDrive = 0.42f; p.filterCutoff = 0.64f; p.glideMs = 50.0f;
              p.filtEnvAmt = 0.7f; p.ampDecay = 0.22f;
              return p;
          }() },
        { "Wavephonk Ring", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.osc2DetuneCents = 28.0f; p.filterRes = 0.45f; p.filterCutoff = 0.74f;
              p.filtEnvAmt = 0.92f; p.ampAttack = 0.004f;
              return p;
          }() },
        { "Vapor Trap Rise", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.filtAttack = 0.02f; p.filtEnvAmt = 0.95f; p.filterCutoff = 0.8f;
              p.glideMs = 42.0f; p.ampSustain = 0.65f;
              return p;
          }() },
        { "Wave Glide", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.glideMs = 90.0f; p.osc2DetuneCents = 16.0f; p.filterCutoff = 0.6f;
              p.legatoMode = 1.0f; p.ampAttack = 0.012f;
              return p;
          }() },
        { "Cloud Phonk Wire", "Lead", [] {
              auto p = base();
              p.monoMode = 1.0f; p.unisonSpread = 7.0f; p.filterCutoff = 0.55f; p.osc2DetuneCents = 22.0f;
              p.ampRelease = 0.48f; p.unisonVoices = 2.0f;
              return p;
          }() },

        { "Wave Deep", "Pad", [] {
              auto p = base();
              p.ampAttack = 0.58f; p.ampRelease = 2.3f; p.filterCutoff = 0.33f; p.unisonVoices = 2.0f;
              p.osc2DetuneCents = 12.0f; p.filtAttack = 0.35f;
              return p;
          }() },
        { "Cloud Phonk Float", "Pad", [] {
              auto p = base();
              p.ampAttack = 0.68f; p.filterCutoff = 0.29f; p.unisonSpread = 12.0f; p.ampRelease = 2.5f;
              p.osc2DetuneCents = -9.0f; p.unisonVoices = 2.0f;
              return p;
          }() },
        { "Chillwave Dust", "Pad", [] {
              auto p = base();
              p.filterCutoff = 0.36f; p.ampAttack = 0.78f; p.filtEnvAmt = 0.2f; p.ampRelease = 2.7f;
              p.circuitDrive = 0.07f;
              return p;
          }() },
        { "Ambient Phonk Night", "Pad", [] {
              auto p = base();
              p.filterCutoff = 0.26f; p.ampAttack = 0.72f; p.circuitDrive = 0.22f; p.filtRelease = 2.0f;
              p.unisonVoices = 2.0f; p.filterRes = 0.2f;
              return p;
          }() },
        { "Vapor Trap Fade", "Pad", [] {
              auto p = base();
              p.ampAttack = 0.42f; p.ampRelease = 1.9f; p.filterCutoff = 0.46f; p.filtEnvAmt = 0.38f;
              p.osc2DetuneCents = 20.0f; p.ampSustain = 0.76f;
              return p;
          }() },

        { "Wave Echo Keys", "Pluck/Keys", [] {
              auto p = base();
              p.ampDecay = 0.12f; p.filtDecay = 0.11f; p.filterCutoff = 0.64f; p.osc2DetuneCents = 17.0f;
              p.filtEnvAmt = 0.68f;
              return p;
          }() },
        { "Hardware Keys Pluck", "Pluck/Keys", [] {
              auto p = base();
              p.ampAttack = 0.001f; p.ampDecay = 0.1f; p.keyTrack = 0.55f; p.filterCutoff = 0.68f;
              p.circuitDrive = 0.15f;
              return p;
          }() },
        { "Vapor Trap Rim", "Pluck/Keys", [] {
              auto p = base();
              p.ampDecay = 0.05f; p.ampRelease = 0.08f; p.filterRes = 0.48f; p.filtEnvAmt = 0.88f;
              p.filterCutoff = 0.72f;
              return p;
          }() },
    } };

    return kFoundations;
}

} // namespace prophetrev2::presets
