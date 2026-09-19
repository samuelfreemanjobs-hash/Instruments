#pragma once

#include <cstdint>

namespace disklordz::content
{

/** Browser taxonomy — lock before mass preset generation. */
enum class InstrumentType : std::uint16_t
{
    piano = 0,
    electricPiano,
    keys,
    organ,
    bell,
    mallet,
    guitar,
    bass,
    sub808,
    synthBass,
    lead,
    pluck,
    pad,
    strings,
    brass,
    woodwind,
    choir,
    vocal,
    chord,
    arp,
    synth,
    atmosphere,
    texture,
    fx,
    count
};

enum class SamplingStrategy : std::uint8_t
{
    deep = 0,
    medium,
    lightweight,
    oneShot
};

struct PresetTaxonomyTags final
{
    InstrumentType instrumentType = InstrumentType::pad;
    const char* subtype = "";
    const char* genre = "";
    const char* mood = "";
    const char* era = "";
    const char* intensity = "";
    const char* registerHint = "";
    const char* sourceMultisampleId = "";
    const char* expansionId = "";
};

} // namespace disklordz::content
