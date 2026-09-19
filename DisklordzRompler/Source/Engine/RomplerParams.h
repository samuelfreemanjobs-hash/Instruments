#pragma once

#include <array>
#include <cstdint>

namespace disklordz::rompler
{

struct RomplerParams final
{
    float outputGainDb = -3.0f;
    std::array<float, 4> toneLevel { 0.85f, 0.55f, 0.35f, 0.75f };
    float macroCharacter = 0.5f;
    float macroBrightness = 0.45f;
    float macroDrive = 0.35f;
    float macroSpace = 0.4f;
    float macroMovement = 0.3f;
    float macroWidth = 0.5f;
    float macroDecay = 0.4f;
    float macroCrush = 0.2f;
    float filterCutoff = 0.75f;
    float ampAttack = 0.005f;
    float ampDecay = 0.25f;
    float ampSustain = 0.85f;
    float ampRelease = 0.35f;
    float pitchBendSemis = 0.0f;
    /** Expansion ROM card index (0…15 factory). */
    std::array<std::uint8_t, 4> toneRomBank { 0, 0, 0, 0 };
    /** Program variant 0…7 within each tone's engine family on that card. */
    std::array<std::uint8_t, 4> toneProgram { 0, 0, 0, 0 };
};

} // namespace disklordz::rompler
