#pragma once

#include <juce_core/juce_core.h>

namespace sp1200
{
class SamplerEngine;

/** Single `.sp12p` bundle: embedded 12-bit samples + patterns + song + pads. */
class ProjectFile
{
public:
    static constexpr const char* kExtension = ".sp12p";
    static bool saveToFile (const SamplerEngine& engine, const juce::File& file);
    static bool loadFromFile (SamplerEngine& engine, const juce::File& file);
    static bool saveToMemoryBlock (const SamplerEngine& engine, juce::MemoryBlock& dest);
    static bool loadFromMemoryBlock (SamplerEngine& engine, const void* data, std::size_t size);
};

} // namespace sp1200
