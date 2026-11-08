#include "Presets/ActiveAgentSeeds.h"
#include "Presets/FactoryPresets.h"
#include "Presets/PresetAgent.h"

#include <cstdio>
#include <vector>

namespace pluggva::presets
{

namespace
{
SynthParams base()
{
    return SynthParams {};
}

SynthParams mutateFromSeed (const SynthParams& seed, std::size_t index) noexcept
{
    using agent::clamp;
    using agent::rand01;
    std::uint32_t state = 0x504C5641u ^ static_cast<std::uint32_t> (index * 2654435761ull);
    SynthParams p = seed;

    auto jitter = [&] (float& v, float span, float lo, float hi) {
        v = clamp (v + (rand01 (state) - 0.5f) * span, lo, hi);
    };

    jitter (p.osc1Level, 0.15f, 0.2f, 1.0f);
    jitter (p.osc2Level, 0.15f, 0.0f, 1.0f);
    jitter (p.osc2DetuneCents, 18.0f, -40.0f, 40.0f);
    jitter (p.pulseWidth, 0.18f, 0.08f, 0.92f);
    jitter (p.subLevel, 0.2f, 0.0f, 1.0f);
    jitter (p.unisonAmount, 0.22f, 0.0f, 1.0f);
    jitter (p.unisonSpreadCents, 16.0f, 4.0f, 48.0f);
    jitter (p.filterCutoff, 0.2f, 0.05f, 1.0f);
    jitter (p.filterRes, 0.18f, 0.0f, 0.92f);
    jitter (p.filtEnvAmt, 0.22f, 0.0f, 1.0f);
    jitter (p.drive, 0.2f, 0.0f, 1.0f);
    jitter (p.crush, 0.15f, 0.0f, 0.85f);
    jitter (p.chorus, 0.2f, 0.0f, 0.95f);
    jitter (p.drift, 0.12f, 0.0f, 0.55f);
    jitter (p.glideMs, 55.0f, 0.0f, 220.0f);
    jitter (p.stereoWidth, 0.2f, 0.0f, 1.0f);

    if (rand01 (state) > 0.88f)
        p.osc2Octave = clamp (p.osc2Octave + (rand01 (state) > 0.5f ? 1 : -1), -1, 2);
    if (rand01 (state) > 0.9f)
        p.osc1Wave = static_cast<Waveform> ((static_cast<int> (p.osc1Wave) + 1) % 4);
    if (rand01 (state) > 0.9f)
        p.osc2Wave = static_cast<Waveform> ((static_cast<int> (p.osc2Wave) + 2) % 4);

    return p;
}
} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = [] {
        const std::vector<FactoryPreset> curated = {
        // —— PLUGG (8) ——
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
        { "Plugg Pluck 145", "PLUGG", [] {
              auto p = base();
              p.osc2Octave = 1;
              p.osc2DetuneCents = 9.0f;
              p.filtEnvAmt = 0.76f;
              p.ampDecay = 0.1f;
              p.ampSustain = 0.0f;
              p.glideMs = 38.0f;
              p.chorus = 0.32f;
              return p;
          }() },
        { "Plugg Pluck 155", "PLUGG", [] {
              auto p = base();
              p.osc2Wave = Waveform::square;
              p.osc2Octave = 1;
              p.filterCutoff = 0.64f;
              p.filtEnvAmt = 0.8f;
              p.ampDecay = 0.085f;
              p.filterRes = 0.38f;
              return p;
          }() },
        { "Plugg Pluck 160", "PLUGG", [] {
              auto p = base();
              p.osc1Wave = Waveform::pulse;
              p.pulseWidth = 0.35f;
              p.osc2Octave = 1;
              p.ampDecay = 0.075f;
              p.filtDecay = 0.25f;
              p.filtEnvAmt = 0.85f;
              return p;
          }() },
        { "Mono Space Lead", "PLUGG", [] {
              auto p = base();
              p.unisonAmount = 0.2f;
              p.stereoWidth = 0.05f;
              p.chorus = 0.15f;
              p.glideMs = 95.0f;
              p.drift = 0.18f;
              p.filterCutoff = 0.76f;
              return p;
          }() },
        { "Plugg Arp Hook", "PLUGG", [] {
              auto p = base();
              p.osc2Octave = 1;
              p.osc2DetuneCents = 14.0f;
              p.ampDecay = 0.09f;
              p.ampSustain = 0.0f;
              p.filtEnvAmt = 0.7f;
              p.glideMs = 25.0f;
              return p;
          }() },
        // —— TRAP (6) ——
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
        { "808 Trap Reese", "TRAP", [] {
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
        { "Detuned Reese II", "TRAP", [] {
              auto p = base();
              p.unisonAmount = 0.9f;
              p.unisonSpreadCents = 32.0f;
              p.osc2DetuneCents = -18.0f;
              p.subLevel = 0.65f;
              p.filterCutoff = 0.36f;
              p.drive = 0.35f;
              return p;
          }() },
        { "Trap Glide Lead 150", "TRAP", [] {
              auto p = base();
              p.unisonAmount = 0.5f;
              p.glideMs = 140.0f;
              p.filterCutoff = 0.62f;
              p.chorus = 0.38f;
              p.ampSustain = 0.65f;
              return p;
          }() },
        { "808 Glide Bass", "TRAP", [] {
              auto p = base();
              p.subLevel = 0.88f;
              p.filterCutoff = 0.3f;
              p.glideMs = 200.0f;
              p.ampSustain = 0.95f;
              p.drive = 0.3f;
              return p;
          }() },
        { "Metro Detuned Bell", "TRAP", [] {
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

        // —— PHONK (6) ——
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
        { "Phonkwave Reese Wide", "PHONK", [] {
              auto p = base();
              p.unisonAmount = 0.92f;
              p.unisonSpreadCents = 26.0f;
              p.drift = 0.5f;
              p.crush = 0.28f;
              p.subLevel = 0.7f;
              p.filterCutoff = 0.4f;
              return p;
          }() },
        { "Memphis Cowbell VA", "PHONK", [] {
              auto p = base();
              p.osc1Wave = Waveform::square;
              p.osc2Octave = 2;
              p.osc2DetuneCents = -7.0f;
              p.filterRes = 0.48f;
              p.filtEnvAmt = 0.75f;
              p.ampDecay = 0.15f;
              p.crush = 0.2f;
              return p;
          }() },
        { "Wide Phonk Pad", "PHONK", [] {
              auto p = base();
              p.crush = 0.32f;
              p.drift = 0.35f;
              p.stereoWidth = 0.5f;
              p.chorus = 0.5f;
              p.ampAttack = 0.38f;
              p.filterCutoff = 0.5f;
              return p;
          }() },
        { "Screw Bell Pluck", "PHONK", [] {
              auto p = base();
              p.osc1Wave = Waveform::triangle;
              p.drift = 0.55f;
              p.ampDecay = 0.55f;
              p.ampRelease = 0.85f;
              p.filterCutoff = 0.52f;
              return p;
          }() },

        // —— RAGE (4) ——
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
        { "Rage Pluck Fast", "RAGE", [] {
              auto p = base();
              p.filtEnvAmt = 0.88f;
              p.ampDecay = 0.065f;
              p.filterRes = 0.58f;
              p.drive = 0.48f;
              p.crush = 0.22f;
              return p;
          }() },
        { "Rage Screech II", "RAGE", [] {
              auto p = base();
              p.filterRes = 0.72f;
              p.drive = 0.68f;
              p.crush = 0.4f;
              p.filtEnvAmt = 0.92f;
              p.ampDecay = 0.07f;
              return p;
          }() },
        { "Cyber Funk Stab", "RAGE", [] {
              auto p = base();
              p.osc1Wave = Waveform::square;
              p.filterCutoff = 0.76f;
              p.filterRes = 0.25f;
              p.drive = 0.2f;
              p.ampDecay = 0.11f;
              p.chorus = 0.12f;
              return p;
          }() },

        // —— SOUTH / LANES (4) ——
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
        { "Screw Glide Chord", "SOUTH", [] {
              auto p = base();
              p.osc1Wave = Waveform::square;
              p.glideMs = 280.0f;
              p.drift = 0.48f;
              p.ampAttack = 0.015f;
              p.ampSustain = 0.62f;
              p.filterCutoff = 0.46f;
              return p;
          }() },
        { "French Filter Chord", "SOUTH", [] {
              auto p = base();
              p.osc1Wave = Waveform::saw;
              p.osc2Wave = Waveform::triangle;
              p.chorus = 0.55f;
              p.filtEnvAmt = 0.35f;
              p.filterCutoff = 0.66f;
              p.ampAttack = 0.018f;
              p.stereoWidth = 0.42f;
              return p;
          }() },
        { "Southern Portamento Saw", "SOUTH", [] {
              auto p = base();
              p.unisonAmount = 0.6f;
              p.glideMs = 190.0f;
              p.filterCutoff = 0.54f;
              p.chorus = 0.35f;
              return p;
          }() },

        // —— PAD / KEYS / PLUCK (4) ——
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
        { "Drift Night Pad", "PAD", [] {
              auto p = base();
              p.drift = 0.42f;
              p.ampAttack = 0.42f;
              p.ampRelease = 1.1f;
              p.filterCutoff = 0.44f;
              p.chorus = 0.48f;
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
        { "Casual Keys 2", "KEYS", [] {
              auto p = base();
              p.osc1Wave = Waveform::triangle;
              p.osc2Level = 0.4f;
              p.chorus = 0.5f;
              p.ampAttack = 0.012f;
              p.filterCutoff = 0.64f;
              return p;
          }() },

        // —— remaining PLUCK ——
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

        // —— TRAP (12) —— VST Preset Agent
        { "Wheezy Outta Here", "TRAP", [] {
              auto p = base();
              p.unisonAmount = 0.75f;
              p.filterCutoff = 0.54f;
              p.drive = 0.32f;
              p.glideMs = 95.0f;
              return p;
          }() },
        { "TM88 Dark Trap", "TRAP", [] {
              auto p = base();
              p.subLevel = 0.82f;
              p.filterCutoff = 0.34f;
              p.drive = 0.45f;
              p.ampDecay = 0.48f;
              return p;
          }() },
        { "Plugg Trap Reese", "TRAP", [] {
              auto p = base();
              p.unisonAmount = 0.88f;
              p.osc2DetuneCents = -16.0f;
              p.subLevel = 0.7f;
              p.filterCutoff = 0.4f;
              return p;
          }() },
        { "Trap Bell VA", "TRAP", [] {
              auto p = base();
              p.osc1Wave = Waveform::square;
              p.osc2Octave = 1;
              p.filterRes = 0.55f;
              p.crush = 0.25f;
              p.ampDecay = 0.12f;
              return p;
          }() },
        { "Young Thug Slime", "TRAP", [] {
              auto p = base();
              p.chorus = 0.62f;
              p.stereoWidth = 0.55f;
              p.glideMs = 130.0f;
              p.drift = 0.22f;
              return p;
          }() },
        { "Trap Night Keys", "TRAP", [] {
              auto p = base();
              p.osc2Wave = Waveform::triangle;
              p.chorus = 0.45f;
              p.ampAttack = 0.02f;
              p.filterCutoff = 0.58f;
              return p;
          }() },
        { "Hard Trap Stab", "TRAP", [] {
              auto p = base();
              p.drive = 0.55f;
              p.filterRes = 0.48f;
              p.ampDecay = 0.16f;
              p.ampSustain = 0.05f;
              p.filtEnvAmt = 0.85f;
              return p;
          }() },
        { "808 Trap Glide III", "TRAP", [] {
              auto p = base();
              p.subLevel = 0.95f;
              p.glideMs = 175.0f;
              p.filterCutoff = 0.32f;
              p.osc1Wave = Waveform::saw;
              return p;
          }() },
        { "Trap Arp Slide", "TRAP", [] {
              auto p = base();
              p.osc2Octave = 1;
              p.glideMs = 40.0f;
              p.ampDecay = 0.085f;
              p.filtEnvAmt = 0.78f;
              return p;
          }() },
        { "Drill Trap Lead", "TRAP", [] {
              auto p = base();
              p.unisonAmount = 0.6f;
              p.drive = 0.4f;
              p.filterCutoff = 0.5f;
              p.crush = 0.18f;
              return p;
          }() },
        { "Metro Style Pluck", "TRAP", [] {
              auto p = base();
              p.osc2DetuneCents = 12.0f;
              p.ampDecay = 0.07f;
              p.ampSustain = 0.0f;
              p.filterCutoff = 0.72f;
              return p;
          }() },
        { "Trap Supersaw Stack", "TRAP", [] {
              auto p = base();
              p.unisonAmount = 0.95f;
              p.unisonSpreadCents = 30.0f;
              p.chorus = 0.5f;
              p.filterCutoff = 0.56f;
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

} // namespace pluggva::presets
