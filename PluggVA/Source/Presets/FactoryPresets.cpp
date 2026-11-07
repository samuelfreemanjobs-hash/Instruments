#include "Presets/FactoryPresets.h"

#include <vector>

namespace pluggva::presets
{

namespace
{
SynthParams base()
{
    return SynthParams {};
}
} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = {
        { "Atlanta Plugg Pluck", "PLUGG", [] {
              auto p = base();
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::square;
              p.osc2Octave = 1;
              p.osc2DetuneCents = 11.0f;
              p.filterCutoff = 0.62f;
              p.filterRes = 0.35f;
              p.filtEnvAmt = 0.72f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.11f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.28f;
              p.chorus = 0.35f;
              p.glideMs = 35.0f;
              return p;
          }() },
        { "Space Lead (Whole Lotta)", "PLUGG", [] {
              auto p = base();
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.unisonAmount = 0.55f;
              p.unisonSpreadCents = 22.0f;
              p.filterCutoff = 0.78f;
              p.filterRes = 0.18f;
              p.chorus = 0.55f;
              p.stereoWidth = 0.4f;
              p.glideMs = 120.0f;
              p.drift = 0.25f;
              return p;
          }() },
        { "Brick Boys Supersaw", "TRAP", [] {
              auto p = base();
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.osc2Level = 0.7f;
              p.osc2DetuneCents = -9.0f;
              p.unisonAmount = 0.85f;
              p.unisonSpreadCents = 28.0f;
              p.filterCutoff = 0.58f;
              p.filterRes = 0.32f;
              p.drive = 0.25f;
              p.chorus = 0.45f;
              p.glideMs = 80.0f;
              return p;
          }() },
        { "Dirty South Glide", "SOUTH", [] {
              auto p = base();
              p.osc1Wave = Waveform::square;
              p.osc2Wave = Waveform::saw;
              p.filterCutoff = 0.52f;
              p.filterRes = 0.28f;
              p.filtEnvAmt = 0.55f;
              p.ampAttack = 0.02f;
              p.ampSustain = 0.7f;
              p.glideMs = 220.0f;
              p.chorus = 0.3f;
              p.stereoWidth = 0.35f;
              return p;
          }() },
        { "Arena Pluck (Bellish)", "PLUCK", [] {
              auto p = base();
              p.osc1Wave = Waveform::triangle;
              p.osc2Wave = Waveform::square;
              p.osc2Octave = 1;
              p.osc2DetuneCents = 5.0f;
              p.filterCutoff = 0.74f;
              p.filterRes = 0.42f;
              p.filtEnvAmt = 0.8f;
              p.ampDecay = 0.14f;
              p.ampSustain = 0.05f;
              p.ampRelease = 0.35f;
              return p;
          }() },
        { "Rage Resonance Stab", "RAGE", [] {
              auto p = base();
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::pulse;
              p.pulseWidth = 0.28f;
              p.filterCutoff = 0.48f;
              p.filterRes = 0.62f;
              p.filtEnvAmt = 0.85f;
              p.drive = 0.55f;
              p.crush = 0.15f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.09f;
              p.ampSustain = 0.0f;
              return p;
          }() },
        { "Memphis Sub Stack", "PHONK", [] {
              auto p = base();
              p.osc1Wave = Waveform::square;
              p.osc2Level = 0.2f;
              p.subLevel = 0.92f;
              p.filterCutoff = 0.32f;
              p.filterRes = 0.12f;
              p.drive = 0.48f;
              p.crush = 0.35f;
              p.drift = 0.4f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.4f;
              p.ampSustain = 0.9f;
              return p;
          }() },
        { "Phonkwave Drift", "PHONK", [] {
              auto p = base();
              p.osc1Wave = Waveform::saw;
              p.unisonAmount = 0.4f;
              p.filterCutoff = 0.55f;
              p.drive = 0.35f;
              p.crush = 0.25f;
              p.drift = 0.65f;
              p.chorus = 0.25f;
              return p;
          }() },
        { "Majestic Casual Pad", "PAD", [] {
              auto p = base();
              p.osc1Wave = Waveform::triangle;
              p.osc2Wave = Waveform::saw;
              p.osc2Level = 0.35f;
              p.unisonAmount = 0.65f;
              p.unisonSpreadCents = 16.0f;
              p.filterCutoff = 0.62f;
              p.ampAttack = 0.35f;
              p.ampSustain = 0.85f;
              p.ampRelease = 0.9f;
              p.chorus = 0.6f;
              p.stereoWidth = 0.55f;
              return p;
          }() },
        { "Nightdrive Keys", "KEYS", [] {
              auto p = base();
              p.osc1Wave = Waveform::square;
              p.osc2Wave = Waveform::triangle;
              p.osc2Octave = -1;
              p.filterCutoff = 0.68f;
              p.filtEnvAmt = 0.45f;
              p.chorus = 0.4f;
              p.drift = 0.2f;
              return p;
          }() },
        { "808 Trap Reese", "BASS", [] {
              auto p = base();
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.osc2DetuneCents = -14.0f;
              p.unisonAmount = 0.7f;
              p.subLevel = 0.75f;
              p.filterCutoff = 0.38f;
              p.drive = 0.4f;
              p.ampDecay = 0.55f;
              p.ampSustain = 0.8f;
              return p;
          }() },
        { "Metro Detuned Bell", "BELL", [] {
              auto p = base();
              p.osc1Wave = Waveform::triangle;
              p.osc2Wave = Waveform::saw;
              p.osc2Octave = 2;
              p.osc2DetuneCents = 16.0f;
              p.filterCutoff = 0.81f;
              p.filterRes = 0.38f;
              p.filtEnvAmt = 0.65f;
              p.ampDecay = 0.22f;
              p.ampRelease = 0.55f;
              return p;
          }() },
    };

    return kPresets;
}

} // namespace pluggva::presets
