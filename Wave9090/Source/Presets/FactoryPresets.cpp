#include "Presets/FactoryPresets.h"

#include <vector>

namespace wave9090::presets
{

namespace
{
SynthParams baseParams()
{
    SynthParams p;
    return p;
}
} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = {
        { "Slide 808", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.08f;
              p.filterCutoff = 0.35f;
              p.filterRes = 0.15f;
              p.subLevel = 0.95f;
              p.drive = 0.45f;
              p.crush = 0.25f;
              p.filterDestructive = true;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.35f;
              p.ampSustain = 0.95f;
              p.ampRelease = 0.25f;
              return p;
          }() },
        { "Night Drive Lead", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.62f;
              p.wtScan = 0.35f;
              p.filterCutoff = 0.58f;
              p.tapeWobble = 0.4f;
              p.detuneCents = 8.0f;
              return p;
          }() },
        { "Dark Trap Pluck", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.48f;
              p.filterCutoff = 0.66f;
              p.filterRes = 0.45f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.12f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.22f;
              p.filtEnvAmt = 0.75f;
              return p;
          }() },
        { "Funeral Bell", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.78f;
              p.filterCutoff = 0.82f;
              p.filterRes = 0.35f;
              p.ampDecay = 0.55f;
              p.ampRelease = 0.8f;
              return p;
          }() },
        { "Memphis Cowbell", "PHONK", [] {
              auto p = baseParams();
              p.wtPosition = 0.55f;
              p.filterCutoff = 0.71f;
              p.drive = 0.65f;
              p.crush = 0.4f;
              p.filterDestructive = true;
              p.detuneCents = -14.0f;
              return p;
          }() },
        { "Smoke Keys", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.42f;
              p.tapeWobble = 0.55f;
              p.stereoWidth = 0.35f;
              p.wtScan = 0.15f;
              return p;
          }() },
        { "Drill Stab", "DRILL", [] {
              auto p = baseParams();
              p.wtPosition = 0.88f;
              p.filterCutoff = 0.52f;
              p.filterRes = 0.55f;
              p.drive = 0.5f;
              p.filterDestructive = true;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.18f;
              p.ampSustain = 0.1f;
              return p;
          }() },
        { "Trap Pad", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.3f;
              p.wtScan = 0.22f;
              p.tapeWobble = 0.65f;
              p.ampAttack = 0.35f;
              p.ampRelease = 1.2f;
              p.filterCutoff = 0.48f;
              return p;
          }() },
    };
    return kPresets;
}

} // namespace wave9090::presets
