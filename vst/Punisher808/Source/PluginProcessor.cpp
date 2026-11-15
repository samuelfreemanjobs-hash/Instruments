#include "PluginProcessor.h"

#include "PluginEditor.h"

namespace {

float pctToNorm(float pct) { return juce::jlimit(0.0f, 1.0f, pct * 0.01f); }

} // namespace

Punisher808AudioProcessor::Punisher808AudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
  applyFactoryPreset(0);
}

Punisher808AudioProcessor::~Punisher808AudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout Punisher808AudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamPunish, "Punish", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 55.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " %"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamHarmonicShape, "Harmonic Shape", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 70.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 0); }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamSubCrossover, "Sub Crossover", juce::NormalisableRange<float>(40.0f, 120.0f, 0.1f), 60.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " Hz"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamMix, "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 0) + " %"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamOutputGain, "Output", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " dB"; }));
  return {params.begin(), params.end()};
}

void Punisher808AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  dsp_.setPunish(pctToNorm(apvts_.getRawParameterValue(kParamPunish)->load()));
  dsp_.setHarmonicShape(pctToNorm(apvts_.getRawParameterValue(kParamHarmonicShape)->load()));
  dsp_.setCrossoverHz(apvts_.getRawParameterValue(kParamSubCrossover)->load());
  dsp_.setMix(pctToNorm(apvts_.getRawParameterValue(kParamMix)->load()));
  dsp_.setOutputGainDb(apvts_.getRawParameterValue(kParamOutputGain)->load());

  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
  spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
  dsp_.prepare(spec);
  setLatencySamples(dsp_.getLatencySamples());
}

void Punisher808AudioProcessor::releaseResources() {
  dsp_.reset();
}

bool Punisher808AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet()) {
    return false;
  }
  const auto& set = layouts.getMainOutputChannelSet();
  return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

void Punisher808AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();

  for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch) {
    buffer.clear(ch, 0, buffer.getNumSamples());
  }

  dsp_.setPunish(pctToNorm(apvts_.getRawParameterValue(kParamPunish)->load()));
  dsp_.setHarmonicShape(pctToNorm(apvts_.getRawParameterValue(kParamHarmonicShape)->load()));
  dsp_.setCrossoverHz(apvts_.getRawParameterValue(kParamSubCrossover)->load());
  dsp_.setMix(pctToNorm(apvts_.getRawParameterValue(kParamMix)->load()));
  dsp_.setOutputGainDb(apvts_.getRawParameterValue(kParamOutputGain)->load());

  dsp_.process(buffer);
}

void Punisher808AudioProcessor::applyFactoryPreset(int index) {
  index = juce::jlimit(0, kFactoryPresetCount - 1, index);
  currentProgram_ = index;
  const auto& p = kFactoryPresets[static_cast<size_t>(index)];
  if (auto* param = apvts_.getParameter(kParamPunish)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.punish * 100.0f));
  }
  if (auto* param = apvts_.getParameter(kParamHarmonicShape)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.shape * 100.0f));
  }
  if (auto* param = apvts_.getParameter(kParamSubCrossover)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.crossoverHz));
  }
  if (auto* param = apvts_.getParameter(kParamMix)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.mix * 100.0f));
  }
  if (auto* param = apvts_.getParameter(kParamOutputGain)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.outputDb));
  }
}

void Punisher808AudioProcessor::setCurrentProgram(int index) {
  applyFactoryPreset(index);
}

const juce::String Punisher808AudioProcessor::getProgramName(int index) {
  if (index < 0 || index >= kFactoryPresetCount) {
    return {};
  }
  return kFactoryPresets[static_cast<size_t>(index)].name;
}

juce::AudioProcessorEditor* Punisher808AudioProcessor::createEditor() {
  return new Punisher808AudioProcessorEditor(*this);
}

void Punisher808AudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts_.copyState();
  state.setProperty("currentProgram", currentProgram_, nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void Punisher808AudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml == nullptr || !xml->hasTagName(apvts_.state.getType())) {
    return;
  }
  auto vt = juce::ValueTree::fromXml(*xml);
  apvts_.replaceState(vt);
  currentProgram_ = vt.getProperty("currentProgram", 0);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new Punisher808AudioProcessor();
}
