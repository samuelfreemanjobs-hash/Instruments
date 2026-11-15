#include "Presets/FactoryPresets.h"

#include <vector>

namespace wave9090::presets
{

namespace
{
SynthParams baseParams()
{
    SynthParams p;
    p.voiceGain = 0.58f;
    p.ampAttack = 0.004f;
    return p;
}
} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = {
        { "Trap 808", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.05f;
              p.filterCutoff = 0.28f;
              p.filterRes = 0.12f;
              p.subLevel = 1.0f;
              p.drive = 0.55f;
              p.crush = 0.18f;
              p.filterDestructive = true;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.42f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.28f;
              p.pitchEnvDepthCents = -360.0f;
              p.pitchEnvDecay = 0.35f;
              p.voiceGain = 0.78f;
              return p;
          }() },
        { "Phonk Lead", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.58f;
              p.wtScan = 0.22f;
              p.filterCutoff = 0.52f;
              p.filterRes = 0.38f;
              p.tapeWobble = 0.35f;
              p.detuneCents = 11.0f;
              p.drive = 0.35f;
              p.ampDecay = 0.22f;
              p.voiceGain = 0.48f;
              return p;
          }() },
        { "Crisp Pluck", "PLUCK", [] {
              auto p = baseParams();
              p.wtPosition = 0.46f;
              p.filterCutoff = 0.62f;
              p.filterRes = 0.4f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.09f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.18f;
              p.filtEnvAmt = 0.82f;
              p.filtAttack = 0.001f;
              p.filtDecay = 0.14f;
              p.voiceGain = 0.55f;
              return p;
          }() },
        { "Dark Bell", "BELL", [] {
              auto p = baseParams();
              p.wtPosition = 0.78f;
              p.filterCutoff = 0.76f;
              p.filterRes = 0.32f;
              p.ampDecay = 0.48f;
              p.ampRelease = 0.75f;
              p.voiceGain = 0.46f;
              return p;
          }() },
        { "Phonk Cowbell", "BELL", [] {
              auto p = baseParams();
              p.wtPosition = 0.62f;
              p.filterCutoff = 0.68f;
              p.drive = 0.58f;
              p.crush = 0.32f;
              p.filterDestructive = true;
              p.detuneCents = -11.0f;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.16f;
              p.ampSustain = 0.05f;
              p.voiceGain = 0.5f;
              return p;
          }() },
        { "Memphis Keys", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.4f;
              p.tapeWobble = 0.38f;
              p.stereoWidth = 0.42f;
              p.wtScan = 0.12f;
              p.filterCutoff = 0.55f;
              p.voiceGain = 0.44f;
              return p;
          }() },
        { "Underground Stab", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.85f;
              p.filterCutoff = 0.48f;
              p.filterRes = 0.5f;
              p.drive = 0.42f;
              p.filterDestructive = true;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.14f;
              p.ampSustain = 0.08f;
              p.voiceGain = 0.52f;
              return p;
          }() },
        { "Tape Pad", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.3f;
              p.wtScan = 0.18f;
              p.tapeWobble = 0.48f;
              p.ampAttack = 0.28f;
              p.ampRelease = 1.1f;
              p.filterCutoff = 0.44f;
              p.voiceGain = 0.4f;
              return p;
          }() },
    };
    return kPresets;
}

} // namespace wave9090::presets