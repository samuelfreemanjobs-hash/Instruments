#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace
{

void printUsage()
{
    std::cerr
        << "Usage: Vst3OfflineRender --plugin <pathToVst3BundleOrFile> --out <output.wav>\n"
        << "       [--program n] [--note n] [--velocity n] [--seconds n] [--sampleRate hz]\n"
        << "       [--blockSize n] [--tailFraction 0..1]\n"
        << "  Defaults: program=0 note=60 velocity=100 seconds=3.0 sampleRate=48000 blockSize=512 tailFraction=0.85\n";
}

std::string normalisePluginPath (std::string path)
{
    const auto lower = path;
    auto findExt = [&path] (const char* ext) -> std::size_t
    {
        const auto pos = path.find (ext);
        if (pos == std::string::npos)
            return std::string::npos;
        return pos;
    };

    std::size_t start = findExt (".vst3");
    if (start == std::string::npos)
        start = findExt (".vst");
    if (start == std::string::npos)
        start = findExt (".component");

    if (start != std::string::npos)
    {
        const auto slash = path.find_first_of ("/\\", start);
        if (slash != std::string::npos)
            path = path.substr (0, slash);
    }

    return path;
}

} // namespace

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    juce::String pluginPath;
    juce::String outPath;
    int program = 0;
    int midiNote = 60;
    int velocity = 100;
    double seconds = 3.0;
    double sampleRate = 48000.0;
    int blockSize = 512;
    double tailFraction = 0.85;

    for (int i = 1; i < argc; ++i)
    {
        const juce::String arg (argv[i]);
        if (arg == "--plugin" && i + 1 < argc)
            pluginPath = argv[++i];
        else if (arg == "--out" && i + 1 < argc)
            outPath = argv[++i];
        else if (arg == "--program" && i + 1 < argc)
            program = std::atoi (argv[++i]);
        else if (arg == "--note" && i + 1 < argc)
            midiNote = std::atoi (argv[++i]);
        else if (arg == "--velocity" && i + 1 < argc)
            velocity = std::atoi (argv[++i]);
        else if (arg == "--seconds" && i + 1 < argc)
            seconds = juce::String (argv[++i]).getDoubleValue();
        else if (arg == "--sampleRate" && i + 1 < argc)
            sampleRate = juce::String (argv[++i]).getDoubleValue();
        else if (arg == "--blockSize" && i + 1 < argc)
            blockSize = std::atoi (argv[++i]);
        else if (arg == "--tailFraction" && i + 1 < argc)
            tailFraction = juce::String (argv[++i]).getDoubleValue();
        else
        {
            std::cerr << "Unknown or incomplete argument: " << arg << '\n';
            printUsage();
            return 1;
        }
    }

    if (pluginPath.isEmpty() || outPath.isEmpty())
    {
        printUsage();
        return 1;
    }

    if (seconds <= 0.0 || sampleRate <= 0.0 || blockSize <= 0 || tailFraction <= 0.0 || tailFraction > 1.0)
    {
        std::cerr << "Invalid timing or block parameters\n";
        return 1;
    }

    if (midiNote < 0 || midiNote > 127 || velocity < 1 || velocity > 127)
    {
        std::cerr << "Invalid MIDI note or velocity\n";
        return 1;
    }

    const auto pluginFile = juce::File (normalisePluginPath (pluginPath.toStdString()));
    if (! pluginFile.exists())
    {
        std::cerr << "Plugin path not found: " << pluginFile.getFullPathName() << '\n';
        return 1;
    }

    juce::AudioPluginFormatManager formatManager;
    juce::addHeadlessDefaultFormatsToManager (formatManager);

    juce::PluginDescription desc;
    bool found = false;

    for (int fmtIdx = 0; fmtIdx < formatManager.getNumFormats(); ++fmtIdx)
    {
        auto* format = formatManager.getFormat (fmtIdx);
        if (format == nullptr)
            continue;

        juce::OwnedArray<juce::PluginDescription> typesFound;
        juce::KnownPluginList list;
        list.scanAndAddFile (pluginFile.getFullPathName(), true, typesFound, *format);

        if (typesFound.isEmpty())
            continue;

        desc = *typesFound.getFirst();
        found = true;
        break;
    }

    if (! found)
    {
        std::cerr << "No plugin description found for " << pluginFile.getFullPathName() << '\n';
        return 1;
    }

    juce::String loadError;
    std::unique_ptr<juce::AudioPluginInstance> instance (
        formatManager.createPluginInstance (desc, sampleRate, blockSize, loadError));

    if (instance == nullptr)
    {
        std::cerr << "Failed to load plugin: " << loadError << '\n';
        return 1;
    }

    if (program >= 0 && program < instance->getNumPrograms())
        instance->setCurrentProgram (program);

    instance->prepareToPlay (sampleRate, blockSize);

    const int totalSamples = static_cast<int> (std::ceil (seconds * sampleRate));
    const int noteOffSample = std::max (1, static_cast<int> (tailFraction * static_cast<double> (totalSamples)));

    juce::AudioBuffer<float> buffer (2, blockSize);
    juce::MidiBuffer midi;
    juce::AudioBuffer<float> capture (2, totalSamples);
    capture.clear();

    for (int offset = 0; offset < totalSamples; offset += blockSize)
    {
        const int numThisBlock = std::min (blockSize, totalSamples - offset);
        buffer.setSize (2, numThisBlock, false, false, true);
        buffer.clear();
        midi.clear();

        if (offset == 0)
            midi.addEvent (juce::MidiMessage::noteOn (1, midiNote, static_cast<juce::uint8> (velocity)), 0);
        if (offset <= noteOffSample && noteOffSample < offset + numThisBlock)
            midi.addEvent (juce::MidiMessage::noteOff (1, midiNote), noteOffSample - offset);

        instance->processBlock (buffer, midi);

        for (int ch = 0; ch < capture.getNumChannels(); ++ch)
            capture.copyFrom (ch, offset, buffer, ch, 0, numThisBlock);
    }

    const juce::File outFile (outPath);
    outFile.getParentDirectory().createDirectory();

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> stream (outFile.createOutputStream());
    if (stream == nullptr)
    {
        std::cerr << "Cannot open output file\n";
        return 1;
    }

    std::unique_ptr<juce::AudioFormatWriter> writer (
        wav.createWriterFor (stream.get(), sampleRate, static_cast<unsigned int> (capture.getNumChannels()),
                             24, {}, 0));

    if (writer == nullptr)
    {
        std::cerr << "Failed to create WAV writer\n";
        return 1;
    }

    stream.release();
    if (! writer->writeFromAudioSampleBuffer (capture, 0, totalSamples))
    {
        std::cerr << "WAV write failed\n";
        return 1;
    }

    std::cout << "Wrote " << totalSamples << " samples → " << outFile.getFullPathName() << '\n';
    return 0;
}
