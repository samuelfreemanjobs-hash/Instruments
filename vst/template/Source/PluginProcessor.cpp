#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
constexpr const char *kParamGain = "gain";
constexpr const char *kParamWave = "wave";
} // namespace

TemplateSynthAudioProcessor::TemplateSynthAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

TemplateSynthAudioProcessor::~TemplateSynthAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
TemplateSynthAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamGain, "Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.7f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamWave, "Wave", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));
  return {params.begin(), params.end()};
}

void TemplateSynthAudioProcessor::prepareToPlay(double sampleRate, int) {
  sampleRate_ = sampleRate;
  phase_ = 0.0f;
}

void TemplateSynthAudioProcessor::releaseResources() {}

bool TemplateSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
  const auto &out = layouts.getMainOutputChannelSet();
  return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void TemplateSynthAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                               juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();

  float freqHz = 0.0f;
  for (const auto metadata : midi) {
    const auto msg = metadata.getMessage();
    if (msg.isNoteOn()) {
      freqHz = static_cast<float>(msg.getMidiNoteInHertz(msg.getNoteNumber()));
    } else if (msg.isNoteOff()) {
      freqHz = 0.0f;
    }
  }

  if (freqHz <= 0.0f) {
    return;
  }

  const float gain = apvts_.getRawParameterValue(kParamGain)->load();
  const float wave = apvts_.getRawParameterValue(kParamWave)->load();
  const float phaseInc = freqHz / static_cast<float>(sampleRate_);

  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();

  for (int sample = 0; sample < numSamples; ++sample) {
    const float sine = std::sin(phase_ * juce::MathConstants<float>::twoPi);
    const float saw = 2.0f * phase_ - 1.0f;
    const float out = gain * (sine * (1.0f - wave) + saw * wave);
    for (int ch = 0; ch < numChannels; ++ch) {
      buffer.setSample(ch, sample, out);
    }
    phase_ += phaseInc;
    if (phase_ >= 1.0f) {
      phase_ -= 1.0f;
    }
  }
}

juce::AudioProcessorEditor *TemplateSynthAudioProcessor::createEditor() {
  return new TemplateSynthAudioProcessorEditor(*this);
}

void TemplateSynthAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void TemplateSynthAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new TemplateSynthAudioProcessor();
}
