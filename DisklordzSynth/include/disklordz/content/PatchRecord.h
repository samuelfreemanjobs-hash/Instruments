#pragma once

#include "disklordz/content/PresetTaxonomy.h"
#include "disklordz/content/ToneRecord.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::content
{

struct MacroSnapshot final
{
    float character = 0.5f;
    float brightness = 0.5f;
    float drive = 0.35f;
    float space = 0.4f;
    float movement = 0.3f;
    float width = 0.5f;
    float decay = 0.4f;
    float crush = 0.2f;
};

/** Four-tone instrument definition (shared by many presets). */
struct PatchRecord final
{
    std::string id;
    std::string name;
    std::array<std::string, 4> toneIds{};
    std::array<float, 4> toneLevels { 1.0f, 0.6f, 0.4f, 0.5f };
    MacroSnapshot macros{};
    PresetTaxonomyTags taxonomy{};
    std::string contentPackage;
};

/** Preset = patch + display name + optional macro overrides. */
struct PresetRecord final
{
    std::string id;
    std::string displayName;
    std::string patchId;
    MacroSnapshot macroOverrides{};
    PresetTaxonomyTags taxonomy{};
};

/** Live stack / split (future). */
struct PerformanceRecord final
{
    std::string id;
    std::string name;
    std::vector<std::string> patchIds;
};

} // namespace disklordz::content
