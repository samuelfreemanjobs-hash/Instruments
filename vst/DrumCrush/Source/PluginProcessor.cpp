#include "PluginProcessor.h"

#include "PluginEditor.h"

namespace {

float pctToNorm(float pct) { return juce::jlimit(0.0f, 1.0f, pct * 0.01f); }

} // namespace

DrumCrushAudioProcessor::DrumCrushAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
  applyFactoryPreset(0);
}

DrumCrushAudioProcessor::~DrumCrushAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout DrumCrushAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamPunch, "Punch", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 52.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " %"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamCrush, "Crush", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 48.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " %"; }));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamClip, "Clip", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 58.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " %"; }));
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

void DrumCrushAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  dsp_.setPunch(pctToNorm(apvts_.getRawParameterValue(kParamPunch)->load()));
  dsp_.setCrush(pctToNorm(apvts_.getRawParameterValue(kParamCrush)->load()));
  dsp_.setClip(pctToNorm(apvts_.getRawParameterValue(kParamClip)->load()));
  dsp_.setMix(pctToNorm(apvts_.getRawParameterValue(kParamMix)->load()));
  dsp_.setOutputGainDb(apvts_.getRawParameterValue(kParamOutputGain)->load());

  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
  spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
  dsp_.prepare(spec);
  setLatencySamples(dsp_.getLatencySamples());
}

void DrumCrushAudioProcessor::releaseResources() {
  dsp_.reset();
}

bool DrumCrushAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet()) {
    return false;
  }
  const auto& set = layouts.getMainOutputChannelSet();
  return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

void DrumCrushAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();

  for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch) {
    buffer.clear(ch, 0, buffer.getNumSamples());
  }

  dsp_.setPunch(pctToNorm(apvts_.getRawParameterValue(kParamPunch)->load()));
  dsp_.setCrush(pctToNorm(apvts_.getRawParameterValue(kParamCrush)->load()));
  dsp_.setClip(pctToNorm(apvts_.getRawParameterValue(kParamClip)->load()));
  dsp_.setMix(pctToNorm(apvts_.getRawParameterValue(kParamMix)->load()));
  dsp_.setOutputGainDb(apvts_.getRawParameterValue(kParamOutputGain)->load());

  dsp_.process(buffer);
}

void DrumCrushAudioProcessor::applyFactoryPreset(int index) {
  index = juce::jlimit(0, kFactoryPresetCount - 1, index);
  currentProgram_ = index;
  const auto& p = kFactoryPresets[static_cast<size_t>(index)];
  if (auto* param = apvts_.getParameter(kParamPunch)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.punch * 100.0f));
  }
  if (auto* param = apvts_.getParameter(kParamCrush)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.crush * 100.0f));
  }
  if (auto* param = apvts_.getParameter(kParamClip)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.clip * 100.0f));
  }
  if (auto* param = apvts_.getParameter(kParamMix)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.mix * 100.0f));
  }
  if (auto* param = apvts_.getParameter(kParamOutputGain)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.outputDb));
  }
}

void DrumCrushAudioProcessor::setCurrentProgram(int index) {
  applyFactoryPreset(index);
}

const juce::String DrumCrushAudioProcessor::getProgramName(int index) {
  if (index < 0 || index >= kFactoryPresetCount) {
    return {};
  }
  return kFactoryPresets[static_cast<size_t>(index)].name;
}

juce::AudioProcessorEditor* DrumCrushAudioProcessor::createEditor() {
  return new DrumCrushAudioProcessorEditor(*this);
}

void DrumCrushAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts_.copyState();
  state.setProperty("currentProgram", currentProgram_, nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void DrumCrushAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml == nullptr || !xml->hasTagName(apvts_.state.getType())) {
    return;
  }
  auto vt = juce::ValueTree::fromXml(*xml);
  apvts_.replaceState(vt);
  currentProgram_ = vt.getProperty("currentProgram", 0);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new DrumCrushAudioProcessor();
}
