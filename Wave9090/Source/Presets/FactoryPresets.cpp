#include "Presets/ActiveAgentSeeds.h"
#include "Presets/FactoryPresets.h"
#include "Presets/PresetAgent.h"

#include <cstdio>
#include <vector>

namespace wave9090::presets
{

namespace
{
SynthParams baseParams()
{
    return SynthParams {};
}

SynthParams mutateFromSeed (const SynthParams& seed, std::size_t index) noexcept
{
    using agent::clamp;
    using agent::rand01;
    std::uint32_t state = 0x9090u ^ static_cast<std::uint32_t> (index * 2654435761ull);
    SynthParams p = seed;

    auto jitter = [&] (float& v, float span, float lo, float hi) {
        v = clamp (v + (rand01 (state) - 0.5f) * span, lo, hi);
    };

    jitter (p.wtPosition, 0.28f, 0.0f, 1.0f);
    jitter (p.wtScan, 0.22f, 0.0f, 1.0f);
    jitter (p.filterCutoff, 0.18f, 0.05f, 1.0f);
    jitter (p.filterRes, 0.2f, 0.0f, 0.95f);
    jitter (p.drive, 0.25f, 0.0f, 1.0f);
    jitter (p.crush, 0.22f, 0.0f, 1.0f);
    jitter (p.subLevel, 0.2f, 0.0f, 1.0f);
    jitter (p.detuneCents, 24.0f, -50.0f, 50.0f);
    jitter (p.glideMs, 60.0f, 0.0f, 250.0f);
    jitter (p.tapeWobble, 0.18f, 0.0f, 0.95f);
    jitter (p.ampAttack, 0.015f, 0.0005f, 0.6f);
    jitter (p.ampDecay, 0.2f, 0.02f, 1.2f);
    jitter (p.ampSustain, 0.12f, 0.0f, 1.0f);
    jitter (p.ampRelease, 0.35f, 0.05f, 2.0f);
    jitter (p.filtEnvAmt, 0.2f, 0.0f, 1.0f);
    jitter (p.stereoWidth, 0.22f, 0.0f, 1.0f);

    if (rand01 (state) > 0.82f)
        p.filterDestructive = ! p.filterDestructive;

    return p;
}
} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = [] {
        const std::vector<FactoryPreset> curated = {
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

        // —— TRAP (16) —— VST Preset Agent / new trap wavetables 64–113
        { "Southside 808 Trap", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.58f;
              p.filterCutoff = 0.28f;
              p.subLevel = 0.98f;
              p.drive = 0.5f;
              p.ampSustain = 0.96f;
              return p;
          }() },
        { "Metro Boomin 808", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.62f;
              p.filterCutoff = 0.26f;
              p.subLevel = 1.0f;
              p.glideMs = 90.0f;
              p.ampDecay = 0.55f;
              return p;
          }() },
        { "Zaytoven Pluck Trap", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.71f;
              p.filterCutoff = 0.68f;
              p.ampDecay = 0.09f;
              p.ampSustain = 0.0f;
              p.filtEnvAmt = 0.82f;
              p.glideMs = 32.0f;
              return p;
          }() },
        { "Drill Reese WAVE", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.78f;
              p.detuneCents = -11.0f;
              p.filterCutoff = 0.42f;
              p.drive = 0.55f;
              p.crush = 0.2f;
              return p;
          }() },
        { "NY Drill Stab", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.85f;
              p.filterCutoff = 0.55f;
              p.filterRes = 0.5f;
              p.ampDecay = 0.14f;
              p.ampSustain = 0.0f;
              p.drive = 0.45f;
              return p;
          }() },
        { "Memphis Phonk Trap", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.66f;
              p.crush = 0.48f;
              p.drive = 0.65f;
              p.filterDestructive = true;
              p.tapeWobble = 0.25f;
              return p;
          }() },
        { "Carti Slide Lead", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.74f;
              p.wtScan = 0.22f;
              p.glideMs = 110.0f;
              p.filterCutoff = 0.62f;
              p.stereoWidth = 0.35f;
              return p;
          }() },
        { "Gunna Float Trap", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.69f;
              p.filterCutoff = 0.7f;
              p.stereoWidth = 0.4f;
              p.ampAttack = 0.008f;
              p.ampSustain = 0.75f;
              return p;
          }() },
        { "Future Dark Pluck", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.73f;
              p.filterCutoff = 0.64f;
              p.ampDecay = 0.075f;
              p.filtEnvAmt = 0.88f;
              p.subLevel = 0.25f;
              return p;
          }() },
        { "Trap Bell Memphis", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.81f;
              p.drive = 0.6f;
              p.crush = 0.35f;
              p.filterDestructive = true;
              p.detuneCents = 9.0f;
              return p;
          }() },
        { "808 Glide Trap II", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.59f;
              p.glideMs = 160.0f;
              p.subLevel = 0.92f;
              p.filterCutoff = 0.3f;
              return p;
          }() },
        { "Trap Hihat Synth", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.88f;
              p.filterCutoff = 0.82f;
              p.ampDecay = 0.04f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.08f;
              return p;
          }() },
        { "Offset Type Beat", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.76f;
              p.stereoWidth = 0.05f;
              p.drive = 0.38f;
              p.filterCutoff = 0.48f;
              p.ampSustain = 0.7f;
              return p;
          }() },
        { "Lil Baby Arp Trap", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.72f;
              p.wtScan = 0.35f;
              p.filtEnvAmt = 0.7f;
              p.ampDecay = 0.11f;
              p.glideMs = 45.0f;
              return p;
          }() },
        { "Travis Filter Trap", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.67f;
              p.filterCutoff = 0.38f;
              p.filterRes = 0.62f;
              p.filtEnvAmt = 0.92f;
              p.drive = 0.28f;
              return p;
          }() },
        { "Trap Night Pad", "TRAP", [] {
              auto p = baseParams();
              p.wtPosition = 0.64f;
              p.filterCutoff = 0.5f;
              p.ampAttack = 0.35f;
              p.ampSustain = 0.88f;
              p.tapeWobble = 0.18f;
              return p;
          }() },
        };

        std::vector<FactoryPreset> seeds = curated;
        const auto agentSeeds = getActiveAgentSeeds();
        seeds.insert (seeds.end(), agentSeeds.begin(), agentSeeds.end());

        std::vector<FactoryPreset> bank;
        bank.reserve (agent::kFactoryPresetCount);
        for (const auto& entry : seeds)
            bank.push_back (entry);

        for (std::size_t i = seeds.size(); i < agent::kFactoryPresetCount; ++i)
        {
            const auto& seed = seeds[i % seeds.size()];
            char nameBuf[72];
            std::snprintf (nameBuf,
                           sizeof (nameBuf),
                           "%s %03zu",
                           seed.category.c_str(),
                           i + 1);
            bank.push_back (
                FactoryPreset { nameBuf, seed.category, mutateFromSeed (seed.params, i) });
        }
        return bank;
    }();

    return kPresets;
}

} // namespace wave9090::presets
