#include "SynthProcessor.h"
#include "SynthEditor.h"

namespace
{
constexpr int kNumVoices = 8;
}

{{PLUGIN_CAMEL}}AudioProcessor::SynthVoice::SynthVoice({{PLUGIN_CAMEL}}AudioProcessor& ownerProcessor)
    : owner(ownerProcessor)
{
}

bool {{PLUGIN_CAMEL}}AudioProcessor::SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void {{PLUGIN_CAMEL}}AudioProcessor::SynthVoice::startNote(int midiNoteNumber, float velocity,
                                                       juce::SynthesiserSound*, int)
{
    const auto frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentAngle = 0.0;
    angleDelta = juce::MathConstants<double>::twoPi * frequency / getSampleRate();
    currentLevel = velocity * 0.2f;

    adsr.setSampleRate(getSampleRate());
    adsr.setParameters(owner.getEnvelopeParameters());
    adsr.noteOn();
}

void {{PLUGIN_CAMEL}}AudioProcessor::SynthVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
        adsr.noteOff();
    else
    {
        adsr.reset();
        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void {{PLUGIN_CAMEL}}AudioProcessor::SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                                             int startSample,
                                                             int numSamples)
{
    if (angleDelta == 0.0)
        return;

    while (--numSamples >= 0)
    {
        const auto envelope = adsr.getNextSample();
        const auto sample = static_cast<float>(std::sin(currentAngle) * currentLevel * envelope);

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample(ch, startSample, sample);

        currentAngle += angleDelta;
        ++startSample;

        if (!adsr.isActive())
        {
            clearCurrentNote();
            angleDelta = 0.0;
            break;
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout {{PLUGIN_CAMEL}}AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(SynthParamIDs::outputGainDb), 1 },
        "Output",
        juce::NormalisableRange<float> { -24.0f, 12.0f, 0.01f },
        -3.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(SynthParamIDs::attack), 1 },
        "Attack",
        juce::NormalisableRange<float> { 0.001f, 2.0f, 0.001f, 0.4f },
        0.01f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(SynthParamIDs::decay), 1 },
        "Decay",
        juce::NormalisableRange<float> { 0.001f, 2.0f, 0.001f, 0.4f },
        0.2f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(SynthParamIDs::sustain), 1 },
        "Sustain",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f },
        0.8f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { std::string(SynthParamIDs::release), 1 },
        "Release",
        juce::NormalisableRange<float> { 0.001f, 5.0f, 0.001f, 0.4f },
        0.4f));

    return layout;
}

{{PLUGIN_CAMEL}}AudioProcessor::{{PLUGIN_CAMEL}}AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
       apvts(*this, nullptr, "{{PLUGIN_CAMEL}}State", createParameterLayout())
{
    for (int i = 0; i < kNumVoices; ++i)
        synthesiser.addVoice(new SynthVoice(*this));

    synthesiser.addSound(new SynthSound());

    outputGainDbParam = apvts.getRawParameterValue(std::string(SynthParamIDs::outputGainDb));
    attackParam = apvts.getRawParameterValue(std::string(SynthParamIDs::attack));
    decayParam = apvts.getRawParameterValue(std::string(SynthParamIDs::decay));
    sustainParam = apvts.getRawParameterValue(std::string(SynthParamIDs::sustain));
    releaseParam = apvts.getRawParameterValue(std::string(SynthParamIDs::release));
}

{{PLUGIN_CAMEL}}AudioProcessor::~{{PLUGIN_CAMEL}}AudioProcessor() = default;

void {{PLUGIN_CAMEL}}AudioProcessor::refreshEnvelopeParameters() noexcept
{
    envelopeParams.attack = attackParam->load();
    envelopeParams.decay = decayParam->load();
    envelopeParams.sustain = sustainParam->load();
    envelopeParams.release = releaseParam->load();
}

const juce::String {{PLUGIN_CAMEL}}AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool {{PLUGIN_CAMEL}}AudioProcessor::acceptsMidi() const
{
    return true;
}

bool {{PLUGIN_CAMEL}}AudioProcessor::producesMidi() const
{
    return false;
}

bool {{PLUGIN_CAMEL}}AudioProcessor::isMidiEffect() const
{
    return false;
}

double {{PLUGIN_CAMEL}}AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int {{PLUGIN_CAMEL}}AudioProcessor::getNumPrograms()
{
    return 1;
}

int {{PLUGIN_CAMEL}}AudioProcessor::getCurrentProgram()
{
    return 0;
}

void {{PLUGIN_CAMEL}}AudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String {{PLUGIN_CAMEL}}AudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void {{PLUGIN_CAMEL}}AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void {{PLUGIN_CAMEL}}AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    refreshEnvelopeParameters();
}

void {{PLUGIN_CAMEL}}AudioProcessor::releaseResources() {}

bool {{PLUGIN_CAMEL}}AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void {{PLUGIN_CAMEL}}AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    refreshEnvelopeParameters();

    buffer.clear();
    synthesiser.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    const float gain = juce::Decibels::decibelsToGain(outputGainDbParam->load(), -100.0f);
    buffer.applyGain(gain);
}

bool {{PLUGIN_CAMEL}}AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* {{PLUGIN_CAMEL}}AudioProcessor::createEditor()
{
    return new {{PLUGIN_CAMEL}}AudioProcessorEditor(*this);
}

void {{PLUGIN_CAMEL}}AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void {{PLUGIN_CAMEL}}AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new {{PLUGIN_CAMEL}}AudioProcessor();
}
