#include "PluginProcessor.h"
#include "PluginEditor.h"

RenaissanceHostAudioProcessor::RenaissanceHostAudioProcessor()
    : juce::AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

RenaissanceHostAudioProcessor::~RenaissanceHostAudioProcessor() = default;

void RenaissanceHostAudioProcessor::prepareToPlay (double sampleRate, int)
{
    engine_.prepare (sampleRate);
    const auto outs = juce::MidiOutput::getAvailableDevices();
    for (const auto& d : outs)
    {
        if (d.name.containsIgnoreCase ("Renaissance"))
        {
            midiOut_ = juce::MidiOutput::openDevice (d.identifier);
            lcdBridge_.setMidiOutput (midiOut_.get());
            lcdBridge_.sendStatusLine ("Renaissance Host ready");
            break;
        }
    }
}

void RenaissanceHostAudioProcessor::releaseResources()
{
    midiOut_.reset();
    lcdBridge_.setMidiOutput (nullptr);
}

bool RenaissanceHostAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
           || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void RenaissanceHostAudioProcessor::handleMidiMessage (const juce::MidiMessage& msg)
{
    if (! surfaceMap_.acceptsChannel (msg.getChannel()))
        return;

    if (msg.isNoteOn())
    {
        const int pad = surfaceMap_.padIndexForNote (msg.getNoteNumber());
        if (pad >= 0)
        {
            const float vel = msg.getFloatVelocity();
            engine_.triggerPad (pad, vel);
            const juce::ScopedLock lock (activityLock_);
            lastPadIndex_ = pad;
            lastVelocity_ = vel;
        }
    }
    else if (msg.isController())
    {
        const int cc = msg.getControllerNumber();
        for (int p = 0; p < renaissance::kNumPads; ++p)
        {
            if (surfaceMap_.qlinkCc (p) != cc)
                continue;
            const float norm = static_cast<float> (msg.getControllerValue()) / 127.0f;
            engine_.setPadLevel (p, norm);
            const juce::ScopedLock lock (activityLock_);
            lastQlinkPad_ = p;
            lastQlinkValue_ = norm;
            break;
        }
    }
}

void RenaissanceHostAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    for (const auto metadata : midi)
        handleMidiMessage (metadata.getMessage());

    engine_.render (buffer, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* RenaissanceHostAudioProcessor::createEditor()
{
    return new RenaissanceHostAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RenaissanceHostAudioProcessor();
}
