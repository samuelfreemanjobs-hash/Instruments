// AUTO-GENERATED — do not edit. Run scripts/vst/daily_preset_batch.py

#include "Presets/ActiveAgentSeeds.h"
#include "Engine/SynthParams.h"

#include <vector>

namespace pluggva::presets
{

namespace
{
SynthParams base() { return SynthParams {}; }
} // namespace

std::vector<FactoryPreset> getActiveAgentSeeds()
{
    return {
        { "Southside Sub (1108)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.5020f;
              p.ampSustain = 0.8890f;
              p.drive = 0.4096f;
              p.filterCutoff = 0.3412f;
              p.glideMs = 119.9840f;
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.subLevel = 0.9094f;
              return p;
          }() },
        { "Memphis Undertow (1108)", "TRAP", [] {
              auto p = base();
              p.drive = 0.3588f;
              p.filterCutoff = 0.3916f;
              p.osc1Wave = Waveform::saw;
              p.osc2DetuneCents = -15.8413f;
              p.osc2Wave = Waveform::saw;
              p.subLevel = 0.6701f;
              p.unisonAmount = 0.8689f;
              p.unisonSpreadCents = 27.8222f;
              return p;
          }() },
        { "Carti Bell Pluck (1108)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.0816f;
              p.ampRelease = 0.2925f;
              p.ampSustain = 0.0094f;
              p.chorus = 0.2800f;
              p.filtEnvAmt = 0.8210f;
              p.glideMs = 32.0228f;
              p.osc2DetuneCents = 10.1476f;
              p.osc2Octave = 1;
              return p;
          }() },
        { "Nardo Bell Pluck (1108)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.0823f;
              p.ampRelease = 0.2930f;
              p.ampSustain = 0.0095f;
              p.chorus = 0.2800f;
              p.filtEnvAmt = 0.8217f;
              p.glideMs = 32.0384f;
              p.osc2DetuneCents = 10.1358f;
              p.osc2Octave = 1;
              return p;
          }() },
        { "Brick Wall (1108)", "TRAP", [] {
              auto p = base();
              p.chorus = 0.4800f;
              p.drive = 0.2863f;
              p.filterCutoff = 0.5620f;
              p.glideMs = 75.0644f;
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.unisonAmount = 0.9081f;
              p.unisonSpreadCents = 29.8613f;
              return p;
          }() },
        { "Travis Hook (1108)", "TRAP", [] {
              auto p = base();
              p.chorus = 0.5200f;
              p.drift = 0.2000f;
              p.filterCutoff = 0.7320f;
              p.glideMs = 115.0654f;
              p.stereoWidth = 0.4200f;
              p.unisonAmount = 0.5381f;
              return p;
          }() },
        { "Trap Knife (1108)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.1459f;
              p.ampSustain = 0.0496f;
              p.crush = 0.1440f;
              p.drive = 0.5255f;
              p.filtEnvAmt = 0.8636f;
              p.filterRes = 0.4844f;
              return p;
          }() },
        { "Whole Drift (1108)", "TRAP", [] {
              auto p = base();
              p.ampSustain = 0.6293f;
              p.chorus = 0.3600f;
              p.filterCutoff = 0.6116f;
              p.glideMs = 145.1140f;
              p.unisonAmount = 0.4681f;
              return p;
          }() },
        { "Southside Sub (1120)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.4916f;
              p.ampSustain = 0.8877f;
              p.drive = 0.3894f;
              p.filterCutoff = 0.3292f;
              p.glideMs = 120.0448f;
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.subLevel = 0.9095f;
              return p;
          }() },
        { "Memphis Undertow (1120)", "TRAP", [] {
              auto p = base();
              p.drive = 0.3401f;
              p.filterCutoff = 0.3779f;
              p.osc1Wave = Waveform::saw;
              p.osc2DetuneCents = -16.1778f;
              p.osc2Wave = Waveform::saw;
              p.subLevel = 0.6689f;
              p.unisonAmount = 0.8888f;
              p.unisonSpreadCents = 27.8222f;
              return p;
          }() },
        { "Carti Bell Pluck (1120)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.0705f;
              p.ampRelease = 0.2909f;
              p.ampSustain = 0.0066f;
              p.chorus = 0.2800f;
              p.filtEnvAmt = 0.8266f;
              p.glideMs = 32.0819f;
              p.osc2DetuneCents = 9.8316f;
              p.osc2Octave = 1;
              return p;
          }() },
        { "Nardo Bell Pluck (1120)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.0710f;
              p.ampRelease = 0.2907f;
              p.ampSustain = 0.0060f;
              p.chorus = 0.2800f;
              p.filtEnvAmt = 0.8260f;
              p.glideMs = 32.0963f;
              p.osc2DetuneCents = 9.8419f;
              p.osc2Octave = 1;
              return p;
          }() },
        { "Brick Wall (1120)", "TRAP", [] {
              auto p = base();
              p.chorus = 0.4800f;
              p.drive = 0.2723f;
              p.filterCutoff = 0.5448f;
              p.glideMs = 75.1195f;
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.unisonAmount = 0.9263f;
              p.unisonSpreadCents = 29.8613f;
              return p;
          }() },
        { "Travis Hook (1120)", "TRAP", [] {
              auto p = base();
              p.chorus = 0.5200f;
              p.drift = 0.2000f;
              p.filterCutoff = 0.7148f;
              p.glideMs = 115.1204f;
              p.stereoWidth = 0.4200f;
              p.unisonAmount = 0.5563f;
              return p;
          }() },
        { "Trap Knife (1120)", "TRAP", [] {
              auto p = base();
              p.ampDecay = 0.1344f;
              p.ampSustain = 0.0444f;
              p.crush = 0.1545f;
              p.drive = 0.5130f;
              p.filtEnvAmt = 0.8644f;
              p.filterRes = 0.4844f;
              return p;
          }() },
        { "Whole Drift (1120)", "TRAP", [] {
              auto p = base();
              p.ampSustain = 0.6229f;
              p.chorus = 0.3600f;
              p.filterCutoff = 0.5923f;
              p.glideMs = 145.1607f;
              p.unisonAmount = 0.4836f;
              return p;
          }() },
    };
}

} // namespace pluggva::presets
