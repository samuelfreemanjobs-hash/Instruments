#include "VibeArchetypeLibrary.h"
#include "VibeTuning.h"

namespace resonance::audio
{
namespace
{
struct Archetype
{
    juce::String tag;
    juce::StringArray keywords;
    VibeTuning tune;
};

void blendArchetype(const VibeTuning& arch, VibeTuning& tune, float amount)
{
    tune.lowDb += arch.lowDb * amount;
    tune.midDb += arch.midDb * amount;
    tune.highDb += arch.highDb * amount;
    tune.thresholdDb += (arch.thresholdDb - (-18.0f)) * amount * 0.3f;
    tune.ratio += (arch.ratio - tune.ratio) * amount * 0.5f;
    if (arch.useLimiter)
        tune.useLimiter = true;
    if (arch.useTape)
    {
        tune.useTape = true;
        tune.tapeDrive = juce::jmax(tune.tapeDrive, arch.tapeDrive);
    }
    if (arch.useWidth)
    {
        tune.useWidth = true;
        tune.width = juce::jmax(tune.width, arch.width);
    }
}

const std::vector<Archetype>& archetypes()
{
    static const std::vector<Archetype> list {
        { "club_master",
          { "club", "master", "loud" },
          [] {
              VibeTuning t;
              t.useEq = true;
              t.useComp = true;
              t.useLimiter = true;
              t.lowDb = 1.5f;
              t.highDb = 2.0f;
              t.thresholdDb = -14.0f;
              t.ratio = 3.5f;
              return t;
          }() },
        { "tape_vocal",
          { "vocal", "tape", "warm" },
          [] {
              VibeTuning t;
              t.useEq = true;
              t.useDeEsser = true;
              t.useComp = true;
              t.useTape = true;
              t.tapeDrive = 0.25f;
              t.midDb = 2.5f;
              return t;
          }() },
        { "drum_punch",
          { "drum", "punch", "snappy" },
          [] {
              VibeTuning t;
              t.useHpf = true;
              t.hpfHz = 70.0f;
              t.useComp = true;
              t.attackMs = 5.0f;
              t.ratio = 5.0f;
              return t;
          }() },
        { "bedroom_lofi",
          { "lofi", "bedroom", "dusty" },
          [] {
              VibeTuning t;
              t.useEq = true;
              t.useTape = true;
              t.tapeDrive = 0.4f;
              t.highDb = -5.0f;
              t.lowDb = 2.0f;
              return t;
          }() },
        { "streaming_clean",
          { "streaming", "clean", "podcast" },
          [] {
              VibeTuning t;
              t.useHpf = true;
              t.hpfHz = 85.0f;
              t.useComp = true;
              t.ratio = 2.2f;
              t.useLimiter = true;
              t.ceilingDb = -1.0f;
              return t;
          }() },
    };
    return list;
}
} // namespace

void VibeArchetypeLibrary::applyBestArchetype(const juce::String& normalisedBrief, VibeTuning& tune)
{
    float bestScore = 0.0f;
    const Archetype* best = nullptr;

    for (const auto& arch : archetypes())
    {
        float score = 0.0f;
        for (const auto& kw : arch.keywords)
        {
            if (normalisedBrief.contains(kw))
                score += 1.0f;
        }
        if (score > bestScore)
        {
            bestScore = score;
            best = &arch;
        }
    }

    if (best != nullptr && bestScore > 0.0f)
    {
        blendArchetype(best->tune, tune, juce::jmin(1.0f, bestScore * 0.45f));
        tune.rationales.add("Archetype blend: " + best->tag);
    }
}

juce::StringArray VibeArchetypeLibrary::chipSuggestions()
{
    return { "warm punchy club master",
             "lofi dusty tape vocals",
             "bright airy streaming clean",
             "gentle transparent glue",
             "aggressive hype limit",
             "tight drums punchy snappy",
             "wide synths spacious",
             "vocal forward de-ess" };
}
} // namespace resonance::audio
