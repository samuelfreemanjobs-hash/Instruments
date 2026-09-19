#pragma once

#include <cstdint>

namespace disklordz::factory
{

/** Five identity ROM layers (Fantom-style expansion world, all original content). */
enum class RomLayer : std::uint8_t
{
    real = 0,
    classicDigital = 1,
    analog = 2,
    loFi = 3,
    cyberShift = 4
};

inline constexpr std::uint32_t kRomLayerCount = 5;

/** Factory PCM cards installed with the plugin (split across 5 layers). */
inline constexpr std::uint32_t kFactoryRomBankCount = 16;

/** ~20 major families → hundreds of source multis, thousands of patch variants. */
enum class InstrumentFamily : std::uint16_t
{
    acousticPiano = 0,
    electricPiano = 1,
    keys = 2,
    organ = 3,
    bellsMallets = 4,
    guitar = 5,
    bass = 6,
    sub808 = 7,
    synthBass = 8,
    synthLead = 9,
    synthPluck = 10,
    pad = 11,
    strings = 12,
    brass = 13,
    woodwind = 14,
    choirVocal = 15,
    percussiveTonal = 16,
    texture = 17,
    atmosphere = 18,
    fx = 19,
    count = 20
};

/** Sampling depth tiers (velocity/key zones) — controls WAV budget. */
enum class SampleTier : std::uint8_t
{
    hero = 0,   // 500–2000 WAVs target (piano, rhodes, …)
    core = 1,   // 30–200 WAVs
    synth = 2,  // 1–20 WAVs
    texture = 3 // 1–5 WAVs
};

/** Production targets (documentation + factory validators). */
inline constexpr std::uint32_t kTargetSourceMultisamplesMin = 250;
inline constexpr std::uint32_t kTargetSourceMultisamplesMax = 350;
inline constexpr std::uint32_t kTargetRawWavMin = 8000;
inline constexpr std::uint32_t kTargetRawWavMax = 20000;
inline constexpr std::uint32_t kTargetFactoryPresetsMin = 1000;

inline RomLayer romLayerForBankIndex (std::uint32_t bankIndex)
{
    if (bankIndex >= kFactoryRomBankCount)
        return RomLayer::real;
    if (bankIndex < 3)
        return RomLayer::real;
    if (bankIndex < 6)
        return RomLayer::classicDigital;
    if (bankIndex < 9)
        return RomLayer::analog;
    if (bankIndex < 12)
        return RomLayer::loFi;
    return RomLayer::cyberShift;
}

inline const char* romLayerName (RomLayer layer)
{
    switch (layer)
    {
        case RomLayer::real: return "REAL";
        case RomLayer::classicDigital: return "CLASSIC_DIGITAL";
        case RomLayer::analog: return "ANALOG";
        case RomLayer::loFi: return "LO_FI";
        case RomLayer::cyberShift: return "CYBER_SHIFT";
    }
    return "REAL";
}

inline const char* instrumentFamilyName (InstrumentFamily f)
{
    switch (f)
    {
        case InstrumentFamily::acousticPiano: return "Acoustic Piano";
        case InstrumentFamily::electricPiano: return "Electric Piano";
        case InstrumentFamily::keys: return "Keys";
        case InstrumentFamily::organ: return "Organ";
        case InstrumentFamily::bellsMallets: return "Bells/Mallets";
        case InstrumentFamily::guitar: return "Guitar";
        case InstrumentFamily::bass: return "Bass";
        case InstrumentFamily::sub808: return "808/Sub";
        case InstrumentFamily::synthBass: return "Synth Bass";
        case InstrumentFamily::synthLead: return "Synth Lead";
        case InstrumentFamily::synthPluck: return "Synth Pluck";
        case InstrumentFamily::pad: return "Pads";
        case InstrumentFamily::strings: return "Strings";
        case InstrumentFamily::brass: return "Brass";
        case InstrumentFamily::woodwind: return "Woodwinds";
        case InstrumentFamily::choirVocal: return "Choir/Vocal";
        case InstrumentFamily::percussiveTonal: return "Percussive Tonal";
        case InstrumentFamily::texture: return "Texture";
        case InstrumentFamily::atmosphere: return "Atmosphere";
        case InstrumentFamily::fx: return "FX";
        default: return "Unknown";
    }
}

} // namespace disklordz::factory
