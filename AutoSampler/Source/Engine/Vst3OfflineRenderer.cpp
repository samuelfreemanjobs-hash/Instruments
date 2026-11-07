#include "Vst3OfflineRenderer.h"
#include "AutoSampleSessionPlan.h"
#include "NoteMapBuilder.h"
#include "SamplePostProcessor.h"
#include "Vst3PluginCache.h"

#include <chrono>

namespace autosampler
{

namespace
{

static std::unique_ptr<juce::AudioPluginInstance> loadInstance (const juce::File& vst3Path,
                                                                  juce::AudioPluginFormatManager& formatManager,
                                                                  double sampleRate,
                                                                  int blockSize,
                                                                  Vst3PluginCache* cache,
                                                                  juce::String& error)
{
    if (auto* vst3 = formatManager.getFormat (0))
    {
        juce::OwnedArray<juce::PluginDescription> typesFound;
        vst3->findAllTypesForFile (typesFound, vst3Path.getFullPathName());
        if (typesFound.isEmpty())
        {
            error = "No VST3 types in file";
            if (cache != nullptr)
                cache->markFailed (vst3Path);
            return nullptr;
        }

        juce::String loadError;
        auto instance = std::unique_ptr<juce::AudioPluginInstance> (
            formatManager.createPluginInstance (*typesFound.getFirst(), sampleRate, blockSize, loadError));

        if (instance == nullptr)
        {
            error = loadError;
            if (cache != nullptr)
                cache->markFailed (vst3Path);
            return nullptr;
        }

        if (cache != nullptr)
            cache->markOk (vst3Path);

        return instance;
    }

    error = "VST3 format unavailable";
    return nullptr;
}

static int captureLengthSamples (const AutoRecordEngine::Config& cfg, double sampleRate)
{
    const double sec = AutoSampleSessionPlanBuilder::captureLengthSeconds (cfg);
    return static_cast<int> (std::ceil (sec * sampleRate));
}

static void renderNote (juce::AudioPluginInstance& instance,
                          int note,
                          int velocity,
                          const AutoRecordEngine::Config& cfg,
                          double sampleRate,
                          int blockSize,
                          int outChannels,
                          Zone& destZone,
                          std::chrono::steady_clock::time_point deadline)
{
    const int prerollSamples = static_cast<int> (std::ceil ((cfg.prerollMs / 1000.0) * sampleRate));
    const int holdSamples = cfg.recordUntilSilence
                                ? static_cast<int> (std::ceil ((cfg.noteHoldMs / 1000.0) * sampleRate))
                                : static_cast<int> (std::ceil ((cfg.noteHoldMs / 1000.0) * sampleRate));
    const int tailSamples = cfg.recordUntilSilence
                                  ? static_cast<int> (std::ceil (cfg.maxRecordSeconds * sampleRate))
                                  : static_cast<int> (std::ceil ((cfg.tailMs / 1000.0) * sampleRate));
    const int totalSamples = cfg.recordUntilSilence
                                 ? tailSamples
                                 : prerollSamples + holdSamples + tailSamples;
    const int noteOnSample = cfg.recordUntilSilence ? 0 : prerollSamples;
    const int noteOffSample = cfg.recordUntilSilence
                                  ? juce::jmin (holdSamples, totalSamples - 1)
                                  : prerollSamples + holdSamples;
    const int midiChannel = juce::jlimit (1, 16, cfg.midiChannel);

    VelocityLayer layer;
    layer.velStart = 0;
    layer.velEnd = 127;
    layer.audio.setSize (outChannels, totalSamples);
    layer.audio.clear();

    juce::AudioBuffer<float> buffer (outChannels, blockSize);
    juce::MidiBuffer midi;

    const float silenceThresh = juce::Decibels::decibelsToGain (cfg.silenceThresholdDb);
    const int silenceHoldSamples = static_cast<int> (cfg.silenceHoldSeconds * sampleRate);
    int trailingSilent = 0;
    bool noteOffSent = false;
    int written = 0;

    for (int offset = 0; offset < totalSamples; offset += blockSize)
    {
        if (std::chrono::steady_clock::now() > deadline)
            break;

        const int numThisBlock = std::min (blockSize, totalSamples - offset);
        buffer.setSize (outChannels, numThisBlock, false, false, true);
        buffer.clear();
        midi.clear();

        if (offset <= noteOnSample && noteOnSample < offset + numThisBlock)
            midi.addEvent (juce::MidiMessage::noteOn (midiChannel, note, (juce::uint8) velocity), noteOnSample - offset);
        if (! noteOffSent && offset <= noteOffSample && noteOffSample < offset + numThisBlock)
        {
            midi.addEvent (juce::MidiMessage::noteOff (midiChannel, note), noteOffSample - offset);
            noteOffSent = true;
        }

        instance.processBlock (buffer, midi);

        float blockPeak = 0.0f;
        for (int ch = 0; ch < outChannels; ++ch)
        {
            layer.audio.copyFrom (ch, offset, buffer, ch, 0, numThisBlock);
            blockPeak = juce::jmax (blockPeak, buffer.getMagnitude (ch, 0, numThisBlock));
        }

        written = offset + numThisBlock;

        if (cfg.recordUntilSilence && noteOffSent)
        {
            if (blockPeak < silenceThresh)
                trailingSilent += numThisBlock;
            else
                trailingSilent = 0;

            if (trailingSilent >= silenceHoldSamples)
                break;
        }
    }

    layer.audio.setSize (outChannels, written, true, false, true);

    Zone scratch;
    scratch.rootNote = note;
    scratch.sampleRate = sampleRate;
    scratch.audio = layer.audio;
    SamplePostProcessor::processZone (scratch, cfg.postProcess);
    layer.audio = std::move (scratch.audio);
    layer.loopStart = scratch.loopStart;
    layer.loopEnd = scratch.loopEnd;
    layer.loopConfidence = scratch.loopConfidence;

    destZone.rootNote = note;
    destZone.sampleRate = sampleRate;
    destZone.addVelocityLayer (std::move (layer));
}

static std::pair<int, int> velocitySpanForLayer (int layerIndex, int layerCount)
{
    if (layerCount <= 1)
        return { 0, 127 };
    const int velStart = layerIndex == 0 ? 0 : (layerIndex * 127) / layerCount;
    const int velEnd = layerIndex + 1 == layerCount ? 127 : ((layerIndex + 1) * 127) / layerCount - 1;
    return { velStart, juce::jmax (velStart, velEnd) };
}

} // namespace

bool Vst3OfflineRenderer::isSupportedOnThisPlatform() noexcept
{
#if JUCE_PLUGINHOST_VST3 && (JUCE_WINDOWS || JUCE_MAC || JUCE_LINUX)
    return true;
#else
    return false;
#endif
}

bool Vst3OfflineRenderer::captureProgram (const Vst3CaptureSpec& spec,
                                            MultisampleProgram& dest,
                                            juce::String& error,
                                            Vst3PluginCache* cache,
                                            double timeoutSeconds)
{
#if ! (JUCE_PLUGINHOST_VST3 && (JUCE_WINDOWS || JUCE_MAC || JUCE_LINUX))
    juce::ignoreUnused (spec, dest, cache, timeoutSeconds);
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

    const double sampleRate = 48000.0;
    constexpr int blockSize = 512;
    const auto deadline = std::chrono::steady_clock::now()
                            + std::chrono::milliseconds (static_cast<int> (timeoutSeconds * 1000.0));

    auto instance = loadInstance (spec.vst3Path, formatManager, sampleRate, blockSize, cache, error);
    if (instance == nullptr)
        return false;

    instance->setNonRealtime (true);
    instance->prepareToPlay (sampleRate, blockSize);

    if (spec.pluginProgramIndex >= 0 && spec.pluginProgramIndex < instance->getNumPrograms())
        instance->setCurrentProgram (spec.pluginProgramIndex);

    const auto& cfg = spec.captureConfig;
    const auto noteList = NoteMapBuilder::buildNoteList (cfg.lowNote,
                                                           cfg.highNote,
                                                           cfg.noteStep,
                                                           cfg.noteMapPreset,
                                                           cfg.blackKeyPolicy);

    const auto velocities = AutoSampleSessionPlanBuilder::activeVelocities (cfg);

    dest.clear();
    dest.name = spec.presetName.isNotEmpty() ? spec.presetName : spec.vst3Path.getFileNameWithoutExtension();

    const int outChannels = juce::jmax (1, instance->getTotalNumOutputChannels());

    for (int note : noteList)
    {
        if (std::chrono::steady_clock::now() > deadline)
        {
            error = "Source capture timed out";
            return false;
        }

        Zone zone;
        for (int vi = 0; vi < static_cast<int> (velocities.size()); ++vi)
        {
            if (std::chrono::steady_clock::now() > deadline)
            {
                error = "Source capture timed out";
                return false;
            }

            const int vel = velocities[static_cast<std::size_t> (vi)];
            Zone passZone;
            renderNote (*instance, note, vel, cfg, sampleRate, blockSize, outChannels, passZone, deadline);

            if (passZone.velocityLayers.empty())
                continue;

            auto layer = std::move (passZone.velocityLayers.front());
            const auto [vLo, vHi] = velocitySpanForLayer (vi, static_cast<int> (velocities.size()));
            layer.velStart = vLo;
            layer.velEnd = vHi;

            if (dest.zoneIndexForRoot (note) >= 0)
                dest.zones()[static_cast<std::size_t> (dest.zoneIndexForRoot (note))].addVelocityLayer (std::move (layer));
            else
            {
                Zone z;
                z.rootNote = note;
                z.sampleRate = sampleRate;
                z.addVelocityLayer (std::move (layer));
                dest.addZone (std::move (z));
            }
        }
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
