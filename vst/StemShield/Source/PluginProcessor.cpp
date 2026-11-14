#include "PluginProcessor.h"

#include "PluginEditor.h"

namespace {

float pctToNorm(float pct) { return juce::jlimit(0.0f, 1.0f, pct * 0.01f); }

} // namespace

StemShieldAudioProcessor::StemShieldAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
  applyFactoryPreset(0);
}

StemShieldAudioProcessor::~StemShieldAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout StemShieldAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamCeiling, "Ceiling", juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f),
      StemShieldDsp::kDefaultCeilingDbTp, juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " dBTP"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamSubBalance, "Sub Balance", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 65.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 0) + " %"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamSubCrossover, "Sub Crossover", juce::NormalisableRange<float>(40.0f, 160.0f, 0.1f), 80.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " Hz"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamRelease, "Release", juce::NormalisableRange<float>(30.0f, 400.0f, 1.0f), 120.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 0) + " ms"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamLookahead, "Lookahead", juce::NormalisableRange<float>(0.5f, 8.0f, 0.1f), 2.5f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " ms"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamGain, "Gain", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " dB"; }));
  return {params.begin(), params.end()};
}

void StemShieldAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  syncDspFromApvts();
  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
  spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
  dsp_.prepare(spec);
  setLatencySamples(dsp_.getLatencySamples());
}

void StemShieldAudioProcessor::releaseResources() {
  dsp_.reset();
}

bool StemShieldAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet()) {
    return false;
  }
  const auto& set = layouts.getMainOutputChannelSet();
  return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

void StemShieldAudioProcessor::syncDspFromApvts() {
  dsp_.setCeilingDbTp(apvts_.getRawParameterValue(kParamCeiling)->load());
  dsp_.setSubBalance(pctToNorm(apvts_.getRawParameterValue(kParamSubBalance)->load()));
  dsp_.setSubCrossoverHz(apvts_.getRawParameterValue(kParamSubCrossover)->load());
  dsp_.setReleaseMs(apvts_.getRawParameterValue(kParamRelease)->load());
  dsp_.setLookaheadMs(apvts_.getRawParameterValue(kParamLookahead)->load());
  dsp_.setInputGainDb(apvts_.getRawParameterValue(kParamGain)->load());
}

void StemShieldAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();

  for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch) {
    buffer.clear(ch, 0, buffer.getNumSamples());
  }

  syncDspFromApvts();
  dsp_.process(buffer);

  const int newLatency = dsp_.getLatencySamples();
  if (newLatency != getLatencySamples()) {
    setLatencySamples(newLatency);
  }
}

void StemShieldAudioProcessor::applyFactoryPreset(int index) {
  index = juce::jlimit(0, kStemShieldFactoryPresetCount - 1, index);
  currentProgram_ = index;
  const auto& p = kStemShieldFactoryPresets[static_cast<size_t>(index)];

  const auto set = [&](const char* id, float value) {
    if (auto* param = apvts_.getParameter(id)) {
      param->setValueNotifyingHost(param->convertTo0to1(value));
    }
  };

  set(kParamCeiling, p.ceilingDbTp);
  set(kParamSubBalance, p.subBalance * 100.0f);
  set(kParamSubCrossover, p.crossoverHz);
  set(kParamRelease, p.releaseMs);
  set(kParamLookahead, p.lookaheadMs);
  set(kParamGain, p.inputGainDb);
}

void StemShieldAudioProcessor::setCurrentProgram(int index) {
  applyFactoryPreset(index);
}

const juce::String StemShieldAudioProcessor::getProgramName(int index) {
  if (index < 0 || index >= kStemShieldFactoryPresetCount) {
    return {};
  }
  return kStemShieldFactoryPresets[static_cast<size_t>(index)].name;
}

juce::AudioProcessorEditor* StemShieldAudioProcessor::createEditor() {
  return new StemShieldAudioProcessorEditor(*this);
}

void StemShieldAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts_.copyState();
  state.setProperty("currentProgram", currentProgram_, nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void StemShieldAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml == nullptr || !xml->hasTagName(apvts_.state.getType())) {
    return;
  }
  auto vt = juce::ValueTree::fromXml(*xml);
  apvts_.replaceState(vt);
  currentProgram_ = vt.getProperty("currentProgram", 0);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new StemShieldAudioProcessor();
}
