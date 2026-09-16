#include "PluginProcessor.h"

namespace
{
constexpr float kMinCutoffHz = 20.0f;
constexpr float kMaxCutoffHz = 20000.0f;
constexpr float kSmoothingTimeSeconds = 0.02f;
constexpr float kTwoPi = juce::MathConstants<float>::twoPi;

float dbToLinear(float decibels) noexcept
{
    return juce::Decibels::decibelsToGain(decibels, -100.0f);
}
} // namespace

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout MyFirstPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(ParamIDs::cutoffHz), 1 },
        "Cutoff",
        juce::NormalisableRange<float> { kMinCutoffHz, kMaxCutoffHz, 0.01f, 0.25f },
        1200.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(ParamIDs::resonance), 1 },
        "Resonance",
        juce::NormalisableRange<float> { 0.1f, 10.0f, 0.001f, 0.5f },
        0.707f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(ParamIDs::drive), 1 },
        "Drive",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        0.35f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(ParamIDs::lfoRateHz), 1 },
        "LFO Rate",
        juce::NormalisableRange<float> { 0.05f, 20.0f, 0.001f, 0.35f },
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(ParamIDs::lfoDepth), 1 },
        "LFO Depth",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(ParamIDs::mix), 1 },
        "Mix",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(ParamIDs::outputGainDb), 1 },
        "Output",
        juce::NormalisableRange<float> { -24.0f, 12.0f, 0.01f },
        0.0f));

    return layout;
}

MyFirstPluginAudioProcessor::MyFirstPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                          .withInput("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
#endif
                          ),
#endif
       apvts(*this, nullptr, "MyFirstPluginState", createParameterLayout())
{
    cutoffHzParam = apvts.getRawParameterValue(std::string(ParamIDs::cutoffHz));
    resonanceParam = apvts.getRawParameterValue(std::string(ParamIDs::resonance));
    driveParam = apvts.getRawParameterValue(std::string(ParamIDs::drive));
    lfoRateHzParam = apvts.getRawParameterValue(std::string(ParamIDs::lfoRateHz));
    lfoDepthParam = apvts.getRawParameterValue(std::string(ParamIDs::lfoDepth));
    mixParam = apvts.getRawParameterValue(std::string(ParamIDs::mix));
    outputGainDbParam = apvts.getRawParameterValue(std::string(ParamIDs::outputGainDb));
}

MyFirstPluginAudioProcessor::~MyFirstPluginAudioProcessor() = default;

const juce::String MyFirstPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyFirstPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MyFirstPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MyFirstPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MyFirstPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyFirstPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int MyFirstPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyFirstPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MyFirstPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MyFirstPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MyFirstPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    processSpec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    for (auto& channel : channelDSP)
    {
        channel.svf.reset();
        channel.svf.prepare(processSpec);
        channel.svf.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        channel.lfoPhaseRadians = 0.0f;
    }

    const auto smoothSamples = static_cast<int>(sampleRate * kSmoothingTimeSeconds);

    cutoffSmoothed.reset(sampleRate, kSmoothingTimeSeconds);
    resonanceSmoothed.reset(sampleRate, kSmoothingTimeSeconds);
    driveSmoothed.reset(sampleRate, kSmoothingTimeSeconds);
    lfoRateSmoothed.reset(sampleRate, kSmoothingTimeSeconds);
    lfoDepthSmoothed.reset(sampleRate, kSmoothingTimeSeconds);
    mixSmoothed.reset(sampleRate, kSmoothingTimeSeconds);
    outputGainLinearSmoothed.reset(sampleRate, kSmoothingTimeSeconds);

    cutoffSmoothed.setCurrentAndTargetValue(cutoffHzParam->load());
    resonanceSmoothed.setCurrentAndTargetValue(resonanceParam->load());
    driveSmoothed.setCurrentAndTargetValue(driveParam->load());
    lfoRateSmoothed.setCurrentAndTargetValue(lfoRateHzParam->load());
    lfoDepthSmoothed.setCurrentAndTargetValue(lfoDepthParam->load());
    mixSmoothed.setCurrentAndTargetValue(mixParam->load());
    outputGainLinearSmoothed.setCurrentAndTargetValue(dbToLinear(outputGainDbParam->load()));

    juce::ignoreUnused(smoothSamples);
}

void MyFirstPluginAudioProcessor::releaseResources()
{
    for (auto& channel : channelDSP)
        channel.svf.reset();
}

bool MyFirstPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void MyFirstPluginAudioProcessor::updateSmootherTargets() noexcept
{
    cutoffSmoothed.setTargetValue(cutoffHzParam->load());
    resonanceSmoothed.setTargetValue(resonanceParam->load());
    driveSmoothed.setTargetValue(driveParam->load());
    lfoRateSmoothed.setTargetValue(lfoRateHzParam->load());
    lfoDepthSmoothed.setTargetValue(lfoDepthParam->load());
    mixSmoothed.setTargetValue(mixParam->load());
    outputGainLinearSmoothed.setTargetValue(dbToLinear(outputGainDbParam->load()));
}

float MyFirstPluginAudioProcessor::tubeSaturation(float sample, float driveAmount) noexcept
{
    const float gain = 1.0f + driveAmount * 24.0f;
    return std::tanh(sample * gain);
}

void MyFirstPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = juce::jmin(totalNumOutputChannels, maxChannels);

    for (auto ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear(ch, 0, numSamples);

    updateSmootherTargets();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float baseCutoff = cutoffSmoothed.getNextValue();
        const float resonanceValue = resonanceSmoothed.getNextValue();
        const float driveValue = driveSmoothed.getNextValue();
        const float lfoRate = lfoRateSmoothed.getNextValue();
        const float lfoDepthValue = lfoDepthSmoothed.getNextValue();
        const float mixValue = mixSmoothed.getNextValue();
        const float outputGain = outputGainLinearSmoothed.getNextValue();

        const float lfoRadiansPerSample = kTwoPi * lfoRate / static_cast<float>(currentSampleRate);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto& dsp = channelDSP[static_cast<size_t>(channel)];

            const float lfo = std::sin(dsp.lfoPhaseRadians);
            dsp.lfoPhaseRadians += lfoRadiansPerSample;
            if (dsp.lfoPhaseRadians >= kTwoPi)
                dsp.lfoPhaseRadians -= kTwoPi;

            const float octaveSpread = lfoDepthValue * 2.0f;
            const float modFactor = std::pow(2.0f, lfo * octaveSpread);
            const float modulatedCutoff = juce::jlimit(kMinCutoffHz, kMaxCutoffHz, baseCutoff * modFactor);

            dsp.svf.setCutoffFrequency(modulatedCutoff);
            dsp.svf.setResonance(resonanceValue);

            const float dry = buffer.getSample(channel, sample);
            float wet = dsp.svf.processSample(0, dry);
            wet = tubeSaturation(wet, driveValue);

            const float out = (dry * (1.0f - mixValue)) + (wet * mixValue);
            buffer.setSample(channel, sample, out * outputGain);
        }
    }
}

bool MyFirstPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MyFirstPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void MyFirstPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MyFirstPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyFirstPluginAudioProcessor();
}
