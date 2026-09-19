#pragma once

#include "disklordz/MultisampleManifest.h"
#include "disklordz/RawRomBuilder.h"

#include <string>

namespace disklordz::factory
{

/** Append one manifest's zones into a ROM build (WAV → wave table entries). */
bool appendMultisampleToRom (const MultisampleManifest& manifest,
                             const std::string& contentRoot,
                             synth::BuiltRawRom& rom,
                             std::string& error);

/** Build a single `.dlrrom` card from a directory of `*.manifest` files. */
bool buildRomCardFromManifests (const std::string& manifestDir,
                                const std::string& outputDlrrom,
                                std::uint32_t bankIndex,
                                std::string& error);

} // namespace disklordz::factory
