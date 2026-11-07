#include "MpcKeygroupExporter.h"

namespace autosampler
{

juce::String MpcKeygroupExporter::mpcSampleBaseName (int rootNote)
{
    return "note_" + juce::String (rootNote).paddedLeft ('0', 3);
}

juce::String MpcKeygroupExporter::escapeXml (const juce::String& text)
{
    return text.replace ("&", "&amp;")
        .replace ("<", "&lt;")
        .replace (">", "&gt;")
        .replace ("\"", "&quot;");
}

static void writeWavFile (const Zone& zone, const juce::File& destWav)
{
    std::unique_ptr<juce::FileOutputStream> stream (destWav.createOutputStream());
    if (stream == nullptr)
        return;

    juce::WavAudioFormat wav;
    juce::AudioFormatWriter* writer = wav.createWriterFor (stream.get(), zone.sampleRate,
                                                             static_cast<unsigned int> (zone.audio.getNumChannels()),
                                                             24, {}, 0);
    if (writer == nullptr)
        return;

    stream.release();
    std::unique_ptr<juce::AudioFormatWriter> owner (writer);
    owner->writeFromAudioSampleBuffer (zone.audio, 0, zone.audio.getNumSamples());
}

static juce::String layerBlockV2 (const Zone& zone, const juce::String& sampleFileName)
{
    const bool hasLoop = zone.hasLoop();

    juce::String layer;
    layer << "        <Layer number=\"1\">\n";
    layer << "          <SampleFile>" << MpcKeygroupExporter::escapeXml (sampleFileName) << "</SampleFile>\n";
    layer << "          <RootNote>" << zone.rootNote << "</RootNote>\n";
    layer << "          <VelStart>0</VelStart>\n";
    layer << "          <VelEnd>127</VelEnd>\n";
    layer << "          <SampleStart>0</SampleStart>\n";
    layer << "          <SampleEnd>-1</SampleEnd>\n";

    if (hasLoop)
    {
        layer << "          <LoopStart>" << zone.loopStart << "</LoopStart>\n";
        layer << "          <LoopEnd>" << zone.loopEnd << "</LoopEnd>\n";
        layer << "          <Looping>1</Looping>\n";
    }
    else
    {
        layer << "          <LoopStart>-1</LoopStart>\n";
        layer << "          <LoopEnd>-1</LoopEnd>\n";
        layer << "          <Looping>0</Looping>\n";
    }

    layer << "          <Bidirectional>0</Bidirectional>\n";
    layer << "          <Reverse>0</Reverse>\n";
    layer << "          <VolumeOffset>0</VolumeOffset>\n";
    layer << "          <PanOffset>0</PanOffset>\n";
    layer << "          <TuneOffset>0</TuneOffset>\n";
    layer << "        </Layer>\n";
    return layer;
}

static juce::String instrumentBlockV2 (const MultisampleProgram& program,
                                        std::size_t zoneIndex,
                                        int instrumentNumber)
{
    const auto& z = program.zones()[zoneIndex];
    const int low = program.lowKeyForZone (zoneIndex);
    const int high = program.highKeyForZone (zoneIndex);
    const auto sampleFile = MpcKeygroupExporter::mpcSampleBaseName (z.rootNote) + ".wav";

    juce::String xml;
    xml << "      <Instrument number=\"" << instrumentNumber << "\">\n";
    xml << "        <Polyphony>32</Polyphony>\n";
    xml << "        <LowNote>" << low << "</LowNote>\n";
    xml << "        <HighNote>" << high << "</HighNote>\n";
    xml << "        <Layers>\n";
    xml << layerBlockV2 (z, sampleFile);
    xml << "        </Layers>\n";
    xml << "      </Instrument>\n";
    return xml;
}

bool MpcKeygroupExporter::exportKeygroup (const MultisampleProgram& program, const juce::File& destinationDir, juce::String& error)
{
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
        const auto base = mpcSampleBaseName (z.rootNote);
        writeWavFile (z, programDir.getChildFile (base + ".wav"));
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
