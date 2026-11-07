#include "MultisampleExporter.h"
#include "MpcKeygroupExporter.h"

namespace autosampler
{

static juce::String sampleFileName (int rootNote)
{
    return "note_" + juce::String (rootNote).paddedLeft ('0', 3) + ".wav";
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
        const juce::String samplePath = "samples/" + sampleFileName (z.rootNote);

        text << "<region>\n";
        text << "sample=" << samplePath << "\n";
        text << "lokey=" << program.lowKeyForZone (i) << "\n";
        text << "hikey=" << program.highKeyForZone (i) << "\n";
        text << "pitch_keycenter=" << z.rootNote << "\n";

        if (z.hasLoop())
        {
            text << "loop_mode=loop_continuous\n";
            text << "loop_start=" << z.loopStart << "\n";
            text << "loop_end=" << z.loopEnd << "\n";
        }

        text << "\n";
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
        const juce::File wavFile = samplesDir.getChildFile (sampleFileName (zone.rootNote));
        std::unique_ptr<juce::FileOutputStream> stream (wavFile.createOutputStream());
        if (stream == nullptr)
        {
            error = "Could not write " + wavFile.getFileName();
            return false;
        }

        juce::AudioFormatWriter* writer = wav.createWriterFor (stream.get(), zone.sampleRate,
                                                                 static_cast<unsigned int> (zone.audio.getNumChannels()),
                                                                 24, {}, 0);

        if (writer == nullptr)
        {
            error = "WAV writer failed for " + wavFile.getFileName();
            return false;
        }

        stream.release();
        std::unique_ptr<juce::AudioFormatWriter> writerOwner (writer);
        writerOwner->writeFromAudioSampleBuffer (zone.audio, 0, zone.audio.getNumSamples());
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

    result.ok = true;
    result.presetJson = presetJson;
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
