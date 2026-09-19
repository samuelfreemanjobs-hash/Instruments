#include "disklordz/MultisampleRomBuilder.h"

#include "disklordz/ProceduralSynth.h"
#include "disklordz/WavReader.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace disklordz::factory
{

namespace
{

void appendPcmWave (synth::BuiltRawRom& rom,
                    std::vector<float> pcm,
                    std::int16_t root,
                    synth::EngineId engine,
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

synth::EngineId engineForFamily (InstrumentFamily family)
{
    switch (family)
    {
        case InstrumentFamily::sub808:
        case InstrumentFamily::synthBass:
        case InstrumentFamily::bass:
            return synth::EngineId::subtractive;
        case InstrumentFamily::bellsMallets:
        case InstrumentFamily::choirVocal:
            return synth::EngineId::additive;
        case InstrumentFamily::guitar:
        case InstrumentFamily::keys:
            return synth::EngineId::karplus;
        case InstrumentFamily::pad:
        case InstrumentFamily::atmosphere:
        case InstrumentFamily::texture:
        case InstrumentFamily::fx:
            return synth::EngineId::wave;
        default:
            return synth::EngineId::additive;
    }
}

} // namespace

bool appendMultisampleToRom (const MultisampleManifest& manifest,
                             const std::string& contentRoot,
                             synth::BuiltRawRom& rom,
                             std::string& error)
{
    const auto engine = engineForFamily (manifest.family);
    const auto setId = manifest.multisampleSetId > 0
                           ? manifest.multisampleSetId
                           : rawrom::multisampleSetForEngine (engine);

    for (const auto& zone : manifest.zones)
    {
        const fs::path wavPath = fs::path (contentRoot) / zone.wavPath;
        synth::WavData wav;
        if (! synth::readWavFile (wavPath.string(), wav, error))
            return false;

        appendPcmWave (rom,
                       std::move (wav.samples),
                       zone.rootMidiNote,
                       engine,
                       setId,
                       zone.looped);
    }
    return true;
}

bool buildRomCardFromManifests (const std::string& manifestDir,
                                const std::string& outputDlrrom,
                                std::uint32_t bankIndex,
                                std::string& error)
{
    if (! fs::is_directory (manifestDir))
    {
        error = "manifest dir missing";
        return false;
    }

    synth::BuiltRawRom built;
    built.bankIndex = bankIndex;
    built.category = static_cast<rawrom::RomBankCategory> (romLayerForBankIndex (bankIndex));

    for (const auto& entry : fs::directory_iterator (manifestDir))
    {
        if (entry.path().extension() != ".manifest")
            continue;

        MultisampleManifest manifest;
        if (! loadMultisampleManifest (entry.path().string(), manifest, error))
            return false;

        const auto root = entry.path().parent_path().string();
        if (! appendMultisampleToRom (manifest, root, built, error))
            return false;
    }

    if (built.waves.empty())
    {
        error = "no waves built from manifests";
        return false;
    }

    return synth::writeRawRomFile (outputDlrrom, built);
}

} // namespace disklordz::factory
