#include "Presets/FactoryPresets.h"

#include <vector>

namespace wave9090::presets
{

namespace
{
SynthParams baseParams()
{
    SynthParams p;
    p.outputGainDb = -4.0f;
    return p;
}
} // namespace

std::string_view categoryUiHeading (std::string_view category) noexcept
{
    if (category == "808")
        return "808 / Sub Bass";
    if (category == "BASS")
        return "Bass";
    if (category == "LEAD")
        return "Lead";
    if (category == "PLUCK")
        return "Pluck";
    if (category == "PAD")
        return "Pad";
    if (category == "KEYS")
        return "Keys";
    if (category == "FX")
        return "FX / Destroyed";
    return category;
}

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = {
        // —— 808 / Sub Bass ——
        { "Boulevard 808", "808", [] {
              auto p = baseParams();
              p.wtPosition = 0.04f;
              p.filterCutoff = 0.22f;
              p.filterRes = 0.14f;
              p.filterDestructive = true;
              p.subLevel = 1.0f;
              p.drive = 0.52f;
              p.crush = 0.12f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.38f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.32f;
              p.outputGainDb = -5.5f;
              return p;
          }() },
        { "Cinder Sub", "808", [] {
              auto p = baseParams();
              p.wtPosition = 0.08f;
              p.filterCutoff = 0.26f;
              p.subLevel = 0.95f;
              p.drive = 0.38f;
              p.crush = 0.1f;
              p.filterDestructive = false;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.55f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.45f;
              p.tapeWobble = 0.08f;
              p.outputGainDb = -6.0f;
              return p;
          }() },
        { "Vault 909", "808", [] {
              auto p = baseParams();
              p.wtPosition = 0.12f;
              p.wtScan = 0.06f;
              p.filterCutoff = 0.3f;
              p.filterRes = 0.2f;
              p.subLevel = 1.0f;
              p.drive = 0.62f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.48f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.22f;
              p.outputGainDb = -5.0f;
              return p;
          }() },
        { "Lowgrave 808", "808", [] {
              auto p = baseParams();
              p.wtPosition = 0.02f;
              p.filterCutoff = 0.18f;
              p.filterRes = 0.08f;
              p.subLevel = 1.0f;
              p.drive = 0.42f;
              p.crush = 0.1f;
              p.filterDestructive = false;
              p.detuneCents = -5.0f;
              p.ampDecay = 0.62f;
              p.ampRelease = 0.5f;
              p.outputGainDb = -7.0f;
              return p;
          }() },
        { "Southside Sub", "808", [] {
              auto p = baseParams();
              p.wtPosition = 0.06f;
              p.filterCutoff = 0.24f;
              p.subLevel = 0.88f;
              p.drive = 0.44f;
              p.detuneCents = -7.0f;
              p.ampAttack = 0.003f;
              p.ampDecay = 0.35f;
              p.ampSustain = 0.12f;
              p.ampRelease = 0.28f;
              p.outputGainDb = -5.5f;
              return p;
          }() },
        // —— Bass ——
        { "Trap Furnace", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.22f;
              p.filterCutoff = 0.38f;
              p.filterRes = 0.28f;
              p.subLevel = 0.72f;
              p.drive = 0.58f;
              p.ampDecay = 0.28f;
              p.ampRelease = 0.35f;
              p.outputGainDb = -4.5f;
              return p;
          }() },
        { "Phonk Molasses", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.18f;
              p.filterCutoff = 0.32f;
              p.filterRes = 0.35f;
              p.tapeWobble = 0.28f;
              p.drive = 0.5f;
              p.crush = 0.18f;
              p.ampAttack = 0.008f;
              p.ampDecay = 0.42f;
              p.ampSustain = 0.55f;
              p.outputGainDb = -4.0f;
              return p;
          }() },
        { "Dirt Bassline", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.28f;
              p.filterCutoff = 0.42f;
              p.filterDestructive = true;
              p.drive = 0.65f;
              p.crush = 0.35f;
              p.subLevel = 0.6f;
              p.ampDecay = 0.22f;
              p.outputGainDb = -6.0f;
              return p;
          }() },
        { "Memphis Low", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.34f;
              p.tapeWobble = 0.42f;
              p.filterCutoff = 0.36f;
              p.stereoWidth = 0.22f;
              p.ampAttack = 0.012f;
              p.ampDecay = 0.35f;
              p.ampSustain = 0.68f;
              p.outputGainDb = -4.0f;
              return p;
          }() },
        { "Cinder Block", "BASS", [] {
              auto p = baseParams();
              p.wtPosition = 0.15f;
              p.filterCutoff = 0.3f;
              p.filterRes = 0.42f;
              p.subLevel = 0.85f;
              p.drive = 0.55f;
              p.ampDecay = 0.18f;
              p.ampSustain = 0.35f;
              p.ampRelease = 0.25f;
              p.outputGainDb = -5.0f;
              return p;
          }() },
        // —— Lead ——
        { "Phonk Razor", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.58f;
              p.wtScan = 0.22f;
              p.filterCutoff = 0.52f;
              p.filterRes = 0.38f;
              p.tapeWobble = 0.35f;
              p.detuneCents = 11.0f;
              p.drive = 0.35f;
              p.ampDecay = 0.22f;
              p.outputGainDb = -3.5f;
              return p;
          }() },
        { "Night Stab", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.72f;
              p.filterCutoff = 0.48f;
              p.filterRes = 0.48f;
              p.drive = 0.38f;
              p.filterDestructive = true;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.14f;
              p.ampSustain = 0.08f;
              p.outputGainDb = -4.0f;
              return p;
          }() },
        { "Hood Lead", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.62f;
              p.wtScan = 0.15f;
              p.filterCutoff = 0.55f;
              p.filterRes = 0.32f;
              p.stereoWidth = 0.38f;
              p.glideMs = 35.0f;
              p.ampDecay = 0.28f;
              p.outputGainDb = -3.5f;
              return p;
          }() },
        { "Tape Shriek", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.66f;
              p.tapeWobble = 0.52f;
              p.filterCutoff = 0.62f;
              p.filterRes = 0.45f;
              p.drive = 0.42f;
              p.crush = 0.2f;
              p.ampAttack = 0.004f;
              p.ampDecay = 0.18f;
              p.outputGainDb = -4.5f;
              return p;
          }() },
        { "Vault Siren", "LEAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.78f;
              p.wtScan = 0.35f;
              p.filterCutoff = 0.58f;
              p.filterRes = 0.55f;
              p.filtEnvAmt = 0.72f;
              p.filtDecay = 0.22f;
              p.ampDecay = 0.35f;
              p.outputGainDb = -4.0f;
              return p;
          }() },
        // —— Pluck ——
        { "Crisp Trap", "PLUCK", [] {
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
              p.outputGainDb = -3.0f;
              return p;
          }() },
        { "Bell Pluck", "PLUCK", [] {
              auto p = baseParams();
              p.wtPosition = 0.74f;
              p.filterCutoff = 0.7f;
              p.filterRes = 0.35f;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.12f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.22f;
              p.detuneCents = 14.0f;
              p.outputGainDb = -3.5f;
              return p;
          }() },
        { "Dark Ping", "PLUCK", [] {
              auto p = baseParams();
              p.wtPosition = 0.52f;
              p.filterCutoff = 0.48f;
              p.filterRes = 0.52f;
              p.filtEnvAmt = 0.9f;
              p.filtDecay = 0.08f;
              p.ampDecay = 0.07f;
              p.ampRelease = 0.12f;
              p.outputGainDb = -3.5f;
              return p;
          }() },
        { "Memphis Pluck", "PLUCK", [] {
              auto p = baseParams();
              p.wtPosition = 0.4f;
              p.tapeWobble = 0.32f;
              p.filterCutoff = 0.58f;
              p.crush = 0.12f;
              p.ampDecay = 0.11f;
              p.ampRelease = 0.2f;
              p.outputGainDb = -3.5f;
              return p;
          }() },
        { "Vault Twang", "PLUCK", [] {
              auto p = baseParams();
              p.wtPosition = 0.68f;
              p.wtScan = 0.08f;
              p.filterCutoff = 0.66f;
              p.filterRes = 0.48f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.15f;
              p.filtEnvAmt = 0.75f;
              p.filtDecay = 0.12f;
              p.outputGainDb = -3.5f;
              return p;
          }() },
        // —— Pad ——
        { "Smoke Pad", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.3f;
              p.wtScan = 0.18f;
              p.tapeWobble = 0.48f;
              p.ampAttack = 0.28f;
              p.ampRelease = 1.1f;
              p.filterCutoff = 0.44f;
              p.stereoWidth = 0.55f;
              p.outputGainDb = -5.0f;
              return p;
          }() },
        { "Tape Haze", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.38f;
              p.tapeWobble = 0.62f;
              p.filterCutoff = 0.5f;
              p.ampAttack = 0.45f;
              p.ampDecay = 0.55f;
              p.ampSustain = 0.82f;
              p.ampRelease = 1.25f;
              p.outputGainDb = -5.5f;
              return p;
          }() },
        { "Underground Air", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.25f;
              p.filterCutoff = 0.52f;
              p.filterRes = 0.22f;
              p.ampAttack = 0.35f;
              p.ampSustain = 0.75f;
              p.ampRelease = 0.95f;
              p.stereoWidth = 0.48f;
              p.outputGainDb = -5.0f;
              return p;
          }() },
        { "Vault Wash", "PAD", [] {
              auto p = baseParams();
              p.wtPosition = 0.42f;
              p.wtScan = 0.28f;
              p.tapeWobble = 0.35f;
              p.filterCutoff = 0.46f;
              p.ampAttack = 0.52f;
              p.ampRelease = 1.4f;
              p.outputGainDb = -6.0f;
              return p;
          }() },
        // —— Keys ——
        { "Memphis Keys", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.4f;
              p.tapeWobble = 0.38f;
              p.stereoWidth = 0.42f;
              p.wtScan = 0.12f;
              p.filterCutoff = 0.55f;
              p.ampDecay = 0.35f;
              p.ampSustain = 0.62f;
              p.outputGainDb = -4.0f;
              return p;
          }() },
        { "Dusty Rhodes", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.48f;
              p.tapeWobble = 0.45f;
              p.crush = 0.08f;
              p.filterCutoff = 0.58f;
              p.filterRes = 0.18f;
              p.ampAttack = 0.006f;
              p.ampDecay = 0.42f;
              p.ampSustain = 0.55f;
              p.outputGainDb = -4.5f;
              return p;
          }() },
        { "Trap Celeste", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.72f;
              p.filterCutoff = 0.68f;
              p.filterRes = 0.22f;
              p.ampAttack = 0.004f;
              p.ampDecay = 0.28f;
              p.ampSustain = 0.4f;
              p.detuneCents = 8.0f;
              p.outputGainDb = -4.0f;
              return p;
          }() },
        { "Dungeon Keys", "KEYS", [] {
              auto p = baseParams();
              p.wtPosition = 0.56f;
              p.filterCutoff = 0.42f;
              p.tapeWobble = 0.55f;
              p.crush = 0.15f;
              p.ampDecay = 0.38f;
              p.ampSustain = 0.5f;
              p.stereoWidth = 0.35f;
              p.outputGainDb = -4.5f;
              return p;
          }() },
        // —— FX / Destroyed ——
        { "Destroyed Lead", "FX", [] {
              auto p = baseParams();
              p.wtPosition = 0.82f;
              p.filterCutoff = 0.45f;
              p.filterRes = 0.38f;
              p.filterDestructive = true;
              p.drive = 0.68f;
              p.crush = 0.38f;
              p.detuneCents = 6.0f;
              p.ampAttack = 0.002f;
              p.ampDecay = 0.22f;
              p.outputGainDb = -8.0f;
              return p;
          }() },
        { "Bit Crusher", "FX", [] {
              auto p = baseParams();
              p.wtPosition = 0.52f;
              p.crush = 0.62f;
              p.drive = 0.58f;
              p.filterDestructive = true;
              p.filterCutoff = 0.38f;
              p.detuneCents = -4.0f;
              p.ampDecay = 0.16f;
              p.outputGainDb = -7.0f;
              return p;
          }() },
        { "Wrecked Bell", "FX", [] {
              auto p = baseParams();
              p.wtPosition = 0.86f;
              p.filterCutoff = 0.72f;
              p.drive = 0.68f;
              p.crush = 0.42f;
              p.filterDestructive = true;
              p.detuneCents = -18.0f;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.22f;
              p.outputGainDb = -7.0f;
              return p;
          }() },
        { "Scrape FX", "FX", [] {
              auto p = baseParams();
              p.wtPosition = 0.92f;
              p.wtScan = 0.55f;
              p.filterCutoff = 0.28f;
              p.filterRes = 0.65f;
              p.drive = 0.88f;
              p.crush = 0.48f;
              p.filterDestructive = true;
              p.ampAttack = 0.001f;
              p.ampDecay = 0.35f;
              p.filtEnvAmt = 0.95f;
              p.filtDecay = 0.12f;
              p.outputGainDb = -9.0f;
              return p;
          }() },
        { "Phantom Rattle", "FX", [] {
              auto p = baseParams();
              p.wtPosition = 0.64f;
              p.tapeWobble = 0.48f;
              p.drive = 0.48f;
              p.crush = 0.28f;
              p.filterDestructive = true;
              p.filterCutoff = 0.42f;
              p.detuneCents = 9.0f;
              p.ampAttack = 0.003f;
              p.ampDecay = 0.24f;
              p.stereoWidth = 0.45f;
              p.outputGainDb = -7.0f;
              return p;
          }() },
    };
    return kPresets;
}

} // namespace wave9090::presets