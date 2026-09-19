#include "Presets/FactoryPresets.h"

namespace disklordz::rompler::presets
{

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kPresets = {
        { "Midnight Bell", "BELL",
          [] {
              RomplerParams p;
              p.toneLevel = { 1.0f, 0.35f, 0.2f, 0.45f };
              p.macroBrightness = 0.55f;
              p.macroDrive = 0.25f;
              p.filterCutoff = 0.78f;
              return p;
          }() },
        { "Phonk Cowbell", "BELL",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.95f, 0.25f, 0.15f, 0.5f };
              p.macroDrive = 0.65f;
              p.macroCrush = 0.45f;
              p.macroCharacter = 0.7f;
              return p;
          }() },
        { "Haunted Keys", "KEYS",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.4f, 0.9f, 0.45f, 0.35f };
              p.macroMovement = 0.55f;
              p.macroCrush = 0.3f;
              return p;
          }() },
        { "Sub-Zero 808", "BASS",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.2f, 0.15f, 0.1f, 1.0f };
              p.macroDrive = 0.5f;
              p.filterCutoff = 0.35f;
              p.ampDecay = 0.45f;
              p.ampSustain = 0.95f;
              return p;
          }() },
        { "Drift Lead", "LEAD",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.7f, 0.65f, 0.5f, 0.4f };
              p.macroWidth = 0.65f;
              p.macroMovement = 0.45f;
              return p;
          }() },
        { "Tape Pad", "PAD",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.35f, 0.4f, 0.95f, 0.55f };
              p.macroSpace = 0.7f;
              p.macroCrush = 0.35f;
              p.ampAttack = 0.35f;
              p.ampRelease = 1.0f;
              return p;
          }() },
        { "Sinister Pluck", "PLUCK",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.75f, 0.55f, 0.25f, 0.3f };
              p.ampDecay = 0.12f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.25f;
              return p;
          }() },
        { "Dark Stack", "LEAD",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.8f, 0.7f, 0.6f, 0.65f };
              p.macroCharacter = 0.6f;
              p.macroDrive = 0.4f;
              return p;
          }() },
    };
    return kPresets;
}

} // namespace disklordz::rompler::presets
