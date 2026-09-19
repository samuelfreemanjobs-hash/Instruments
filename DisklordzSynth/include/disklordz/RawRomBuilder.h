#pragma once

#include "disklordz/RawRomFormat.h"

#include <string>
#include <vector>

namespace disklordz::synth
{

/**
 * Build a raw wave ROM using only the four offline engines.
 * Waves are stored in a flat table + PCM pool (not pre-mapped tone regions).
 */
bool buildAndWriteRawRom (const std::string& outputPath);

/** In-memory build for tests and exporters. */
struct BuiltRawRom final
{
    std::vector<rawrom::WaveEntry> waves;
    std::vector<float> pcm;
};

bool buildRawRom (BuiltRawRom& out);

} // namespace disklordz::synth
