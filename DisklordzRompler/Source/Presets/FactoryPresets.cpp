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
              p.toneRomBank = { 0, 0, 0, 0 };
              p.toneProgram = { 0, 0, 1, 0 };
              return p;
          }() },
        { "Phonk Cowbell", "BELL",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.95f, 0.25f, 0.15f, 0.5f };
              p.macroDrive = 0.65f;
              p.macroCrush = 0.45f;
              p.macroCharacter = 0.7f;
              p.toneRomBank = { 1, 1, 1, 1 };
              p.toneProgram = { 2, 1, 0, 3 };
              return p;
          }() },
        { "Haunted Keys", "KEYS",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.4f, 0.9f, 0.45f, 0.35f };
              p.macroMovement = 0.55f;
              p.macroCrush = 0.3f;
              p.toneRomBank = { 2, 2, 2, 2 };
              p.toneProgram = { 1, 3, 2, 0 };
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
              p.toneRomBank = { 3, 3, 3, 3 };
              p.toneProgram = { 0, 0, 0, 4 };
              return p;
          }() },
        { "Drift Lead", "LEAD",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.7f, 0.65f, 0.5f, 0.4f };
              p.macroWidth = 0.65f;
              p.macroMovement = 0.45f;
              p.toneRomBank = { 4, 5, 4, 5 };
              p.toneProgram = { 3, 2, 4, 1 };
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
              p.toneRomBank = { 6, 6, 6, 6 };
              p.toneProgram = { 2, 1, 5, 0 };
              return p;
          }() },
        { "Sinister Pluck", "PLUCK",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.75f, 0.55f, 0.25f, 0.3f };
              p.ampDecay = 0.12f;
              p.ampSustain = 0.0f;
              p.ampRelease = 0.25f;
              p.toneRomBank = { 7, 8, 7, 8 };
              p.toneProgram = { 4, 3, 1, 2 };
              return p;
          }() },
        { "Dark Stack", "LEAD",
          [] {
              RomplerParams p;
              p.toneLevel = { 0.8f, 0.7f, 0.6f, 0.65f };
              p.macroCharacter = 0.6f;
              p.macroDrive = 0.4f;
              p.toneRomBank = { 9, 10, 11, 12 };
              p.toneProgram = { 1, 2, 3, 4 };
              return p;
          }() },
    };
    return kPresets;
}

} // namespace disklordz::rompler::presets
