#include "Presets/FactoryPresets.h"

#include <vector>

namespace wave9090::presets
{

namespace
{
SynthParams baseParams()
{
    return SynthParams {};
}
} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = {
        // —— BASS (5) ——
        { "Sub-Zero 808", "BASS", [] {
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
        { "Phonk Sub 808 II", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.12f;
              p.wtScan = 0.0f;
              p.filterCutoff = 0.3f;
              p.subLevel = 1.0f;
              p.drive = 0.55f;
              p.crush = 0.3f;
              p.filterDestructive = true;
              p.ampSustain = 0.98f;
              return p;
          }() },
        { "Glide 808 Weight", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.05f;
              p.filterCutoff = 0.28f;
              p.subLevel = 0.9f;
              p.glideMs = 180.0f;
              p.ampDecay = 0.5f;
              p.ampSustain = 0.92f;
              return p;
          }() },
        { "Memphis Sub Crusher", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.18f;
              p.filterCutoff = 0.32f;
              p.drive = 0.7f;
              p.crush = 0.45f;
              p.filterDestructive = true;
              p.tapeWobble = 0.2f;
              p.subLevel = 0.85f;
              return p;
          }() },
        { "Deep 808 Sine", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.02f;
              p.filterCutoff = 0.22f;
              p.filterRes = 0.08f;
              p.subLevel = 0.98f;
              p.drive = 0.15f;
              p.ampAttack = 0.003f;
              p.ampSustain = 0.99f;
              return p;
          }() },

        // —— PLUCK / BELL (6) ——
        { "Sinister Pluck", "PLUCK", [] {
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
        { "Plugg Pluck 145", "PLUCK", [] {
              auto p = baseParams();
              p.wtPosition = 0.44f;
              p.filterCutoff = 0.7f;
              p.filterRes = 0.32f;
              p.detuneCents = 6.0f;
              p.ampDecay = 0.1f;
              p.ampSustain = 0.0f;
              p.filtEnvAmt = 0.78f;
              p.glideMs = 40.0f;
              return p;
          }() },
        { "Plugg Pluck 155", "PLUCK", [] {
              auto p = baseParams();
              p.wtPosition = 0.5f;
              p.filterCutoff = 0.64f;
              p.filterRes = 0.4f;
              p.ampDecay = 0.08f;
              p.filtEnvAmt = 0.82f;
              p.stereoWidth = 0.22f;
              return p;
          }() },
        { "Phonk Cowbell", "BELL", [] {
              auto p = baseParams();
              p.wtPosition = 0.55f;
              p.filterCutoff = 0.71f;
              p.drive = 0.65f;
              p.crush = 0.4f;
              p.filterDestructive = true;
              p.detuneCents = -14.0f;
              return p;
          }() },
        { "DL002 Drift Cowbell II", "BELL", [] {
              auto p = baseParams();
              p.wtPosition = 0.58f;
              p.wtScan = 0.08f;
              p.filterCutoff = 0.68f;
              p.crush = 0.5f;
              p.filterDestructive = true;
              p.tapeWobble = 0.15f;
              p.detuneCents = 10.0f;
              return p;
          }() },
        { "Screw Bell Slow", "BELL", [] {
              auto p = baseParams();
              p.wtPosition = 0.72f;
              p.filterCutoff = 0.55f;
              p.tapeWobble = 0.55f;
              p.ampAttack = 0.008f;
              p.ampDecay = 0.65f;
              p.ampSustain = 0.15f;
              p.ampRelease = 0.9f;
              return p;
          }() },

        // —— LEAD (5) ——
        { "Drift Lead", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.62f;
              p.wtScan = 0.35f;
              p.filterCutoff = 0.58f;
              p.tapeWobble = 0.4f;
              p.detuneCents = 8.0f;
              return p;
          }() },
        { "Metallic Stab", "LEAD", [] {
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
        { "Terminal Cyber Stab", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.75f;
              p.filterCutoff = 0.78f;
              p.filterRes = 0.28f;
              p.filterDestructive = false;
              p.drive = 0.18f;
              p.tapeWobble = 0.05f;
              p.ampDecay = 0.14f;
              p.filtEnvAmt = 0.6f;
              return p;
          }() },
        { "Rage Screech Lead", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.92f;
              p.filterCutoff = 0.45f;
              p.filterRes = 0.68f;
              p.drive = 0.72f;
              p.crush = 0.35f;
              p.filterDestructive = true;
              p.filtEnvAmt = 0.9f;
              return p;
          }() },
        { "Space Lead Mono", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.58f;
              p.wtScan = 0.2f;
              p.stereoWidth = 0.02f;
              p.tapeWobble = 0.22f;
              p.filterCutoff = 0.72f;
              return p;
          }() },

        // —— KEYS (4) ——
        { "Haunted Keys", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.42f;
              p.tapeWobble = 0.55f;
              p.stereoWidth = 0.35f;
              p.wtScan = 0.15f;
              return p;
          }() },
        { "Phonkwave Keys", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.38f;
              p.crush = 0.3f;
              p.drive = 0.35f;
              p.tapeWobble = 0.45f;
              p.filterCutoff = 0.6f;
              return p;
          }() },
        { "Screw Keys", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.35f;
              p.tapeWobble = 0.7f;
              p.filterCutoff = 0.48f;
              p.ampAttack = 0.02f;
              p.ampDecay = 0.45f;
              p.ampSustain = 0.55f;
              return p;
          }() },
        { "Cyber Funk Keys", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.52f;
              p.filterCutoff = 0.75f;
              p.filterRes = 0.2f;
              p.wtScan = 0.12f;
              p.stereoWidth = 0.28f;
              return p;
          }() },

        // —— PAD (4) ——
        { "Tape Pad", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.3f;
              p.wtScan = 0.22f;
              p.tapeWobble = 0.65f;
              p.ampAttack = 0.35f;
              p.ampRelease = 1.2f;
              p.filterCutoff = 0.48f;
              return p;
          }() },
        { "Phonk Pad Crush", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.28f;
              p.crush = 0.42f;
              p.drive = 0.4f;
              p.filterDestructive = true;
              p.ampAttack = 0.4f;
              p.ampSustain = 0.88f;
              return p;
          }() },
        { "Majestic Wide Pad", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.25f;
              p.stereoWidth = 0.55f;
              p.wtScan = 0.18f;
              p.ampAttack = 0.5f;
              p.ampRelease = 1.5f;
              p.filterCutoff = 0.55f;
              return p;
          }() },
        { "Drift Night Pad", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.4f;
              p.tapeWobble = 0.5f;
              p.wtScan = 0.28f;
              p.filterCutoff = 0.42f;
              p.ampAttack = 0.45f;
              p.ampSustain = 0.9f;
              return p;
          }() },
    };

    return kPresets;
}

} // namespace wave9090::presets
