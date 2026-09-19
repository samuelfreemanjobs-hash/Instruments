#include "disklordz/RawRomBuilder.h"
#include "disklordz/ProceduralSynth.h"

#include <cstring>
#include <fstream>

namespace disklordz::synth
{

namespace
{

EngineParams zoneParams (EngineId engine, int zoneIndex)
{
    const float z = static_cast<float> (zoneIndex) / 7.0f;
    EngineParams p;
    switch (engine)
    {
        case EngineId::additive:
            p.a = 0.45f + z * 0.35f;
            p.b = 0.35f + z * 0.2f;
            p.c = 0.4f + z * 0.15f;
            break;
        case EngineId::karplus:
            p.a = 0.992f - z * 0.008f;
            p.b = 0.25f + z * 0.35f;
            p.d = 0.12f + z * 0.08f;
            break;
        case EngineId::wave:
            p.a = 0.25f + z * 0.55f;
            p.b = 0.3f + z * 0.2f;
            p.c = 1.0f;
            p.d = 0.5f;
            break;
        case EngineId::subtractive:
            p.a = 0.15f + z * 0.25f;
            p.b = 0.2f + z * 0.15f;
            p.c = 0.0f;
            p.d = 0.55f + z * 0.2f;
            break;
    }
    return p;
}

std::size_t frameCountFor (EngineId engine)
{
    switch (engine)
    {
        case EngineId::additive: return 4096;
        case EngineId::karplus: return 8192;
        case EngineId::wave: return 16384;
        case EngineId::subtractive: return 12000;
    }
    return 4096;
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

EngineParams standaloneParams (EngineId engine, int variant)
{
    const float v = static_cast<float> (variant) / 7.0f;
    EngineParams p;
    p.a = 0.25f + v * 0.6f;
    p.b = 0.2f + v * 0.5f;
    p.c = engine == EngineId::wave ? 1.0f : 0.35f + v * 0.4f;
    p.d = 0.3f + v * 0.5f;
    return p;
}

} // namespace

bool buildRawRom (BuiltRawRom& out)
{
    out.waves.clear();
    out.pcm.clear();

    const int zoneRoots[] = { 36, 43, 50, 57, 64, 71, 78, 84 };
    const EngineId engines[] = { EngineId::additive, EngineId::karplus, EngineId::wave, EngineId::subtractive };

    for (auto engine : engines)
    {
        const auto setId = rawrom::multisampleSetForEngine (engine);
        for (int i = 0; i < rawrom::kZonesPerSet; ++i)
        {
            auto pcm = renderEngine (engine, frameCountFor (engine), zoneParams (engine, i));
            appendWave (out, std::move (pcm), static_cast<std::int16_t> (zoneRoots[i]), engine, setId,
                        engine == EngineId::wave);
        }
    }

    for (auto engine : engines)
    {
        for (int v = 0; v < 4; ++v)
        {
            const std::size_t frames = frameCountFor (engine) / 2;
            auto pcm = renderEngine (engine, frames, standaloneParams (engine, v));
            appendWave (out, std::move (pcm), 60, engine, 0, engine == EngineId::wave);
        }
    }

    return ! out.waves.empty();
}

bool buildAndWriteRawRom (const std::string& outputPath)
{
    BuiltRawRom built;
    if (! buildRawRom (built))
        return false;

    rawrom::RomHeader hdr{};
    std::memcpy (hdr.magic, rawrom::kRomMagic, 8);
    hdr.version = rawrom::kRomVersion;
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

} // namespace disklordz::synth
