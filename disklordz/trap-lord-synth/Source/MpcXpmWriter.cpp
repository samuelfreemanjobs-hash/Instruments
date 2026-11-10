#include "MpcXpmWriter.h"

namespace
{
juce::String xmlEscape (const juce::String& s)
{
    return s.replace ("&", "&amp;").replace ("<", "&lt;").replace (">", "&gt;").replace ("\"", "&quot;");
}
} // namespace

juce::String MpcXpmWriter::makeSafeName (const juce::String& input, int maxLen)
{
    juce::String out;
    for (auto c : input)
    {
        if (juce::CharacterFunctions::isLetterOrDigit (c) || c == '_' || c == '-')
            out << c;
        else if (c == ' ' || c == '/')
            out << '_';
    }
    if (out.isEmpty())
        out = "TrapLordProgram";
    return out.substring (0, maxLen);
}

bool MpcXpmWriter::writeKeygroupProgram (const juce::File& programFolder,
                                         const juce::String& programName,
                                         const std::vector<MpcSampleZone>& zones)
{
    if (zones.empty())
        return false;

    juce::String xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
    xml << "<KeygroupProgram>\n";
    xml << "  <Version>\n";
    xml << "    <FileVersion>5000</FileVersion>\n";
    xml << "  </Version>\n";
    xml << "  <Program>\n";
    xml << "    <ProgramName>" << xmlEscape (programName) << "</ProgramName>\n";
    xml << "    <ProgramType>keygroup</ProgramType>\n";
    xml << "    <PitchBendRange>2.000000</PitchBendRange>\n";
    xml << "    <NumKeygroups>" << zones.size() << "</NumKeygroups>\n";
    xml << "    <Instruments>\n";

    for (const auto& zone : zones)
    {
        xml << "      <Instrument>\n";
        xml << "        <LowNote>" << zone.lowNote << "</LowNote>\n";
        xml << "        <HighNote>" << zone.highNote << "</HighNote>\n";
        xml << "        <VelocityToAmpAmount>1.000000</VelocityToAmpAmount>\n";
        xml << "        <VolumeAttack>0.000000</VolumeAttack>\n";
        xml << "        <VolumeDecay>0.500000</VolumeDecay>\n";
        xml << "        <VolumeSustain>1.000000</VolumeSustain>\n";
        xml << "        <VolumeRelease>0.250000</VolumeRelease>\n";
        xml << "        <TriggerMode>0</TriggerMode>\n";
        xml << "        <Layers>\n";
        xml << "          <Layer>\n";
        xml << "            <Active>true</Active>\n";
        xml << "            <Volume>0.800000</Volume>\n";
        xml << "            <Pan>0.000000</Pan>\n";
        xml << "            <Pitch>0.000000</Pitch>\n";
        xml << "            <VelStart>" << zone.velLow << "</VelStart>\n";
        xml << "            <VelEnd>" << zone.velHigh << "</VelEnd>\n";
        xml << "            <RootNote>" << (zone.rootNote + 1) << "</RootNote>\n";
        xml << "            <KeyTrack>true</KeyTrack>\n";
        xml << "            <SampleName>" << xmlEscape (zone.sampleName) << "</SampleName>\n";
        xml << "            <SampleStart>0</SampleStart>\n";
        xml << "            <SampleEnd>" << juce::jmax (0, zone.sampleFrames) << "</SampleEnd>\n";
        xml << "            <LoopStart>0</LoopStart>\n";
        xml << "            <LoopEnd>0</LoopEnd>\n";
        xml << "            <SampleFile>" << xmlEscape (zone.sampleName) << "</SampleFile>\n";
        xml << "          </Layer>\n";
        xml << "        </Layers>\n";
        xml << "      </Instrument>\n";
    }

    xml << "    </Instruments>\n";
    xml << "  </Program>\n";
    xml << "</KeygroupProgram>\n";

    const auto xpmFile = programFolder.getChildFile (programName + ".xpm");
    return xpmFile.replaceWithText (xml);
}
