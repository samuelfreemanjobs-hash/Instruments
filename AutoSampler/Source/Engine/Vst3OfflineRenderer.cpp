#include "Vst3OfflineRenderer.h"
#include "SamplePostProcessor.h"

namespace autosampler
{

bool Vst3OfflineRenderer::isSupportedOnThisPlatform() noexcept
{
#if JUCE_PLUGINHOST_VST3 && (JUCE_WINDOWS || JUCE_MAC || JUCE_LINUX)
    return true;
#else
    return false;
#endif
}

bool Vst3OfflineRenderer::captureProgram (const Vst3CaptureSpec& spec, MultisampleProgram& dest, juce::String& error)
{
#if ! (JUCE_PLUGINHOST_VST3 && (JUCE_WINDOWS || JUCE_MAC || JUCE_LINUX))
    juce::ignoreUnused (spec, dest);
    error = "VST3 hosting is not enabled in this build";
    return false;
#else
    if (! spec.vst3Path.exists())
    {
        error = "VST3 not found: " + spec.vst3Path.getFullPathName();
        return false;
    }

    juce::AudioPluginFormatManager formatManager;
    formatManager.addFormat (std::make_unique<juce::VST3PluginFormat>());

    juce::OwnedArray<juce::PluginDescription> typesFound;
    if (auto* vst3 = formatManager.getFormat (0))
        vst3->findAllTypesForFile (typesFound, spec.vst3Path.getFullPathName());

    if (typesFound.isEmpty())
    {
        error = "No VST3 types in file";
        return false;
    }

    juce::String loadError;
    const double sampleRate = 48000.0;
    constexpr int blockSize = 512;

    std::unique_ptr<juce::AudioPluginInstance> instance (
        formatManager.createPluginInstance (*typesFound.getFirst(), sampleRate, blockSize, loadError));

    if (instance == nullptr)
    {
        error = loadError;
        return false;
    }

    instance->setNonRealtime (true);
    instance->prepareToPlay (sampleRate, blockSize);

    if (spec.pluginProgramIndex >= 0 && spec.pluginProgramIndex < instance->getNumPrograms())
        instance->setCurrentProgram (spec.pluginProgramIndex);

    const auto& cfg = spec.captureConfig;
    const int lo = juce::jlimit (0, 127, cfg.lowNote);
    const int hi = juce::jlimit (lo, 127, cfg.highNote);
    const int step = juce::jmax (1, cfg.noteStep);
    const int totalSamples = static_cast<int> (std::ceil (cfg.recordSeconds * sampleRate));
    const int noteOffSample = std::max (1, static_cast<int> (0.9 * static_cast<double> (totalSamples)));

    dest.clear();
    dest.name = spec.presetName.isNotEmpty() ? spec.presetName : typesFound.getFirst()->name;

    const int outChannels = juce::jmax (1, instance->getTotalNumOutputChannels());

    for (int note = lo; note <= hi; note += step)
    {
        Zone zone;
        zone.rootNote = note;
        zone.sampleRate = sampleRate;
        zone.audio.setSize (outChannels, totalSamples);
        zone.audio.clear();

        juce::AudioBuffer<float> buffer (outChannels, blockSize);
        juce::MidiBuffer midi;

        for (int offset = 0; offset < totalSamples; offset += blockSize)
        {
            const int numThisBlock = std::min (blockSize, totalSamples - offset);
            buffer.setSize (outChannels, numThisBlock, false, false, true);
            buffer.clear();
            midi.clear();

            if (offset == 0)
                midi.addEvent (juce::MidiMessage::noteOn (1, note, (juce::uint8) cfg.velocity), 0);
            if (offset <= noteOffSample && noteOffSample < offset + numThisBlock)
                midi.addEvent (juce::MidiMessage::noteOff (1, note), noteOffSample - offset);

            instance->processBlock (buffer, midi);

            for (int ch = 0; ch < outChannels; ++ch)
                zone.audio.copyFrom (ch, offset, buffer, ch, 0, numThisBlock);
        }

        SamplePostProcessor::processZone (zone, cfg.postProcess);
        dest.addZone (std::move (zone));
    }

    if (dest.empty())
    {
        error = "Capture produced no zones";
        return false;
    }

    return true;
#endif
}

} // namespace autosampler
