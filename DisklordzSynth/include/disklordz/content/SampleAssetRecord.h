#pragma once

#include "disklordz/content/PresetTaxonomy.h"

#include <cstdint>
#include <string>

namespace disklordz::content
{

struct SampleAssetRecord final
{
    std::string id;
    std::string filename;
    InstrumentType category = InstrumentType::keys;
    std::string instrument;
    std::string source;
    std::int16_t rootNote = 60;
    std::int16_t keyRangeLow = 0;
    std::int16_t keyRangeHigh = 127;
    std::uint8_t velocityMin = 0;
    std::uint8_t velocityMax = 127;
    std::uint32_t sampleRate = 48000;
    std::uint16_t bitDepth = 24;
    std::uint16_t channels = 1;
    std::uint64_t lengthFrames = 0;
    std::uint64_t loopStart = 0;
    std::uint64_t loopEnd = 0;
    bool loopEnabled = false;
    float loudnessLufs = 0.0f;
    float peakDbfs = 0.0f;
    float tuningCents = 0.0f;
    std::uint8_t roundRobin = 0;
    std::uint8_t velocityLayer = 0;
    std::string license;
    std::string creator;
    std::string contentPackage;
};

} // namespace disklordz::content
