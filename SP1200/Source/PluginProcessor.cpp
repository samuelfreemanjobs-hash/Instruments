#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets/FactoryProgramBank.h"
#include "Project/ProjectFile.h"

SP1200AudioProcessor::SP1200AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
#endif
{
    sp1200::presets::applyFactoryProgram (engine_, 0);
}

SP1200AudioProcessor::~SP1200AudioProcessor() = default;

const juce::String SP1200AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SP1200AudioProcessor::acceptsMidi() const { return true; }
bool SP1200AudioProcessor::producesMidi() const { return false; }
bool SP1200AudioProcessor::isMidiEffect() const { return false; }
double SP1200AudioProcessor::getTailLengthSeconds() const { return 0.5; }
bool SP1200AudioProcessor::hasEditor() const { return true; }

int SP1200AudioProcessor::getNumPrograms()
{
    return static_cast<int> (sp1200::presets::kFactoryProgramCount);
}

int SP1200AudioProcessor::getCurrentProgram()
{
    return currentProgram_;
}

void SP1200AudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= getNumPrograms())
        return;
    currentProgram_ = index;
    sp1200::presets::applyFactoryProgram (engine_, index);
}

const juce::String SP1200AudioProcessor::getProgramName (int index)
{
    return sp1200::presets::programNameAt (index).c_str();
}

void SP1200AudioProcessor::changeProgramName (int, const juce::String&) {}

void SP1200AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    engine_.prepare (sampleRate, samplesPerBlock);
}

void SP1200AudioProcessor::releaseResources() {}

bool SP1200AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled())
        return true;

    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
           || layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo();
}

void SP1200AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    if (engine_.isRecording())
    {
        juce::AudioBuffer<float> input (getTotalNumInputChannels(), buffer.getNumSamples());
        for (int ch = 0; ch < input.getNumChannels(); ++ch)
            input.copyFrom (ch, 0, buffer, ch, 0, buffer.getNumSamples());
        engine_.appendRecording (input);
    }

    engine_.process (buffer, midi);
}

juce::AudioProcessorEditor* SP1200AudioProcessor::createEditor()
{
    return new SP1200AudioProcessorEditor (*this);
}

void SP1200AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    sp1200::ProjectFile::saveToMemoryBlock (engine_, destData);
}

void SP1200AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    sp1200::ProjectFile::loadFromMemoryBlock (engine_, data, static_cast<std::size_t> (sizeInBytes));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SP1200AudioProcessor();
}
