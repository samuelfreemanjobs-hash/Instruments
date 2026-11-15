#include "PluginProcessor.h"
#include "PluginEditor.h"

DrumDesignLabAudioProcessor::DrumDesignLabAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "DDL", createParameterLayout())
{
    engine_ = std::make_unique<disklordz::MultiGenreDrumEngine>();
}

juce::AudioProcessorValueTreeState::ParameterLayout DrumDesignLabAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::StringArray styleNames;
    for (int i = 0; i < static_cast<int>(disklordz::KitStyle::Count); ++i)
        styleNames.add(disklordz::kitStyleName(static_cast<disklordz::KitStyle>(i)));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_kit_style", 1 }, "Kit Style", styleNames, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_output_gain", 1 }, "Output",
        juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f), 0.95f));
    return { params.begin(), params.end() };
}

void DrumDesignLabAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    if (engine_ == nullptr)
        engine_ = std::make_unique<disklordz::MultiGenreDrumEngine>();
    engine_->prepare(sampleRate);
    engine_->setStyle(currentStyle_);
}

bool DrumDesignLabAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

void DrumDesignLabAudioProcessor::setKitStyle(disklordz::KitStyle style)
{
    currentStyle_ = style;
    if (engine_ != nullptr)
        engine_->setStyle(style);
    if (auto* p = apvts.getParameter("ddl_kit_style"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(static_cast<int>(style))));
}

void DrumDesignLabAudioProcessor::setCurrentProgram(int index)
{
    const int n = static_cast<int>(disklordz::KitStyle::Count);
    setKitStyle(static_cast<disklordz::KitStyle>(juce::jlimit(0, n - 1, index)));
}

const juce::String DrumDesignLabAudioProcessor::getProgramName(int index)
{
    const int n = static_cast<int>(disklordz::KitStyle::Count);
    if (index < 0 || index >= n)
        return {};
    return disklordz::kitStyleName(static_cast<disklordz::KitStyle>(index));
}

void DrumDesignLabAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    if (engine_ == nullptr)
        return;

    // Host program / APVTS style changes (message-thread params; read atomic float each block)
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_kit_style")))
    {
        const auto style = static_cast<disklordz::KitStyle>(choice->getIndex());
        if (style != currentStyle_)
        {
            currentStyle_ = style;
            engine_->setStyle(style);
        }
    }

    const float outGain = apvts.getRawParameterValue("ddl_output_gain")->load();
    const int n = buffer.getNumSamples();
    auto* L = buffer.getWritePointer(0);
    auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    auto it = midi.findNextSamplePosition(0);
    for (int i = 0; i < n; ++i)
    {
        while (it != midi.cend() && (*it).samplePosition <= i)
        {
            const auto msg = (*it).getMessage();
            if (msg.isNoteOn())
                engine_->handleMidiNote(msg.getNoteNumber(), msg.getFloatVelocity());
            ++it;
        }
        float l = 0.f, r = 0.f;
        engine_->renderTick(l, r);
        L[i] = l * outGain;
        if (R != nullptr)
            R[i] = r * outGain;
    }
}

void DrumDesignLabAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void DrumDesignLabAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* DrumDesignLabAudioProcessor::createEditor()
{
    return new DrumDesignLabAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrumDesignLabAudioProcessor();
}
