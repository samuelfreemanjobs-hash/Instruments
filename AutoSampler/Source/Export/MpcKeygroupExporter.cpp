#include "MpcKeygroupExporter.h"
#include "MpcKeyzoneValidator.h"

namespace autosampler
{

juce::String MpcKeygroupExporter::mpcSampleBaseName (int rootNote, int velStart)
{
    auto base = "note_" + juce::String (rootNote).paddedLeft ('0', 3);
    if (velStart > 0 && velStart < 127)
        base << "_v" << juce::String (velStart).paddedLeft ('0', 3);
    return base;
}

juce::String MpcKeygroupExporter::escapeXml (const juce::String& text)
{
    return text.replace ("&", "&amp;")
        .replace ("<", "&lt;")
        .replace (">", "&gt;")
        .replace ("\"", "&quot;");
}

static void writeLayerWav (const VelocityLayer& layer, double sampleRate, const juce::File& destWav)
{
    std::unique_ptr<juce::FileOutputStream> stream (destWav.createOutputStream());
    if (stream == nullptr)
        return;

    juce::WavAudioFormat wav;
    juce::AudioFormatWriter* writer = wav.createWriterFor (stream.get(), sampleRate,
                                                             static_cast<unsigned int> (layer.audio.getNumChannels()),
                                                             24, {}, 0);
    if (writer == nullptr)
        return;

    stream.release();
    std::unique_ptr<juce::AudioFormatWriter> owner (writer);
    owner->writeFromAudioSampleBuffer (layer.audio, 0, layer.audio.getNumSamples());
}

static juce::String layerBlockV2 (const Zone& zone,
                                    int layerNumber,
                                    const VelocityLayer& layer,
                                    const juce::String& sampleFileName)
{
    const bool hasLoop = layer.hasLoop();

    juce::String out;
    out << "        <Layer number=\"" << layerNumber << "\">\n";
    out << "          <SampleFile>" << MpcKeygroupExporter::escapeXml (sampleFileName) << "</SampleFile>\n";
    out << "          <RootNote>" << zone.rootNote << "</RootNote>\n";
    out << "          <VelStart>" << layer.velStart << "</VelStart>\n";
    out << "          <VelEnd>" << layer.velEnd << "</VelEnd>\n";
    out << "          <SampleStart>0</SampleStart>\n";
    out << "          <SampleEnd>-1</SampleEnd>\n";

    if (hasLoop)
    {
        out << "          <LoopStart>" << layer.loopStart << "</LoopStart>\n";
        out << "          <LoopEnd>" << layer.loopEnd << "</LoopEnd>\n";
        out << "          <Looping>1</Looping>\n";
    }
    else
    {
        out << "          <LoopStart>-1</LoopStart>\n";
        out << "          <LoopEnd>-1</LoopEnd>\n";
        out << "          <Looping>0</Looping>\n";
    }

    out << "          <Bidirectional>0</Bidirectional>\n";
    out << "          <Reverse>0</Reverse>\n";
    out << "          <VolumeOffset>0</VolumeOffset>\n";
    out << "          <PanOffset>0</PanOffset>\n";
    out << "          <TuneOffset>0</TuneOffset>\n";
    out << "        </Layer>\n";
    return out;
}

static juce::String instrumentBlockV2 (const MultisampleProgram& program, std::size_t zoneIndex, int instrumentNumber)
{
    const auto& z = program.zones()[zoneIndex];
    const int low = program.lowKeyForZone (zoneIndex);
    const int high = program.highKeyForZone (zoneIndex);
    const auto layers = z.velocityLayersForExport();

    juce::String xml;
    xml << "      <Instrument number=\"" << instrumentNumber << "\">\n";
    xml << "        <Polyphony>32</Polyphony>\n";
    xml << "        <LowNote>" << low << "</LowNote>\n";
    xml << "        <HighNote>" << high << "</HighNote>\n";
    xml << "        <Layers>\n";

    int layerNum = 1;
    for (const auto& layer : layers)
    {
        const auto sampleFile = MpcKeygroupExporter::mpcSampleBaseName (z.rootNote, layer.velStart) + ".wav";
        xml << layerBlockV2 (z, layerNum++, layer, sampleFile);
    }

    xml << "        </Layers>\n";
    xml << "      </Instrument>\n";
    return xml;
}

bool MpcKeygroupExporter::exportKeygroup (const MultisampleProgram& program, const juce::File& destinationDir, juce::String& error)
{
    const auto validation = MpcKeyzoneValidator::validateProgram (program);
    if (! validation.ok)
    {
        error = validation.errors.joinIntoString ("; ");
        return false;
    }

    if (program.empty())
    {
        error = "Program has no zones";
        return false;
    }

    const juce::String safeName = program.name.replaceCharacter ('/', '_').trim();
    const juce::File programDir = destinationDir.getChildFile (safeName);
    if (! programDir.createDirectory())
    {
        error = "Could not create MPC program directory";
        return false;
    }

    for (const auto& z : program.zones())
    {
        for (const auto& layer : z.velocityLayersForExport())
        {
            const auto base = mpcSampleBaseName (z.rootNote, layer.velStart);
            writeLayerWav (layer, z.sampleRate, programDir.getChildFile (base + ".wav"));
        }
    }

    const int numKeygroups = static_cast<int> (program.zoneCount());
    juce::String xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<MPCVObject>\n";
    xml << "  <Version>\n";
    xml << "    <File_Version>" << kFileVersion << "</File_Version>\n";
    xml << "    <Application>MPC-V</Application>\n";
    xml << "    <Application_Version>" << kAppVersion << "</Application_Version>\n";
    xml << "    <Platform>AutoSampler</Platform>\n";
    xml << "  </Version>\n";
    xml << "  <Program type=\"Keygroup\">\n";
    xml << "    <ProgramName>" << escapeXml (program.name) << "</ProgramName>\n";
    xml << "    <KeygroupNumKeygroups>" << numKeygroups << "</KeygroupNumKeygroups>\n";
    xml << "    <KeygroupMasterTranspose>0</KeygroupMasterTranspose>\n";
    xml << "    <Instruments>\n";

    int instrumentNumber = 1;
    for (std::size_t i = 0; i < program.zoneCount(); ++i)
        xml << instrumentBlockV2 (program, i, instrumentNumber++);

    xml << "    </Instruments>\n";
    xml << "  </Program>\n";
    xml << "</MPCVObject>\n";

    const juce::File xpmFile = programDir.getChildFile (safeName + ".xpm");
    if (! xpmFile.replaceWithText (xml))
    {
        error = "Could not write .xpm";
        return false;
    }

    return true;
}

} // namespace autosampler
