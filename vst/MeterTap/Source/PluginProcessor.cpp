#include "PluginProcessor.h"

#include <cmath>

namespace {
float dbToLinear(float db) noexcept {
  return std::pow(10.0f, db * 0.05f);
}
} // namespace

MeterTapAudioProcessor::MeterTapAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

MeterTapAudioProcessor::~MeterTapAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
MeterTapAudioProcessor::createParameterLayout() {
  // parameter-spec.md v1 — ids must match exactly
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

void MeterTapAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/) {
  sampleRate_ = (sampleRate > 0.0) ? sampleRate : 48000.0;
  trimSmooth_.setTargetCoeff(20.0f, static_cast<float>(sampleRate_));
  const float trimDb = apvts_.getRawParameterValue(kParamTrimDb)->load();
  trimSmooth_.reset(dbToLinear(trimDb));
  peakRms_.prepare(sampleRate_);
  bridge_.reset();
  setLatencySamples(0);
}

void MeterTapAudioProcessor::releaseResources() {}

bool MeterTapAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  // architecture.md §4 — stereo in / stereo out only
  return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
      && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void MeterTapAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();

  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  if (numChannels < 2) {
    return;
  }

  float* left = buffer.getWritePointer(0);
  float* right = buffer.getWritePointer(1);

  // 1) Sanitize — non-finite → 0 (architecture.md §2)
  for (int i = 0; i < numSamples; ++i) {
    if (!std::isfinite(left[i])) {
      left[i] = 0.0f;
    }
    if (!std::isfinite(right[i])) {
      right[i] = 0.0f;
    }
  }

  const bool bypass = apvts_.getRawParameterValue(kParamBypass)->load() >= 0.5f;
  const float trimDbTarget = apvts_.getRawParameterValue(kParamTrimDb)->load();
  const float linearTarget = dbToLinear(trimDbTarget);

  // Keep smoother tracking while bypassed so un-bypass does not jump (architecture.md §2).
  if (bypass) {
    for (int i = 0; i < numSamples; ++i) {
      (void)trimSmooth_.process(linearTarget);
    }
    // Analysis tap on sanitized INPUT; output bit-identical to sanitized input.
    const float* chPtrs[2] = {left, right};
    peakRms_.process(chPtrs, 2, numSamples);
  } else {
    for (int i = 0; i < numSamples; ++i) {
      const float g = trimSmooth_.process(linearTarget);
      left[i] *= g;
      right[i] *= g;
    }
    const float* chPtrs[2] = {left, right};
    peakRms_.process(chPtrs, 2, numSamples);
  }

  bridge_.publish(0, peakRms_.blockPeak(0), peakRms_.rms(0));
  bridge_.publish(1, peakRms_.blockPeak(1), peakRms_.rms(1));
}

juce::AudioProcessorEditor* MeterTapAudioProcessor::createEditor() {
  // Custom LevelMeter editor follows design-spec.md once factory-lib/gui lands.
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
