#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
const char* kPadFiles[OvernightDrumAudioProcessor::kPads] = {
    "kick.wav", "snare.wav", "clap.wav", "closed_hat.wav",
    "open_hat.wav", "tom_low.wav", "tom_high.wav", "perc.wav"
};
}

OvernightDrumAudioProcessor::OvernightDrumAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMS", createParameterLayout())
{
    const auto defaultKit = juce::File(__FILE__)
                                .getParentDirectory()
                                .getParentDirectory()
                                .getParentDirectory()
                                .getChildFile("output")
                                .getChildFile("best_kit");
    if (defaultKit.isDirectory())
        loadKitFromDirectory(defaultKit);
}

juce::AudioProcessorValueTreeState::ParameterLayout OvernightDrumAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "tune", 1 }, "Tune",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "decay", 1 }, "Decay",
        juce::NormalisableRange<float>(0.2f, 1.5f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "gain", 1 }, "Gain",
        juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f), 0.9f));
    return { params.begin(), params.end() };
}

bool OvernightDrumAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void OvernightDrumAudioProcessor::prepareToPlay(double sampleRate, int)
{
    sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    for (auto& v : voices_)
        v.active = false;
}

void OvernightDrumAudioProcessor::loadKitFromDirectory(const juce::File& dir)
{
    kitDir_ = dir;
    juce::WavAudioFormat fmt;
    for (int i = 0; i < kPads; ++i)
    {
        pads_[static_cast<size_t>(i)].setSize(0, 0);
        const auto f = dir.getChildFile(kPadFiles[i]);
        if (!f.existsAsFile())
            continue;
        std::unique_ptr<juce::AudioFormatReader> reader(fmt.createReaderFor(f.createInputStream(), true));
        if (reader == nullptr)
            continue;
        juce::AudioBuffer<float> buf(static_cast<int>(reader->numChannels),
                                     static_cast<int>(reader->lengthInSamples));
        reader->read(&buf, 0, buf.getNumSamples(), 0, true, true);
        pads_[static_cast<size_t>(i)] = std::move(buf);
    }
}

int OvernightDrumAudioProcessor::midiToPad(int note) const noexcept
{
    switch (note)
    {
    case 36: return 0;
    case 38:
    case 40: return 1;
    case 39: return 2;
    case 42:
    case 44: return 3;
    case 46: return 4;
    case 41: return 5;
    case 45: return 6;
    case 47: return 7;
    default: return -1;
    }
}

void OvernightDrumAudioProcessor::noteOn(int midiNote, float velocity)
{
    const int pad = midiToPad(midiNote);
    if (pad < 0 || pads_[static_cast<size_t>(pad)].getNumSamples() <= 1)
        return;

    // hat choke
    if (pad == 3 || pad == 4)
    {
        for (auto& v : voices_)
            if (v.active && (v.pad == 3 || v.pad == 4))
                v.active = false;
    }

    Voice* slot = &voices_[0];
    for (auto& v : voices_)
    {
        if (!v.active)
        {
            slot = &v;
            break;
        }
    }
    const float tuneSemis = apvts_.getRawParameterValue("tune")->load();
    slot->active = true;
    slot->pad = pad;
    slot->pos = 0.0;
    slot->gain = juce::jlimit(0.0f, 1.0f, velocity) * apvts_.getRawParameterValue("gain")->load();
    slot->ratio = std::pow(2.0, static_cast<double>(tuneSemis) / 12.0);
}

void OvernightDrumAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    keyboardState_.processNextMidiBuffer(midi, 0, buffer.getNumSamples(), true);

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
    }

    const float decayScale = apvts_.getRawParameterValue("decay")->load();
    const int n = buffer.getNumSamples();
    const int chans = buffer.getNumChannels();

    for (auto& v : voices_)
    {
        if (!v.active)
            continue;
        auto& pad = pads_[static_cast<size_t>(v.pad)];
        const int total = pad.getNumSamples();
        const int srcCh = pad.getNumChannels();
        for (int i = 0; i < n; ++i)
        {
            const int i0 = static_cast<int>(v.pos);
            if (i0 >= total - 1)
            {
                v.active = false;
                break;
            }
            const float frac = static_cast<float>(v.pos - static_cast<double>(i0));
            const int i1 = i0 + 1;
            for (int c = 0; c < chans; ++c)
            {
                const int sc = juce::jmin(c, srcCh - 1);
                const float* s = pad.getReadPointer(sc);
                const float sample = s[i0] + frac * (s[i1] - s[i0]);
                buffer.addSample(c, i, sample * v.gain);
            }
            v.pos += v.ratio / static_cast<double>(decayScale);
        }
    }
}

void OvernightDrumAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts_.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void OvernightDrumAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts_.state.getType()))
            apvts_.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* OvernightDrumAudioProcessor::createEditor()
{
    return new OvernightDrumAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OvernightDrumAudioProcessor();
}
