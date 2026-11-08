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
        { "Southside Sub (1108)", "TRAP", [] {
              auto p = baseParams();
              p.ampAttack = 0.002f;
              p.ampDecay = 0.4819918249426986f;
              p.ampRelease = 0.3115596580838629f;
              p.ampSustain = 0.9689785201382691f;
              p.crush = 0.1791917242682333f;
              p.drive = 0.4296380066566027f;
              p.filterCutoff = 0.2812231501728365f;
              p.filterRes = 0.12771040532528213f;
              p.glideMs = 94.98399066753608f;
              p.subLevel = 0.9694495726048286f;
              p.wtPosition = 0.5794495726048285f;
              return p;
          }() },
        { "Metro Sub (1108)", "TRAP", [] {
              auto p = baseParams();
              p.ampAttack = 0.002f;
              p.ampDecay = 0.48267146117230286f;
              p.ampRelease = 0.3119157361496321f;
              p.ampSustain = 0.9692024387932331f;
              p.crush = 0.17832064243215362f;
              p.drive = 0.4290909207836071f;
              p.filterCutoff = 0.28150304849154145f;
              p.filterRes = 0.1272727366268857f;
              p.glideMs = 94.99974397370916f;
              p.subLevel = 0.9698946701870401f;
              p.wtPosition = 0.5798946701870401f;
              return p;
          }() },
        { "Carti Bell Pluck (1108)", "TRAP", [] {
              auto p = baseParams();
              p.ampAttack = 0.001f;
              p.ampDecay = 0.08864092783478333f;
              p.ampRelease = 0.2525148875204855f;
              p.ampSustain = 0.00944179689612678f;
              p.filtEnvAmt = 0.841013926463122f;
              p.filterCutoff = 0.6818022461201585f;
              p.filterRes = 0.38656192143424856f;
              p.glideMs = 34.022813345420246f;
              p.stereoWidth = 0.22f;
              p.wtPosition = 0.7206436094006069f;
              return p;
          }() },
        { "Nardo Bell Pluck (1108)", "TRAP", [] {
              auto p = baseParams();
              p.ampAttack = 0.001f;
              p.ampDecay = 0.08927909749955121f;
              p.ampRelease = 0.2529732420068493f;
              p.ampSustain = 0.009543240144842827f;
              p.filtEnvAmt = 0.8417074875930448f;
              p.filterCutoff = 0.6819290501810535f;
              p.filterRes = 0.38603337181187275f;
              p.glideMs = 34.03841847084351f;
              p.stereoWidth = 0.22f;
              p.wtPosition = 0.7212165525085615f;
              return p;
          }() },
        { "Slime Slide (1108)", "TRAP", [] {
              auto p = baseParams();
              p.ampDecay = 0.4248944739785155f;
              p.ampSustain = 0.7295938500470851f;
              p.detuneCents = -10.876864595899901f;
              p.drive = 0.4868408557833388f;
              p.filterCutoff = 0.4219923125588564f;
              p.filterRes = 0.35547268462667103f;
              p.subLevel = 0.5418362818256853f;
              p.wtPosition = 0.7618362818256853f;
              return p;
          }() },
        { "Travis Hook (1108)", "TRAP", [] {
              auto p = baseParams();
              p.ampSustain = 0.6895980407622736f;
              p.filterCutoff = 0.5919975509528419f;
              p.glideMs = 105.06544298195247f;
              p.stereoWidth = 0.38f;
              p.tapeWobble = 0.12f;
              p.wtPosition = 0.7023413471549386f;
              p.wtScan = 0.18f;
              return p;
          }() },
        { "Phonk Haze (1108)", "TRAP", [] {
              auto p = baseParams();
              p.crush = 0.44383646907285906f;
              p.drive = 0.6453337489323754f;
              p.filterCutoff = 0.3919272046213093f;
              p.filterDestructive = true;
              p.subLevel = 0.6132026631732711f;
              p.tapeWobble = 0.22f;
              p.wtPosition = 0.6532026631732711f;
              return p;
          }() },
        { "Drill Cut (1108)", "TRAP", [] {
              auto p = baseParams();
              p.ampDecay = 0.13714216273537344f;
              p.ampSustain = 0.00925678538872581f;
              p.drive = 0.44357595280692086f;
              p.filtEnvAmt = 0.7250653011171927f;
              p.filterCutoff = 0.5515709817359074f;
              p.filterRes = 0.5228607622455367f;
              p.wtPosition = 0.8348433313547899f;
              return p;
          }() },
    };
}

} // namespace wave9090::presets
