#include "MultisampleExporter.h"
#include "Exs24Exporter.h"
#include "MpcKeygroupExporter.h"

namespace autosampler
{

juce::String MultisampleExporter::sampleFileName (int rootNote, int velStart)
{
    if (velStart < 0)
        return "note_" + juce::String (rootNote).paddedLeft ('0', 3) + ".wav";
    return "note_" + juce::String (rootNote).paddedLeft ('0', 3) + "_v"
           + juce::String (velStart).paddedLeft ('0', 3) + ".wav";
}

juce::var MultisampleExporter::presetToVar (const MultisampleProgram& program)
{
    juce::Array<juce::var> zonesVar;

    for (std::size_t i = 0; i < program.zoneCount(); ++i)
    {
        const auto& z = program.zones()[i];
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty ("root", z.rootNote);
        obj->setProperty ("file", "samples/" + sampleFileName (z.rootNote));
        obj->setProperty ("lowKey", program.lowKeyForZone (i));
        obj->setProperty ("highKey", program.highKeyForZone (i));
        obj->setProperty ("loopStart", z.loopStart);
        obj->setProperty ("loopEnd", z.loopEnd);
        obj->setProperty ("loopConfidence", z.loopConfidence);

        const auto layers = z.velocityLayersForExport();
        if (layers.size() > 1)
        {
            juce::Array<juce::var> layerArr;
            for (const auto& layer : layers)
            {
                juce::DynamicObject::Ptr layerObj = new juce::DynamicObject();
                layerObj->setProperty ("velStart", layer.velStart);
                layerObj->setProperty ("velEnd", layer.velEnd);
                layerObj->setProperty ("file", "samples/" + sampleFileName (z.rootNote, layer.velStart));
                layerArr.add (juce::var (layerObj.get()));
            }
            obj->setProperty ("velocityLayers", layerArr);
        }

        zonesVar.add (juce::var (obj.get()));
    }

    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty ("format", kPresetFormat);
    root->setProperty ("name", program.name);
    root->setProperty ("zones", zonesVar);
    return juce::var (root.get());
}

bool MultisampleExporter::writePresetJson (const MultisampleProgram& program, const juce::File& presetJsonFile)
{
    const juce::var v = presetToVar (program);
    const juce::String json = juce::JSON::toString (v, true);
    return presetJsonFile.replaceWithText (json);
}

bool MultisampleExporter::writePresetSfz (const MultisampleProgram& program, const juce::File& presetSfzFile)
{
    juce::String text;
    text << "// Auto Sampler SFZ export — " << program.name << "\n\n";

    for (std::size_t i = 0; i < program.zoneCount(); ++i)
    {
        const auto& z = program.zones()[i];
        const auto layers = z.velocityLayersForExport();

        for (const auto& layer : layers)
        {
            const juce::String samplePath = "samples/" + sampleFileName (z.rootNote, layers.size() > 1 ? layer.velStart : -1);

            text << "<region>\n";
            text << "sample=" << samplePath << "\n";
            text << "lokey=" << program.lowKeyForZone (i) << "\n";
            text << "hikey=" << program.highKeyForZone (i) << "\n";
            text << "pitch_keycenter=" << z.rootNote << "\n";
            text << "lovel=" << layer.velStart << "\n";
            text << "hivel=" << layer.velEnd << "\n";

            if (layer.hasLoop())
            {
                text << "loop_mode=loop_continuous\n";
                text << "loop_start=" << layer.loopStart << "\n";
                text << "loop_end=" << layer.loopEnd << "\n";
            }

            text << "\n";
        }
    }

    return presetSfzFile.replaceWithText (text);
}

bool MultisampleExporter::writeWavZones (const MultisampleProgram& program,
                                             const juce::File& samplesDir,
                                             juce::String& error)
{
    juce::WavAudioFormat wav;

    for (const auto& zone : program.zones())
    {
        const auto layers = zone.velocityLayersForExport();
        for (const auto& layer : layers)
        {
            const juce::File wavFile = samplesDir.getChildFile (
                sampleFileName (zone.rootNote, layers.size() > 1 ? layer.velStart : -1));
            std::unique_ptr<juce::FileOutputStream> stream (wavFile.createOutputStream());
            if (stream == nullptr)
            {
                error = "Could not write " + wavFile.getFileName();
                return false;
            }

            juce::AudioFormatWriter* writer = wav.createWriterFor (stream.get(), zone.sampleRate,
                                                                     static_cast<unsigned int> (layer.audio.getNumChannels()),
                                                                     24, {}, 0);

            if (writer == nullptr)
            {
                error = "WAV writer failed for " + wavFile.getFileName();
                return false;
            }

            stream.release();
            std::unique_ptr<juce::AudioFormatWriter> writerOwner (writer);
            writerOwner->writeFromAudioSampleBuffer (layer.audio, 0, layer.audio.getNumSamples());
        }
    }

    return true;
}

ExportResult MultisampleExporter::exportPreset (const MultisampleProgram& program, const juce::File& outputDirectory)
{
    ExportResult result;
    if (program.empty())
    {
        result.error = "Program has no zones";
        return result;
    }

    const juce::File dir = outputDirectory.getChildFile (program.name.replaceCharacter ('/', '_'));
    const juce::File samplesDir = dir.getChildFile ("samples");
    if (! samplesDir.createDirectory())
    {
        result.error = "Could not create samples directory";
        return result;
    }

    juce::String wavError;
    if (! writeWavZones (program, samplesDir, wavError))
    {
        result.error = wavError;
        return result;
    }

    const juce::File presetJson = dir.getChildFile ("preset.json");
    if (! writePresetJson (program, presetJson))
    {
        result.error = "Could not write preset.json";
        return result;
    }

    const juce::File presetSfz = dir.getChildFile (program.name.replaceCharacter ('/', '_') + ".sfz");
    if (! writePresetSfz (program, presetSfz))
    {
        result.error = "Could not write SFZ";
        return result;
    }

    juce::String mpcError;
    if (! MpcKeygroupExporter::exportKeygroup (program, dir.getChildFile ("mpc"), mpcError))
    {
        result.error = mpcError;
        return result;
    }

    juce::String exsError;
    if (! Exs24Exporter::exportInstrument (program, dir.getChildFile ("exs"), exsError))
    {
        result.error = exsError;
        return result;
    }

    result.ok = true;
    result.presetJson = presetJson;
    return result;
}

ExportResult MultisampleExporter::exportMpcKit (const MultisampleProgram& program,
                                                  const juce::File& outputDirectory,
                                                  bool includeReadme)
{
    auto result = exportPreset (program, outputDirectory);
    if (! result.ok)
        return result;

    const juce::File dir = result.presetJson.getParentDirectory();

    if (includeReadme)
    {
        juce::String readme;
        readme << "Auto Sampler MPC kit\n";
        readme << "Program: " << program.name << "\n";
        readme << "Copy the mpc/" << program.name << "/ folder to MPC content.\n";
        readme << "Companion formats: SFZ, EXS24, preset.json\n";
        dir.getChildFile ("README_MPC.txt").replaceWithText (readme);
    }

    return result;
}

ExportResult MultisampleExporter::exportBank (const BankExportSpec& bank, const juce::File& outputDirectory)
{
    ExportResult result;
    if (bank.presets.empty())
    {
        result.error = "Bank has no presets";
        return result;
    }

    const juce::File bankDir = outputDirectory.getChildFile (bank.bankName.replaceCharacter ('/', '_'));
    if (! bankDir.createDirectory())
    {
        result.error = "Could not create bank directory";
        return result;
    }

    juce::Array<juce::var> entries;

    for (const auto& preset : bank.presets)
    {
        const auto single = exportPreset (preset, bankDir.getChildFile ("presets"));
        if (! single.ok)
            return single;

        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty ("name", preset.name);
        obj->setProperty ("presetFile", single.presetJson.getRelativePathFrom (bankDir));
        entries.add (juce::var (obj.get()));
    }

    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty ("format", kBankFormat);
    root->setProperty ("name", bank.bankName);
    root->setProperty ("presets", entries);

    const juce::File bankJson = bankDir.getChildFile ("bank.json");
    if (! bankJson.replaceWithText (juce::JSON::toString (juce::var (root.get()), true)))
    {
        result.error = "Could not write bank.json";
        return result;
    }

    result.ok = true;
    result.presetJson = bankJson;
    return result;
}

} // namespace autosampler
