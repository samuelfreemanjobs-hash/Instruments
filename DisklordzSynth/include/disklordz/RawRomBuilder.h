#pragma once

#include "disklordz/RawRomFormat.h"

#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::synth
{

struct BuiltRawRom final
{
    std::vector<rawrom::WaveEntry> waves;
    std::vector<float> pcm;
    std::uint32_t bankIndex = 0;
    rawrom::RomBankCategory category = rawrom::RomBankCategory::studioPop;
};

/** Legacy single-card build (bank 0 only). */
bool buildRawRom (BuiltRawRom& out);

/** One expansion-card ROM (hundreds of waves). */
bool buildRawRomBank (std::uint32_t bankIndex, BuiltRawRom& out);

/** Write DLRROM01/02 file from an in-memory build. */
bool writeRawRomFile (const std::string& outputPath, const BuiltRawRom& built);

/** All factory cards → `pcm_bank_XX.dlrrom` in outputDir. Returns number written. */
int buildAndWriteAllFactoryRomBanks (const std::string& outputDir);

bool buildAndWriteRawRom (const std::string& outputPath);

} // namespace disklordz::synth
