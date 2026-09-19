#include "disklordz/RawRomBuilder.h"
#include "disklordz/ProceduralSynth.h"
#include "disklordz/RomFactorySpec.h"

#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

namespace disklordz::synth
{

namespace
{

float bankSeed (std::uint32_t bank, std::uint32_t a, std::uint32_t b) noexcept
{
    const std::uint32_t h = bank * 7919u + a * 104729u + b * 15485863u;
    return static_cast<float> ((h % 10000u)) / 10000.0f;
}

EngineParams programParams (EngineId engine, std::uint32_t bankIndex, int program, int zone)
{
    const float bp = bankSeed (bankIndex, static_cast<std::uint32_t> (program), 0u);
    const float bz = bankSeed (bankIndex, static_cast<std::uint32_t> (program), static_cast<std::uint32_t> (zone + 1));
    const float z = static_cast<float> (zone) / static_cast<float> (rawrom::kZonesPerProgram - 1);
    const float pv = static_cast<float> (program) / static_cast<float> (rawrom::kProgramsPerEngine - 1);

    EngineParams p;
    switch (engine)
    {
        case EngineId::additive:
            p.a = 0.35f + pv * 0.45f + bp * 0.15f;
            p.b = 0.25f + z * 0.35f + bz * 0.1f;
            p.c = 0.3f + pv * 0.25f;
            p.d = 0.2f + z * 0.3f;
            break;
        case EngineId::karplus:
            p.a = 0.985f - pv * 0.012f - z * 0.004f;
            p.b = 0.15f + z * 0.55f + bp * 0.2f;
            p.c = 0.25f + pv * 0.35f;
            p.d = 0.08f + bz * 0.2f;
            break;
        case EngineId::wave:
            p.a = 0.15f + pv * 0.65f + z * 0.1f;
            p.b = 0.2f + z * 0.45f;
            p.c = 1.0f;
            p.d = 0.35f + bp * 0.45f;
            break;
        case EngineId::subtractive:
            p.a = 0.08f + pv * 0.35f + z * 0.15f;
            p.b = 0.12f + z * 0.25f;
            p.c = pv * 0.4f;
            p.d = 0.45f + pv * 0.35f + bp * 0.15f;
            break;
    }
    return p;
}

EngineParams standaloneParams (EngineId engine, std::uint32_t bankIndex, int index)
{
    const float s = bankSeed (bankIndex, 9000u, static_cast<std::uint32_t> (index));
    const float t = static_cast<float> (index % 17) / 16.0f;
    EngineParams p;
    p.a = 0.2f + s * 0.65f;
    p.b = 0.15f + t * 0.55f;
    p.c = engine == EngineId::wave ? 1.0f : 0.25f + t * 0.5f;
    p.d = 0.25f + s * 0.55f;
    return p;
}

std::size_t frameCountFor (EngineId engine, bool isZone)
{
    if (! isZone)
    {
        switch (engine)
        {
            case EngineId::additive: return 2048;
            case EngineId::karplus: return 4096;
            case EngineId::wave: return 8192;
            case EngineId::subtractive: return 3072;
        }
    }
    switch (engine)
    {
        case EngineId::additive: return 3072;
        case EngineId::karplus: return 6144;
        case EngineId::wave: return 12288;
        case EngineId::subtractive: return 8192;
    }
    return 2048;
}

int zoneRootNote (int zoneIndex)
{
    static constexpr int kRoots[] = { 28, 31, 34, 38, 42, 46, 50, 55, 60, 65, 72, 84 };
    return kRoots[zoneIndex % rawrom::kZonesPerProgram];
}

std::uint16_t setIdFor (EngineId engine, int program)
{
    const int engineIdx = static_cast<int> (engine);
    return static_cast<std::uint16_t> (1 + engineIdx * static_cast<int> (rawrom::kProgramsPerEngine) + program);
}

void appendWave (BuiltRawRom& rom,
                 std::vector<float> pcm,
                 std::int16_t root,
                 EngineId engine,
                 std::uint16_t multisampleSetId,
                 bool looped)
{
    rawrom::WaveEntry e{};
    e.waveId = static_cast<std::uint32_t> (rom.waves.size());
    e.pcmOffset = static_cast<std::uint32_t> (rom.pcm.size());
    e.numFrames = static_cast<std::uint32_t> (pcm.size());
    e.rootMidiNote = root;
    e.engineId = static_cast<std::uint16_t> (engine);
    e.multisampleSetId = multisampleSetId;
    if (looped && pcm.size() > 256)
    {
        e.flags = rawrom::waveLooped;
        e.loopStart = static_cast<std::uint32_t> (pcm.size() / 4);
        e.loopEnd = static_cast<std::uint32_t> (pcm.size());
    }
    rom.pcm.insert (rom.pcm.end(), pcm.begin(), pcm.end());
    rom.waves.push_back (e);
}

EngineId engineForIndex (int engineIdx)
{
    static constexpr EngineId kEngines[] = { EngineId::additive, EngineId::karplus, EngineId::wave,
                                             EngineId::subtractive };
    return kEngines[engineIdx % 4];
}

} // namespace

bool buildRawRomBank (std::uint32_t bankIndex, BuiltRawRom& out)
{
    out.waves.clear();
    out.pcm.clear();
    out.bankIndex = bankIndex;
    out.category = static_cast<rawrom::RomBankCategory> (factory::romLayerForBankIndex (bankIndex));

    for (int engineIdx = 0; engineIdx < 4; ++engineIdx)
    {
        const auto engine = engineForIndex (engineIdx);
        for (int program = 0; program < static_cast<int> (rawrom::kProgramsPerEngine); ++program)
        {
            const auto setId = setIdFor (engine, program);
            for (int zone = 0; zone < static_cast<int> (rawrom::kZonesPerProgram); ++zone)
            {
                auto pcm = renderEngine (engine,
                                         frameCountFor (engine, true),
                                         programParams (engine, bankIndex, program, zone));
                appendWave (out,
                            std::move (pcm),
                            static_cast<std::int16_t> (zoneRootNote (zone)),
                            engine,
                            setId,
                            engine == EngineId::wave);
            }
        }
    }

    for (int i = 0; i < static_cast<int> (rawrom::kStandaloneWavesPerBank); ++i)
    {
        const auto engine = engineForIndex (i);
        auto pcm = renderEngine (engine,
                                 frameCountFor (engine, false),
                                 standaloneParams (engine, bankIndex, i));
        appendWave (out, std::move (pcm), 60, engine, 0, engine == EngineId::wave);
    }

    return ! out.waves.empty();
}

bool buildRawRom (BuiltRawRom& out)
{
    return buildRawRomBank (0, out);
}

bool writeRawRomFile (const std::string& outputPath, const BuiltRawRom& built)
{
    rawrom::RomHeader hdr{};
    std::memcpy (hdr.magic, rawrom::kRomMagic, 8);
    hdr.version = rawrom::kRomVersion;
    hdr.bankIndex = built.bankIndex;
    hdr.bankCategory = static_cast<std::uint16_t> (built.category);
    hdr.reserved = 0;
    hdr.waveCount = static_cast<std::uint32_t> (built.waves.size());
    hdr.pcmFloatCount = static_cast<std::uint32_t> (built.pcm.size());

    std::ofstream out (outputPath, std::ios::binary);
    if (! out)
        return false;

    out.write (reinterpret_cast<const char*> (&hdr), sizeof (hdr));
    out.write (reinterpret_cast<const char*> (built.waves.data()),
               static_cast<std::streamsize> (built.waves.size() * sizeof (built.waves[0])));
    out.write (reinterpret_cast<const char*> (built.pcm.data()),
               static_cast<std::streamsize> (built.pcm.size() * sizeof (float)));
    return out.good();
}

bool buildAndWriteRawRom (const std::string& outputPath)
{
    BuiltRawRom built;
    if (! buildRawRom (built))
        return false;
    return writeRawRomFile (outputPath, built);
}

int buildAndWriteAllFactoryRomBanks (const std::string& outputDir)
{
    std::error_code ec;
    fs::create_directories (outputDir, ec);

    int written = 0;
    for (std::uint32_t bank = 0; bank < rawrom::kFactoryRomBankCount; ++bank)
    {
        BuiltRawRom built;
        if (! buildRawRomBank (bank, built))
            continue;

        std::ostringstream name;
        name << "pcm_bank_" << std::setw (2) << std::setfill ('0') << bank << ".dlrrom";
        const fs::path path = fs::path (outputDir) / name.str();
        if (writeRawRomFile (path.string(), built))
            ++written;
    }
    return written;
}

} // namespace disklordz::synth
