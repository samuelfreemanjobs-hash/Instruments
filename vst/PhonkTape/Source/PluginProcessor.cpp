#include "PluginProcessor.h"

#include "PluginEditor.h"

namespace {

float pctToNorm(float pct) { return juce::jlimit(0.0f, 1.0f, pct * 0.01f); }

} // namespace

PhonkTapeAudioProcessor::PhonkTapeAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
  applyFactoryPreset(0);
}

PhonkTapeAudioProcessor::~PhonkTapeAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout PhonkTapeAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  auto pct = [](float v, int) { return juce::String(v, 1) + " %"; };

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamDrive, "Drive", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 45.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter, pct));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamWow, "Wow", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 35.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter, pct));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamFlutter, "Flutter", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 40.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter, pct));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamAge, "Age", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 30.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter, pct));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamNoise, "Noise", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 15.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter, pct));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamMix, "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter, pct));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamOutputGain, "Output", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
      juce::String(), juce::AudioProcessorParameter::genericParameter,
      [](float v, int) { return juce::String(v, 1) + " dB"; }));
  return {params.begin(), params.end()};
}

void PhonkTapeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  dsp_.setDrive(pctToNorm(apvts_.getRawParameterValue(kParamDrive)->load()));
  dsp_.setWow(pctToNorm(apvts_.getRawParameterValue(kParamWow)->load()));
  dsp_.setFlutter(pctToNorm(apvts_.getRawParameterValue(kParamFlutter)->load()));
  dsp_.setAge(pctToNorm(apvts_.getRawParameterValue(kParamAge)->load()));
  dsp_.setNoise(pctToNorm(apvts_.getRawParameterValue(kParamNoise)->load()));
  dsp_.setMix(pctToNorm(apvts_.getRawParameterValue(kParamMix)->load()));
  dsp_.setOutputGainDb(apvts_.getRawParameterValue(kParamOutputGain)->load());

  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
  spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
  dsp_.prepare(spec);
  setLatencySamples(dsp_.getLatencySamples());
}

void PhonkTapeAudioProcessor::releaseResources() {
  dsp_.reset();
}

bool PhonkTapeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet()) {
    return false;
  }
  const auto& set = layouts.getMainOutputChannelSet();
  return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

void PhonkTapeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();

  for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch) {
    buffer.clear(ch, 0, buffer.getNumSamples());
  }

  dsp_.setDrive(pctToNorm(apvts_.getRawParameterValue(kParamDrive)->load()));
  dsp_.setWow(pctToNorm(apvts_.getRawParameterValue(kParamWow)->load()));
  dsp_.setFlutter(pctToNorm(apvts_.getRawParameterValue(kParamFlutter)->load()));
  dsp_.setAge(pctToNorm(apvts_.getRawParameterValue(kParamAge)->load()));
  dsp_.setNoise(pctToNorm(apvts_.getRawParameterValue(kParamNoise)->load()));
  dsp_.setMix(pctToNorm(apvts_.getRawParameterValue(kParamMix)->load()));
  dsp_.setOutputGainDb(apvts_.getRawParameterValue(kParamOutputGain)->load());

  dsp_.process(buffer);
}

void PhonkTapeAudioProcessor::applyFactoryPreset(int index) {
  index = juce::jlimit(0, kFactoryPresetCount - 1, index);
  currentProgram_ = index;
  const auto& p = kFactoryPresets[static_cast<size_t>(index)];
  const auto setPct = [this](const char* id, float v01) {
    if (auto* param = apvts_.getParameter(id)) {
      param->setValueNotifyingHost(param->convertTo0to1(v01 * 100.0f));
    }
  };
  setPct(kParamDrive, p.drive);
  setPct(kParamWow, p.wow);
  setPct(kParamFlutter, p.flutter);
  setPct(kParamAge, p.age);
  setPct(kParamNoise, p.noise);
  setPct(kParamMix, p.mix);
  if (auto* param = apvts_.getParameter(kParamOutputGain)) {
    param->setValueNotifyingHost(param->convertTo0to1(p.outputDb));
  }
}

void PhonkTapeAudioProcessor::setCurrentProgram(int index) {
  applyFactoryPreset(index);
}

const juce::String PhonkTapeAudioProcessor::getProgramName(int index) {
  if (index < 0 || index >= kFactoryPresetCount) {
    return {};
  }
  return kFactoryPresets[static_cast<size_t>(index)].name;
}

juce::AudioProcessorEditor* PhonkTapeAudioProcessor::createEditor() {
  return new PhonkTapeAudioProcessorEditor(*this);
}

void PhonkTapeAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts_.copyState();
  state.setProperty("currentProgram", currentProgram_, nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void PhonkTapeAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml == nullptr || !xml->hasTagName(apvts_.state.getType())) {
    return;
  }
  auto vt = juce::ValueTree::fromXml(*xml);
  apvts_.replaceState(vt);
  currentProgram_ = vt.getProperty("currentProgram", 0);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new PhonkTapeAudioProcessor();
}
