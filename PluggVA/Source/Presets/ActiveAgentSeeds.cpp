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
        { "Agent Trap 20261108-00", "TRAP", [] {
              auto p = base();
              p.drive = 0.3547f;
              p.filterCutoff = 0.3273f;
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.subLevel = 0.7255f;
              p.unisonAmount = 1.7019f;
              return p;
          }() },
        { "Agent Trap 20261108-01", "TRAP", [] {
              auto p = base();
              p.ampDecay = -1.8402f;
              p.ampSustain = -1.0657f;
              p.filtEnvAmt = 1.4513f;
              p.glideMs = 27.3512f;
              p.osc2DetuneCents = 8.7174f;
              p.osc2Octave = -1.0170f;
              return p;
          }() },
        { "Agent Trap 20261108-02", "TRAP", [] {
              auto p = base();
              p.chorus = 2.5159f;
              p.filterCutoff = 0.3720f;
              p.osc2DetuneCents = -16.3944f;
              p.unisonAmount = 0.2769f;
              return p;
          }() },
        { "Agent Trap 20261108-03", "TRAP", [] {
              auto p = base();
              p.ampDecay = 1.2098f;
              p.crush = 0.2131f;
              p.drive = 0.4605f;
              p.filterRes = 1.2106f;
              p.osc1Wave = Waveform::square;
              return p;
          }() },
        { "Agent Trap 20261108-04", "TRAP", [] {
              auto p = base();
              p.drive = 0.3968f;
              p.filterCutoff = 0.4187f;
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::saw;
              p.subLevel = 0.7655f;
              p.unisonAmount = -0.8957f;
              return p;
          }() },
        { "Agent Trap 20261108-05", "TRAP", [] {
              auto p = base();
              p.ampDecay = 1.7207f;
              p.ampSustain = 1.3248f;
              p.filtEnvAmt = -0.5006f;
              p.glideMs = 25.7541f;
              p.osc2DetuneCents = 10.8861f;
              p.osc2Octave = 1.2734f;
              return p;
          }() },
        { "Agent Trap 20261108-06", "TRAP", [] {
              auto p = base();
              p.chorus = 2.5460f;
              p.filterCutoff = 0.3653f;
              p.osc2DetuneCents = -16.1148f;
              p.unisonAmount = 0.0142f;
              return p;
          }() },
        { "Agent Trap 20261108-07", "TRAP", [] {
              auto p = base();
              p.ampDecay = -0.3910f;
              p.crush = 0.1778f;
              p.drive = 0.5187f;
              p.filterRes = 2.3066f;
              p.osc1Wave = Waveform::square;
              return p;
          }() },
    };
}

} // namespace pluggva::presets
