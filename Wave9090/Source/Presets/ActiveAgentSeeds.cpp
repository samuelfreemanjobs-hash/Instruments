// AUTO-GENERATED — do not edit. Run scripts/vst/daily_preset_batch.py

#include "Presets/ActiveAgentSeeds.h"
#include "Engine/SynthParams.h"

#include <vector>

namespace wave9090::presets
{

namespace
{
SynthParams baseParams() { return SynthParams {}; }
} // namespace

std::vector<FactoryPreset> getActiveAgentSeeds()
{
    return {
        { "Agent Trap 20261108-00", "TRAP", [] {
              auto p = baseParams();
              p.ampSustain = 2.0750618279872595f;
              p.drive = 0.4467852885778587f;
              p.filterCutoff = 0.21300129062672005f;
              p.subLevel = 0.9330035182042943f;
              p.wtPosition = 0.6267312158149461f;
              return p;
          }() },
        { "Agent Trap 20261108-01", "TRAP", [] {
              auto p = baseParams();
              p.ampDecay = -1.9026936033788022f;
              p.ampSustain = -0.37477526551070195f;
              p.filtEnvAmt = -1.4169733466972625f;
              p.filterCutoff = 0.7070615481245814f;
              p.glideMs = 34.649462279057296f;
              p.wtPosition = 0.7612039384907493f;
              return p;
          }() },
        { "Agent Trap 20261108-02", "TRAP", [] {
              auto p = baseParams();
              p.crush = 0.23798613253354278f;
              p.detuneCents = -14.272627345517455f;
              p.drive = 0.48386051808239783f;
              p.filterCutoff = 0.445392977672386f;
              p.wtPosition = 0.8206426345724034f;
              return p;
          }() },
        { "Agent Trap 20261108-03", "TRAP", [] {
              auto p = baseParams();
              p.crush = 0.41071188206510906f;
              p.drive = 0.6571316548013392f;
              p.filterDestructive = true;
              p.tapeWobble = -2.168805953545307f;
              p.wtPosition = 0.6464528746448837f;
              return p;
          }() },
        { "Agent Trap 20261108-04", "TRAP", [] {
              auto p = baseParams();
              p.ampSustain = 3.13462274659255f;
              p.drive = 0.43865753997741375f;
              p.filterCutoff = 0.29377672739858723f;
              p.subLevel = 0.940830061982159f;
              p.wtPosition = 0.646698310219937f;
              return p;
          }() },
        { "Agent Trap 20261108-05", "TRAP", [] {
              auto p = baseParams();
              p.ampDecay = -1.8557613911993727f;
              p.ampSustain = 1.667972958467807f;
              p.filtEnvAmt = 1.337884950561077f;
              p.filterCutoff = 0.6111295012056178f;
              p.glideMs = 37.47421571171702f;
              p.wtPosition = 0.7203913454135152f;
              return p;
          }() },
        { "Agent Trap 20261108-06", "TRAP", [] {
              auto p = baseParams();
              p.crush = 0.20542412526500242f;
              p.detuneCents = -11.826105161017436f;
              p.drive = 0.5767738916775245f;
              p.filterCutoff = 0.44624487575278726f;
              p.wtPosition = 0.8475678144032581f;
              return p;
          }() },
        { "Agent Trap 20261108-07", "TRAP", [] {
              auto p = baseParams();
              p.crush = 0.41422237028370956f;
              p.drive = 0.6634048280372933f;
              p.filterDestructive = true;
              p.tapeWobble = 0.5712902972324578f;
              p.wtPosition = 0.7195285597103594f;
              return p;
          }() },
    };
}

} // namespace wave9090::presets
