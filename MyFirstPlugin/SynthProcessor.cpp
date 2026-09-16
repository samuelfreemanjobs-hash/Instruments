#include "SynthProcessor.h"
#include "SynthEditor.h"

namespace
{
constexpr int kNumVoices = 8;
}

MyFirstSynthAudioProcessor::SynthVoice::SynthVoice(MyFirstSynthAudioProcessor& ownerProcessor)
    : owner(ownerProcessor)
{
}

bool MyFirstSynthAudioProcessor::SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void MyFirstSynthAudioProcessor::SynthVoice::startNote(int midiNoteNumber, float velocity,
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

void MyFirstSynthAudioProcessor::SynthVoice::stopNote(float, bool allowTailOff)
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

void MyFirstSynthAudioProcessor::SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
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

juce::AudioProcessorValueTreeState::ParameterLayout MyFirstSynthAudioProcessor::createParameterLayout()
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

MyFirstSynthAudioProcessor::MyFirstSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
       apvts(*this, nullptr, "MyFirstSynthState", createParameterLayout())
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

MyFirstSynthAudioProcessor::~MyFirstSynthAudioProcessor() = default;

void MyFirstSynthAudioProcessor::refreshEnvelopeParameters() noexcept
{
    envelopeParams.attack = attackParam->load();
    envelopeParams.decay = decayParam->load();
    envelopeParams.sustain = sustainParam->load();
    envelopeParams.release = releaseParam->load();
}

const juce::String MyFirstSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyFirstSynthAudioProcessor::acceptsMidi() const
{
    return true;
}

bool MyFirstSynthAudioProcessor::producesMidi() const
{
    return false;
}

bool MyFirstSynthAudioProcessor::isMidiEffect() const
{
    return false;
}

double MyFirstSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyFirstSynthAudioProcessor::getNumPrograms()
{
    return 1;
}

int MyFirstSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyFirstSynthAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MyFirstSynthAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MyFirstSynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MyFirstSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    refreshEnvelopeParameters();
}

void MyFirstSynthAudioProcessor::releaseResources() {}

bool MyFirstSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void MyFirstSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    refreshEnvelopeParameters();

    buffer.clear();
    synthesiser.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    const float gain = juce::Decibels::decibelsToGain(outputGainDbParam->load(), -100.0f);
    buffer.applyGain(gain);
}

bool MyFirstSynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MyFirstSynthAudioProcessor::createEditor()
{
    return new MyFirstSynthAudioProcessorEditor(*this);
}

void MyFirstSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MyFirstSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyFirstSynthAudioProcessor();
}
