#include "Exs24Exporter.h"

namespace autosampler
{

static juce::String exsSampleName (int root, int velStart)
{
    if (velStart <= 0)
        return "note_" + juce::String (root).paddedLeft ('0', 3) + ".aif";
    return "note_" + juce::String (root).paddedLeft ('0', 3) + "_v" + juce::String (velStart).paddedLeft ('0', 3) + ".aif";
}

bool Exs24Exporter::exportInstrument (const MultisampleProgram& program, const juce::File& destinationDir, juce::String& error)
{
    if (program.empty())
    {
        error = "Program has no zones";
        return false;
    }

    const juce::String safeName = program.name.replaceCharacter ('/', '_').trim();
    const juce::File instrDir = destinationDir.getChildFile (safeName);
    const juce::File sampleDir = instrDir.getChildFile ("Samples");
    if (! sampleDir.createDirectory())
    {
        error = "Could not create EXS sample folder";
        return false;
    }

    juce::String exs;
    exs << "# EXS24 text export — " << program.name << "\n\n";

    for (std::size_t zi = 0; zi < program.zoneCount(); ++zi)
    {
        const auto& z = program.zones()[zi];
        const int lo = program.lowKeyForZone (zi);
        const int hi = program.highKeyForZone (zi);

        const auto layers = z.velocityLayersForExport();
        for (const auto& layer : layers)
        {
            const auto sampleName = exsSampleName (z.rootNote, layer.velStart);
            const juce::File wavOut = sampleDir.getChildFile (sampleName.replace (".aif", ".wav"));

            std::unique_ptr<juce::FileOutputStream> stream (wavOut.createOutputStream());
            if (stream == nullptr)
            {
                error = "Could not write " + wavOut.getFileName();
                return false;
            }

            juce::WavAudioFormat wav;
            juce::AudioFormatWriter* writer = wav.createWriterFor (stream.get(), z.sampleRate,
                                                                     static_cast<unsigned int> (layer.audio.getNumChannels()),
                                                                     24, {}, 0);
            if (writer == nullptr)
            {
                error = "WAV writer failed";
                return false;
            }

            stream.release();
            std::unique_ptr<juce::AudioFormatWriter> owned (writer);
            owned->writeFromAudioSampleBuffer (layer.audio, 0, layer.audio.getNumSamples());

            exs << "Group " << (zi + 1) << "\n";
            exs << "#KeyLo " << lo << "\n";
            exs << "#KeyHi " << hi << "\n";
            exs << "#VelLo " << layer.velStart << "\n";
            exs << "#VelHi " << layer.velEnd << "\n";
            exs << "#RootKey " << z.rootNote << "\n";
            exs << "#Sample " << sampleName << "\n";
            if (layer.hasLoop())
            {
                exs << "#LoopStart " << layer.loopStart << "\n";
                exs << "#LoopEnd " << layer.loopEnd << "\n";
            }
            exs << "\n";
        }
    }

    const juce::File exsFile = instrDir.getChildFile (safeName + ".exs");
    if (! exsFile.replaceWithText (exs))
    {
        error = "Could not write .exs";
        return false;
    }

    return true;
}

} // namespace autosampler
