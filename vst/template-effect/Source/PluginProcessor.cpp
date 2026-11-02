#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
constexpr const char *kParamDrive = "drive";
constexpr const char *kParamTone = "tone";
constexpr const char *kParamMix = "mix";
} // namespace

TemplateEffectAudioProcessor::TemplateEffectAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

TemplateEffectAudioProcessor::~TemplateEffectAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
TemplateEffectAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamDrive, "Drive", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.2f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamTone, "Tone", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.6f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamMix, "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
  return {params.begin(), params.end()};
}

void TemplateEffectAudioProcessor::prepareToPlay(double sampleRate, int) {
  sampleRate_ = sampleRate;
  toneL_.reset();
  toneR_.reset();
  driveSmooth_.setTargetCoeff(15.0f, static_cast<float>(sampleRate));
  toneSmooth_.setTargetCoeff(20.0f, static_cast<float>(sampleRate));
  mixSmooth_.setTargetCoeff(10.0f, static_cast<float>(sampleRate));
  driveSmooth_.reset(apvts_.getRawParameterValue(kParamDrive)->load());
  toneSmooth_.reset(apvts_.getRawParameterValue(kParamTone)->load());
  mixSmooth_.reset(apvts_.getRawParameterValue(kParamMix)->load());
}

void TemplateEffectAudioProcessor::releaseResources() {}

bool TemplateEffectAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
  if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet()) {
    return false;
  }
  const auto &set = layouts.getMainOutputChannelSet();
  return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

float TemplateEffectAudioProcessor::normToCutoff(float norm, float sampleRate) {
  const float fMin = 80.0f;
  const float fMax = 0.45f * sampleRate;
  const float t = norm * norm;
  return fMin * std::pow(fMax / fMin, t);
}

float TemplateEffectAudioProcessor::softClip(float x, float drive) {
  const float g = 1.0f + drive * 8.0f;
  return std::tanh(x * g) / std::tanh(g);
}

void TemplateEffectAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();

  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  if (numChannels < 1) {
    return;
  }

  float *left = buffer.getWritePointer(0);
  float *right = numChannels > 1 ? buffer.getWritePointer(1) : left;

  for (int i = 0; i < numSamples; ++i) {
    const float drive = driveSmooth_.process(apvts_.getRawParameterValue(kParamDrive)->load());
    const float tone = toneSmooth_.process(apvts_.getRawParameterValue(kParamTone)->load());
    const float mix = mixSmooth_.process(apvts_.getRawParameterValue(kParamMix)->load());

    if ((i & 15) == 0) {
      const float cutoff = normToCutoff(tone, static_cast<float>(sampleRate_));
      toneL_.setLowpass(static_cast<float>(sampleRate_), cutoff, 0.707f);
      toneR_.setLowpass(static_cast<float>(sampleRate_), cutoff, 0.707f);
    }

    const float dryL = left[i];
    const float dryR = right[i];
    float wetL = softClip(dryL, drive);
    float wetR = softClip(dryR, drive);
    wetL = toneL_.process(wetL);
    wetR = toneR_.process(wetR);

    left[i] = dryL * (1.0f - mix) + wetL * mix;
    right[i] = dryR * (1.0f - mix) + wetR * mix;
  }
}

juce::AudioProcessorEditor *TemplateEffectAudioProcessor::createEditor() {
  return new TemplateEffectAudioProcessorEditor(*this);
}

void TemplateEffectAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void TemplateEffectAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new TemplateEffectAudioProcessor();
}
