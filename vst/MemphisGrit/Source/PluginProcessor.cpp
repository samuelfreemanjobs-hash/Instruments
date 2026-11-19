#include "PluginProcessor.h"
#include "PluginEditor.h"

MemphisGritAudioProcessor::MemphisGritAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

MemphisGritAudioProcessor::~MemphisGritAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
MemphisGritAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamDrive, "Drive", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.42f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamLofi, "Lo-Fi", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.48f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamMid, "Mid Grit", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.55f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamMix, "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamOutputDb, "Output",
      juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamBypass, "Bypass", false));
  return {params.begin(), params.end()};
}

void MemphisGritAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  dsp_.prepare(sampleRate, samplesPerBlock,
               juce::jmax(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
  syncDspFromParameters();
}

void MemphisGritAudioProcessor::releaseResources() { dsp_.reset(); }

bool MemphisGritAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet()) {
    return false;
  }
  const auto& set = layouts.getMainOutputChannelSet();
  return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

void MemphisGritAudioProcessor::syncDspFromParameters() {
  dsp_.setDrive(apvts_.getRawParameterValue(kParamDrive)->load());
  dsp_.setLofi(apvts_.getRawParameterValue(kParamLofi)->load());
  dsp_.setMid(apvts_.getRawParameterValue(kParamMid)->load());
  dsp_.setMix(apvts_.getRawParameterValue(kParamMix)->load());
  dsp_.setOutputGainDb(apvts_.getRawParameterValue(kParamOutputDb)->load());
}

void MemphisGritAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();

  const bool bypass = apvts_.getRawParameterValue(kParamBypass)->load() >= 0.5f;
  if (bypass) {
    const float gain = juce::Decibels::decibelsToGain(
        apvts_.getRawParameterValue(kParamOutputDb)->load());
    buffer.applyGain(gain);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
      auto* data = buffer.getWritePointer(ch);
      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        data[i] = juce::jlimit(-MemphisGritDsp::kCeilingLinear, MemphisGritDsp::kCeilingLinear,
                               data[i]);
      }
    }
    return;
  }

  syncDspFromParameters();
  dsp_.process(buffer);
}

juce::AudioProcessorEditor* MemphisGritAudioProcessor::createEditor() {
  return new MemphisGritAudioProcessorEditor(*this);
}

void MemphisGritAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void MemphisGritAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    syncDspFromParameters();
  }
}

juce::AudioProcessorParameter* MemphisGritAudioProcessor::getBypassParameter() const {
  return apvts_.getParameter(kParamBypass);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new MemphisGritAudioProcessor();
}
