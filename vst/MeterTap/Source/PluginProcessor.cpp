#include "PluginProcessor.h"

// Phase 2 stub — DSP bodies land in Phase 3 (PluginProcessor.cpp module commit).
MeterTapAudioProcessor::MeterTapAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

MeterTapAudioProcessor::~MeterTapAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
MeterTapAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{kParamTrimDb, 1}, "Input trim",
      juce::NormalisableRange<float>(-24.0f, 12.0f, 0.01f), 0.0f));
  params.push_back(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{kParamBypass, 1}, "Bypass", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{kParamPeakHold, 1}, "Peak hold", true));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{kParamMeterFloorDb, 1}, "Meter floor",
      juce::NormalisableRange<float>(-96.0f, -48.0f, 0.1f), -60.0f,
      juce::AudioParameterFloatAttributes().withAutomatable(false)));
  return {params.begin(), params.end()};
}

void MeterTapAudioProcessor::prepareToPlay(double, int) {}
void MeterTapAudioProcessor::releaseResources() {}

bool MeterTapAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
      && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void MeterTapAudioProcessor::processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midi) {
  midi.clear();
}

juce::AudioProcessorEditor* MeterTapAudioProcessor::createEditor() {
  return new juce::GenericAudioProcessorEditor(*this);
}

juce::AudioProcessorParameter* MeterTapAudioProcessor::getBypassParameter() {
  return apvts_.getParameter(kParamBypass);
}

void MeterTapAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void MeterTapAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
  }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new MeterTapAudioProcessor();
}
