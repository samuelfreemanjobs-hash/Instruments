#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIds.h"
#include <cmath>

namespace huckaby
{

namespace
{
juce::String trackParamId(int trackIndex, const char* suffix)
{
    return pid::trackField(trackIndex, suffix);
}

float dbToLinear(float decibels) noexcept
{
    return juce::Decibels::decibelsToGain(decibels, -100.0f);
}

void bitsToTriggers(uint64_t bits, TrackState& t) noexcept
{
    t.triggers.fill(0);
    const int max = juce::jmin(t.activeStepLength, kMaxSteps);
    for (int s = 0; s < max; ++s)
        t.triggers[static_cast<size_t>(s)] = static_cast<uint8_t>((bits >> s) & 1ull);

    for (int s = max; s < kMaxSteps; ++s)
        t.triggers[static_cast<size_t>(s)] = 0;
}

int countLoadedSamples(TrackState& t) noexcept
{
    int count = 0;
    for (int i = 0; i < kMaxBankSize; ++i)
        if (t.sampleBank[static_cast<size_t>(i)].load() != nullptr)
            count = i + 1;
    return count;
}
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout HuckabyAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "master_gain_db", 1 },
        "Master Gain",
        juce::NormalisableRange<float> { -48.0f, 12.0f, 0.1f },
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "internal_bpm", 1 },
        "Internal BPM",
        juce::NormalisableRange<float> { 40.0f, 240.0f, 0.01f, 0.35f },
        120.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "internal_play", 1 },
        "Play (internal)",
        false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "internal_transport", 1 },
        "Use internal transport",
        true));

    for (int t = 0; t < kNumTracks; ++t)
    {
        const juce::String idLen = trackParamId(t, "len");
        const juce::String idMode = trackParamId(t, "mode");
        const juce::String idBankStatic = trackParamId(t, "bank_static");
        const juce::String idModDiv = trackParamId(t, "mod_div");

        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID { idLen, 1 },
            "Track " + juce::String(t + 1) + " length (steps)",
            1,
            64,
            (t == 0 ? 16 : t == 1 ? 12 : t == 2 ? 7 : t == 3 ? 5 : 16)));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { idMode, 1 },
            "Track " + juce::String(t + 1) + " bank mode",
            juce::StringArray { "Static", "Modulo cycle", "Random" },
            0));

        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID { idBankStatic, 1 },
            "Track " + juce::String(t + 1) + " static bank",
            0,
            kMaxBankSize - 1,
            0));

        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID { idModDiv, 1 },
            "Track " + juce::String(t + 1) + " modulo divisor",
            1,
            64,
            1));
    }

    return layout;
}

void HuckabyAudioProcessor::refreshParameterHandles()
{
    outputGainDbParam = apvts.getRawParameterValue("master_gain_db");
    internalBpmParam = apvts.getRawParameterValue("internal_bpm");
    internalPlayParam = apvts.getRawParameterValue("internal_play");
    useInternalTransportParam = apvts.getRawParameterValue("internal_transport");

    for (int t = 0; t < kNumTracks; ++t)
    {
        trackParams[static_cast<size_t>(t)].length = apvts.getRawParameterValue(trackParamId(t, "len"));
        trackParams[static_cast<size_t>(t)].mode = apvts.getRawParameterValue(trackParamId(t, "mode"));
        trackParams[static_cast<size_t>(t)].staticBank = apvts.getRawParameterValue(trackParamId(t, "bank_static"));
        trackParams[static_cast<size_t>(t)].modulo = apvts.getRawParameterValue(trackParamId(t, "mod_div"));
    }
}

HuckabyAudioProcessor::HuckabyAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "HuckabyParams", createParameterLayout())
{
    refreshParameterHandles();

    installDefaultPatterns(tracks);

    for (int t = 0; t < kNumTracks; ++t)
        patternBits[static_cast<size_t>(t)].store(0, std::memory_order_relaxed);

    // Encode the built-in demo pattern into bits for tracks 0..3 (16-step lanes).
    for (int t = 0; t < kNumTracks; ++t)
    {
        uint64_t bits = 0;
        for (int s = 0; s < kMaxSteps; ++s)
            if (tracks[static_cast<size_t>(t)].triggers[static_cast<size_t>(s)] != 0)
                bits |= (1ull << s);
        patternBits[static_cast<size_t>(t)].store(bits, std::memory_order_relaxed);
    }

    for (auto& t : tracks)
        t.numSamplesLoaded = countLoadedSamples(t);
}

HuckabyAudioProcessor::~HuckabyAudioProcessor()
{
    loadPool.removeAllJobs(true, 60000);
}

void HuckabyAudioProcessor::prepareToPlay(double sampleRate, int)
{
    hostSampleRate = sampleRate;

    currentBpm = internalBpmParam != nullptr ? static_cast<double>(internalBpmParam->load()) : 120.0;
    if (currentBpm < 1.0)
        currentBpm = 120.0;

    samplesPerStep = (hostSampleRate * 60.0) / (currentBpm * 4.0);
    sampleCounter = 0.0;
}

void HuckabyAudioProcessor::releaseResources() {}

bool HuckabyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

bool HuckabyAudioProcessor::isEffectivelyPlaying() const noexcept
{
    const bool internalOn = useInternalTransportParam != nullptr && useInternalTransportParam->load() >= 0.5f;
    if (internalOn)
        return internalPlayParam != nullptr && internalPlayParam->load() >= 0.5f;

    return hostIsPlaying;
}

void HuckabyAudioProcessor::updateTransportAndTempo()
{
    hostIsPlaying = false;

    if (auto* hostPlayHead = getPlayHead())
    {
        const auto pos = hostPlayHead->getPosition();
        if (pos.hasValue())
        {
            if (const auto bpmOpt = pos->getBpm(); bpmOpt.hasValue())
            {
                const double bpm = *bpmOpt;
                if (bpm > 0.0)
                    currentBpm = bpm;
            }

            hostIsPlaying = pos->getIsPlaying();
        }
    }

    const bool internalOn = useInternalTransportParam != nullptr && useInternalTransportParam->load() >= 0.5f;
    if (internalOn && internalBpmParam != nullptr)
    {
        const double bpm = static_cast<double>(internalBpmParam->load());
        if (bpm > 0.0)
            currentBpm = bpm;
    }

    if (currentBpm < 1.0)
        currentBpm = 120.0;

    samplesPerStep = (hostSampleRate * 60.0) / (currentBpm * 4.0);
}

void HuckabyAudioProcessor::reconcilePlaybackStartStopEdge()
{
    const bool playing = isEffectivelyPlaying();
    if (playing && !lastEffectivePlaying)
    {
        globalTickIndex = 0;
        sampleCounter = 0.0;
        for (auto& t : tracks)
            t.resetCounters();
    }

    lastEffectivePlaying = playing;
}

void HuckabyAudioProcessor::pullPatternBitsIntoTracks() noexcept
{
    for (int t = 0; t < kNumTracks; ++t)
    {
        auto& tr = tracks[static_cast<size_t>(t)];
        tr.activeStepLength = getTrackLengthSteps(t);
        const uint64_t bits = patternBits[static_cast<size_t>(t)].load(std::memory_order_relaxed);
        bitsToTriggers(bits, tr);
        tr.numSamplesLoaded = countLoadedSamples(tr);

        tr.selectionMode = getTrackMode(t);
        tr.staticBankIndex = getStaticBankIndex(t);
        tr.moduloDivisor = getModuloDivisor(t);
    }
}

int HuckabyAudioProcessor::getTrackLengthSteps(int trackIndex) const noexcept
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return 16;

    const auto* p = trackParams[static_cast<size_t>(trackIndex)].length;
    if (p == nullptr)
        return 16;

    return juce::roundToInt(p->load());
}

SampleSelectionMode HuckabyAudioProcessor::getTrackMode(int trackIndex) const noexcept
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return SampleSelectionMode::Static;

    const auto* p = trackParams[static_cast<size_t>(trackIndex)].mode;
    if (p == nullptr)
        return SampleSelectionMode::Static;

    const int idx = juce::roundToInt(p->load());
    switch (idx)
    {
    case 1:
        return SampleSelectionMode::ModuloCycle;
    case 2:
        return SampleSelectionMode::Random;
    default:
        return SampleSelectionMode::Static;
    }
}

int HuckabyAudioProcessor::getStaticBankIndex(int trackIndex) const noexcept
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return 0;

    const auto* p = trackParams[static_cast<size_t>(trackIndex)].staticBank;
    return p != nullptr ? juce::roundToInt(p->load()) : 0;
}

int HuckabyAudioProcessor::getModuloDivisor(int trackIndex) const noexcept
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return 1;

    const auto* p = trackParams[static_cast<size_t>(trackIndex)].modulo;
    return p != nullptr ? juce::jmax(1, juce::roundToInt(p->load())) : 1;
}

void HuckabyAudioProcessor::processIncomingMidi(const juce::MidiBuffer& midi) noexcept
{
    for (const auto metadata : midi)
    {
        const auto m = metadata.getMessage();
        if (m.isProgramChange())
        {
            // Placeholder hook for future scene/bank automation. Values are ignored for now.
            juce::ignoreUnused(m.getProgramChangeNumber());
        }
    }
}

void HuckabyAudioProcessor::onSequencerTick() noexcept
{
    const int64_t g = globalTickIndex;

    for (int t = 0; t < kNumTracks; ++t)
        fireTrackIfTriggered(t, g);

    ++globalTickIndex;
}

void HuckabyAudioProcessor::fireTrackIfTriggered(int trackIndex, int64_t globalTick) noexcept
{
    auto& tr = tracks[static_cast<size_t>(trackIndex)];
    const int L = juce::jlimit(1, kMaxSteps, tr.activeStepLength);
    const int step = static_cast<int>(globalTick % static_cast<int64_t>(L));

    if (!tr.evaluateTriggerAtStep(step))
        return;

    const int bankIdx = tr.getActiveSampleIndex();
    if (bankIdx < 0 || bankIdx >= kMaxBankSize)
        return;

    auto sample = tr.sampleBank[static_cast<size_t>(bankIdx)].load();
    if (sample == nullptr || sample->lengthInSamples < 2)
        return;

    const float v = juce::jlimit(0.0f, 1.0f, tr.velocities[static_cast<size_t>(step)]);

    const int voiceIdx = findFreeVoiceIndex();
    voices[static_cast<size_t>(voiceIdx)].trigger(std::move(sample), v);
}

int HuckabyAudioProcessor::findFreeVoiceIndex() noexcept
{
    for (int i = 0; i < kMaxVoices; ++i)
        if (!voices[static_cast<size_t>(i)].isActive())
            return i;

    return 0;
}

void HuckabyAudioProcessor::renderVoices(juce::AudioBuffer<float>& buffer,
                                        int startSample,
                                        int numSamples) noexcept
{
    for (auto& v : voices)
        if (v.isActive())
            v.renderNextBlock(buffer, startSample, numSamples, hostSampleRate);
}

void HuckabyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    processIncomingMidi(midi);

    updateTransportAndTempo();
    reconcilePlaybackStartStopEdge();
    pullPatternBitsIntoTracks();

    buffer.clear();

    const float masterLinear = dbToLinear(outputGainDbParam != nullptr ? outputGainDbParam->load() : 0.0f);

    const int numSamples = buffer.getNumSamples();
    int processed = 0;

    if (!isEffectivelyPlaying() || samplesPerStep <= 0.0)
    {
        renderVoices(buffer, 0, numSamples);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.applyGain(ch, 0, numSamples, masterLinear);

        return;
    }

    while (processed < numSamples)
    {
        const int samplesToNext = juce::jmax(1, static_cast<int>(std::ceil(samplesPerStep - sampleCounter)));
        const int chunk = juce::jmin(numSamples - processed, samplesToNext);

        renderVoices(buffer, processed, chunk);

        sampleCounter += static_cast<double>(chunk);
        processed += chunk;

        while (sampleCounter >= samplesPerStep)
        {
            sampleCounter -= samplesPerStep;
            onSequencerTick();
        }
    }

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        buffer.applyGain(ch, 0, numSamples, masterLinear);
}

juce::AudioProcessorEditor* HuckabyAudioProcessor::createEditor()
{
    return new HuckabyAudioProcessorEditor(*this);
}

void HuckabyAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree state("HuckabyState");
    state.appendChild(apvts.copyState(), nullptr);

    juce::ValueTree patterns("Patterns");
    for (int t = 0; t < kNumTracks; ++t)
        patterns.setProperty("t" + juce::String(t),
                             static_cast<juce::int64>(patternBits[static_cast<size_t>(t)].load(std::memory_order_relaxed)),
                             nullptr);

    state.appendChild(patterns, nullptr);

    if (const auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void HuckabyAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xmlState);
        if (!state.isValid())
            return;

        if (auto params = state.getChildWithName(apvts.state.getType()); params.isValid())
            apvts.replaceState(params);

        if (auto patterns = state.getChildWithName("Patterns"); patterns.isValid())
        {
            for (int t = 0; t < kNumTracks; ++t)
            {
                const auto v = patterns.getProperty("t" + juce::String(t));
                if (!v.isVoid())
                    patternBits[static_cast<size_t>(t)].store(static_cast<uint64_t>((juce::int64) v), std::memory_order_relaxed);
            }
        }
    }
}

void HuckabyAudioProcessor::setPatternBits(int trackIndex, uint64_t bits) noexcept
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return;

    patternBits[static_cast<size_t>(trackIndex)].store(bits, std::memory_order_relaxed);
}

uint64_t HuckabyAudioProcessor::getPatternBits(int trackIndex) const noexcept
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return 0;

    return patternBits[static_cast<size_t>(trackIndex)].load(std::memory_order_relaxed);
}

void HuckabyAudioProcessor::resetTransportCounters()
{
    globalTickIndex = 0;
    sampleCounter = 0.0;
    for (auto& t : tracks)
        t.resetCounters();
}

void HuckabyAudioProcessor::requestLoadSample(int trackIndex, int slotIndex, const juce::File& file)
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return;

    if (!juce::isPositiveAndBelow(slotIndex, kMaxBankSize))
        return;

    if (!file.existsAsFile())
        return;

    loadPool.addJob([this, trackIndex, slotIndex, file]() {
        juce::AudioFormatManager fm;
        fm.registerBasicFormats();

        std::unique_ptr<juce::AudioFormatReader> reader(fm.createReaderFor(file));
        if (reader == nullptr)
            return;

        juce::AudioBuffer<float> buffer(static_cast<int>(reader->numChannels),
                                        static_cast<int>(reader->lengthInSamples));
        reader->read(&buffer, 0, buffer.getNumSamples(), 0, true, true);

        auto sample = std::make_shared<SampleData>(std::move(buffer), reader->sampleRate, file.getFileNameWithoutExtension());

        juce::MessageManager::callAsync([this, trackIndex, slotIndex, sample]() mutable {
            if (juce::MessageManager::getInstance()->hasStopMessageBeenSent())
                return;

            tracks[static_cast<size_t>(trackIndex)].sampleBank[static_cast<size_t>(slotIndex)].store(std::move(sample));
            tracks[static_cast<size_t>(trackIndex)].numSamplesLoaded = countLoadedSamples(tracks[static_cast<size_t>(trackIndex)]);
        });
    });
}

} // namespace huckaby

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new huckaby::HuckabyAudioProcessor();
}
