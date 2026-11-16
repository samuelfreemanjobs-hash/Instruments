#pragma once
// Unified DNA formula catalog  -  Bang / Hitmaker / MelodyMotion / SouthernTriumph (32 kits).
#include "BangBank.h"
#include "HitmakerBank.h"
#include "MelodyMotionBank.h"
#include "SouthernTriumphBank.h"
#include <cstdint>

namespace disklordz::genre_bank
{

enum class Line : uint8_t
{
    Bang = 0,
    Hitmaker,
    MelodyMotion,
    SouthernTriumph,
    Count
};

struct Formula
{
    Line line = Line::Bang;
    uint8_t bankIndex = 0;
    const char* id = "";
    const char* displayName = "";
};

inline constexpr Formula kFormulas[32] = {
    // Bang (0–7)
    { Line::Bang, 0, "bang_milli_pressure", "Bang  -  Milli Pressure" },
    { Line::Bang, 1, "bang_dirty_digital", "Bang  -  Dirty Digital" },
    { Line::Bang, 2, "bang_atlanta_eccentric", "Bang  -  Atlanta Eccentric" },
    { Line::Bang, 3, "bang_boss_drums", "Bang  -  Boss Drums" },
    { Line::Bang, 4, "bang_street_experimental", "Bang  -  Street Experimental" },
    { Line::Bang, 5, "bang_southern_futurism", "Bang  -  Southern Futurism" },
    { Line::Bang, 6, "bang_raw_machine", "Bang  -  Raw Machine" },
    { Line::Bang, 7, "bang_minimal_monster", "Bang  -  Minimal Monster" },
    // Hitmaker (8–15)
    { Line::Hitmaker, 0, "hitmaker_earthquake", "Hitmaker  -  Earthquake" },
    { Line::Hitmaker, 1, "hitmaker_atlanta_heat", "Hitmaker  -  Atlanta Heat" },
    { Line::Hitmaker, 2, "hitmaker_trunk_rattle", "Hitmaker  -  Trunk Rattle" },
    { Line::Hitmaker, 3, "hitmaker_hit_record", "Hitmaker  -  Hit Record" },
    { Line::Hitmaker, 4, "hitmaker_dark_experiment", "Hitmaker  -  Dark Experiment" },
    { Line::Hitmaker, 5, "hitmaker_club_anthem", "Hitmaker  -  Club Anthem" },
    { Line::Hitmaker, 6, "hitmaker_melodic_pressure", "Hitmaker  -  Melodic Pressure" },
    { Line::Hitmaker, 7, "hitmaker_future_machine", "Hitmaker  -  Future Machine" },
    // Melody & Motion (16–23)
    { Line::MelodyMotion, 0, "mm_rich_bounce", "MelodyMotion  -  Rich Bounce" },
    { Line::MelodyMotion, 1, "mm_slime_melody", "MelodyMotion  -  Slime Melody" },
    { Line::MelodyMotion, 2, "mm_church_to_trap", "MelodyMotion  -  Church To Trap" },
    { Line::MelodyMotion, 3, "mm_atlanta_lifestyle", "MelodyMotion  -  Atlanta Lifestyle" },
    { Line::MelodyMotion, 4, "mm_keys_and_bells", "MelodyMotion  -  Keys And Bells" },
    { Line::MelodyMotion, 5, "mm_street_anthem", "MelodyMotion  -  Street Anthem" },
    { Line::MelodyMotion, 6, "mm_melodic_luxury", "MelodyMotion  -  Melodic Luxury" },
    { Line::MelodyMotion, 7, "mm_late_night_london", "MelodyMotion  -  Late Night London" },
    // Southern Triumph (24–31)
    { Line::SouthernTriumph, 0, "st_southern_triumph", "SouthernTriumph  -  Southern Triumph" },
    { Line::SouthernTriumph, 1, "st_memphis_roots", "SouthernTriumph  -  Memphis Roots" },
    { Line::SouthernTriumph, 2, "st_atlanta_anthem", "SouthernTriumph  -  Atlanta Anthem" },
    { Line::SouthernTriumph, 3, "st_orchestral_soul", "SouthernTriumph  -  Orchestral Soul" },
    { Line::SouthernTriumph, 4, "st_triple_kick_warfare", "SouthernTriumph  -  Triple Kick Warfare" },
    { Line::SouthernTriumph, 5, "st_drum_squad_classics", "SouthernTriumph  -  Drum Squad Classics" },
    { Line::SouthernTriumph, 6, "st_blues_and_trap", "SouthernTriumph  -  Blues And Trap" },
    { Line::SouthernTriumph, 7, "st_epic_street_cinema", "SouthernTriumph  -  Epic Street Cinema" },
};

inline constexpr int kFormulaCount = 32;

inline const Formula& formulaAt(int index) noexcept
{
    const int i = (index < 0) ? 0 : (index % kFormulaCount);
    return kFormulas[static_cast<size_t>(i)];
}

inline const char* lineName(Line line) noexcept
{
    switch (line)
    {
    case Line::Bang: return "Bang";
    case Line::Hitmaker: return "Hitmaker";
    case Line::MelodyMotion: return "MelodyMotion";
    case Line::SouthernTriumph: return "SouthernTriumph";
    default: return "Unknown";
    }
}

/** Build a full 16-pad KitProgram for a catalog formula index (0..31). */
inline KitProgram makeFormulaKit(int formulaIndex, uint64_t seed = 0) noexcept
{
    const auto& f = formulaAt(formulaIndex);
    const uint64_t s = (seed != 0) ? seed : (0xDF400000ULL + static_cast<uint64_t>(formulaIndex) * 0x10001ULL);
    switch (f.line)
    {
    case Line::Bang:
        return bang::makeKit(static_cast<bang::Bank>(f.bankIndex % 8u), s);
    case Line::Hitmaker:
        return hitmaker::makeKit(static_cast<hitmaker::Bank>(f.bankIndex % 8u), s);
    case Line::MelodyMotion:
        return melodymotion::makeKit(static_cast<melodymotion::Bank>(f.bankIndex % 8u), s);
    case Line::SouthernTriumph:
        return southern::makeKit(static_cast<southern::Bank>(f.bankIndex % 8u), s);
    default:
        return {};
    }
}

inline hitmaker::FusionParams fusionForFormula(int formulaIndex) noexcept
{
    const auto& f = formulaAt(formulaIndex);
    if (f.line != Line::Hitmaker)
        return hitmaker::makeFusionFamily(0);
    return hitmaker::fusionFor(static_cast<hitmaker::Bank>(f.bankIndex % 8u));
}

inline bool usesFusionBass(int formulaIndex) noexcept
{
    return formulaAt(formulaIndex).line == Line::Hitmaker;
}

} // namespace disklordz::genre_bank
