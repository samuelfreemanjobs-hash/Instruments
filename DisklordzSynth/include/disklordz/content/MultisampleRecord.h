#pragma once

#include "disklordz/content/PresetTaxonomy.h"
#include "disklordz/content/SampleAssetRecord.h"

#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::content
{

struct MultisampleZoneRef final
{
    std::string sampleId;
    std::int16_t rootMidiNote = 60;
    std::int16_t keyLow = 0;
    std::int16_t keyHigh = 127;
    std::uint8_t velocityLow = 0;
    std::uint8_t velocityHigh = 127;
};

struct MultisampleRecord final
{
    std::string id;
    std::string name;
    InstrumentType category = InstrumentType::keys;
    SamplingStrategy strategy = SamplingStrategy::medium;
    std::vector<MultisampleZoneRef> zones;
    std::uint8_t romBankIndex = 0;
    std::uint16_t multisampleSetId = 1;
    std::string contentPackage;
};

} // namespace disklordz::content
