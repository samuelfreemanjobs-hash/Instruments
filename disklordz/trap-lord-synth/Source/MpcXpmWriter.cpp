#include "MpcXpmWriter.h"

namespace
{
juce::String xmlEscape (const juce::String& s)
{
    return s.replace ("&", "&amp;").replace ("<", "&lt;").replace (">", "&gt;").replace ("\"", "&quot;");
}

juce::String fmt6 (double v)
{
    return juce::String (v, 6, false);
}
} // namespace

juce::String MpcXpmWriter::makeSafeName (const juce::String& input, int maxLen)
{
    juce::String out;
    for (auto c : input)
    {
        if (juce::CharacterFunctions::isLetterOrDigit (c) || c == '_')
            out << (juce::CharacterFunctions::isLowerCase (c) ? juce::CharacterFunctions::toUpperCase (c) : c);
        else if (c == ' ' || c == '-' || c == '/')
            out << '_';
    }
    while (out.contains ("__"))
        out = out.replace ("__", "_");
    if (out.isEmpty())
        out = "TRAP_PROGRAM";
    return out.substring (0, maxLen);
}

juce::String MpcXpmWriter::makeSampleFilename (int rootNote, int layerIndex)
{
    return "DL_N" + juce::String (rootNote).paddedLeft ('0', 3) + "_L" + juce::String (layerIndex + 1) + ".WAV";
}

bool MpcXpmWriter::writeKeygroupProgram (const juce::File& programFolder,
                                         const juce::String& programName,
                                         const std::vector<MpcKeygroupZone>& keygroups,
                                         float pitchBendRange)
{
    if (keygroups.empty())
        return false;

    juce::String xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
    xml << "<KeygroupProgram>\n";
    xml << "  <Version>\n";
    xml << "    <FileVersion>" << kFileVersion << "</FileVersion>\n";
    xml << "  </Version>\n";
    xml << "  <Program>\n";
    xml << "    <ProgramName>" << xmlEscape (programName) << "</ProgramName>\n";
    xml << "    <ProgramType>keygroup</ProgramType>\n";
    xml << "    <PitchBendRange>" << fmt6 (pitchBendRange) << "</PitchBendRange>\n";
    xml << "    <NumKeygroups>" << keygroups.size() << "</NumKeygroups>\n";
    xml << "    <Instruments>\n";

    for (const auto& kg : keygroups)
    {
        const int layerCount = juce::jlimit (0, 4, int (kg.layers.size()));
        if (layerCount == 0)
            continue;

        xml << "      <Instrument>\n";
        xml << "        <LowNote>" << kg.lowNote << "</LowNote>\n";
        xml << "        <HighNote>" << kg.highNote << "</HighNote>\n";
        xml << "        <IgnoreBaseNote>False</IgnoreBaseNote>\n";
        xml << "        <VelocityToAmpAmount>1.000000</VelocityToAmpAmount>\n";
        xml << "        <VolumeAttack>" << fmt6 (kg.volumeAttack) << "</VolumeAttack>\n";
        xml << "        <VolumeHold>0.000000</VolumeHold>\n";
        xml << "        <VolumeDecay>" << fmt6 (kg.volumeDecay) << "</VolumeDecay>\n";
        xml << "        <VolumeSustain>" << fmt6 (kg.volumeSustain) << "</VolumeSustain>\n";
        xml << "        <VolumeRelease>" << fmt6 (kg.volumeRelease) << "</VolumeRelease>\n";
        xml << "        <TriggerMode>0</TriggerMode>\n";
        xml << "        <Layers>\n";

        for (int li = 0; li < layerCount; ++li)
        {
            const auto& layer = kg.layers[size_t (li)];
            xml << "          <Layer>\n";
            xml << "            <Active>true</Active>\n";
            xml << "            <Volume>0.800000</Volume>\n";
            xml << "            <Pan>0.000000</Pan>\n";
            xml << "            <Pitch>0.000000</Pitch>\n";
            xml << "            <VelStart>" << layer.velLow << "</VelStart>\n";
            xml << "            <VelEnd>" << layer.velHigh << "</VelEnd>\n";
            xml << "            <RootNote>" << (layer.rootNote + 1) << "</RootNote>\n";
            xml << "            <KeyTrack>true</KeyTrack>\n";
            xml << "            <SampleName>" << xmlEscape (layer.sampleName) << "</SampleName>\n";
            xml << "            <SampleStart>0</SampleStart>\n";
            xml << "            <SampleEnd>" << juce::jmax (0, layer.sampleFrames) << "</SampleEnd>\n";
            xml << "            <LoopStart>" << juce::jmax (0, layer.loopStart) << "</LoopStart>\n";
            xml << "            <LoopEnd>" << juce::jmax (0, layer.loopEnd) << "</LoopEnd>\n";
            xml << "            <SampleFile>" << xmlEscape (layer.sampleName) << "</SampleFile>\n";
            xml << "          </Layer>\n";
        }

        xml << "        </Layers>\n";
        xml << "      </Instrument>\n";
    }

    xml << "    </Instruments>\n";
    xml << "  </Program>\n";
    xml << "</KeygroupProgram>\n";

    const auto xpmFile = programFolder.getChildFile (programName + ".xpm");
    return xpmFile.replaceWithText (xml);
}
