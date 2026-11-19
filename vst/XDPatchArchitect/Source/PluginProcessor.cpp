#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
constexpr const char *kSend = "send_patch";
constexpr float kCcEpsilon = 0.002f;
} // namespace

XDPatchArchitectAudioProcessor::XDPatchArchitectAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
  lastSent_.fill(-1.f);
}

XDPatchArchitectAudioProcessor::~XDPatchArchitectAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout XDPatchArchitectAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  for (const auto &binding : xd::kCcMap) {
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        binding.paramId, juce::String(binding.paramId).replaceCharacter('_', ' '),
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
  }
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "mutate_amount", "Mutate", juce::NormalisableRange<float>(0.05f, 0.5f, 0.01f), 0.15f));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kSend, "Send Patch", false));
  return {params.begin(), params.end()};
}

void XDPatchArchitectAudioProcessor::prepareToPlay(double, int) { lastSent_.fill(-1.f); }

void XDPatchArchitectAudioProcessor::releaseResources() {}

bool XDPatchArchitectAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
  const auto &out = layouts.getMainOutputChannelSet();
  return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void XDPatchArchitectAudioProcessor::queueCc(juce::MidiBuffer &midi, int sampleOffset, std::uint8_t cc,
                                             float norm01) {
  const auto value = xd::normToCc(norm01);
  midi.addEvent(juce::MidiMessage::controllerEvent(1, cc, value), sampleOffset);
}

void XDPatchArchitectAudioProcessor::sendAllCc(juce::MidiBuffer &midi, int sampleOffset) {
  for (size_t i = 0; i < xd::kCcMap.size(); ++i) {
    const auto &binding = xd::kCcMap[i];
    const float v = apvts_.getRawParameterValue(binding.paramId)->load();
    queueCc(midi, sampleOffset, binding.cc, v);
    lastSent_[i] = v;
  }
}

void XDPatchArchitectAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();

  const bool sendNow = apvts_.getRawParameterValue(kSend)->load() >= 0.5f;
  if (sendNow) {
    sendAllCc(midi, 0);
    if (auto *sendParam = dynamic_cast<juce::AudioParameterBool *>(apvts_.getParameter(kSend))) {
      sendParam->setValueNotifyingHost(0.f);
    }
    return;
  }

  for (size_t i = 0; i < xd::kCcMap.size(); ++i) {
    const auto &binding = xd::kCcMap[i];
    const float v = apvts_.getRawParameterValue(binding.paramId)->load();
    if (std::abs(v - lastSent_[i]) > kCcEpsilon) {
      queueCc(midi, 0, binding.cc, v);
      lastSent_[i] = v;
    }
  }
}

juce::AudioProcessorEditor *XDPatchArchitectAudioProcessor::createEditor() {
  return new XDPatchArchitectAudioProcessorEditor(*this);
}

void XDPatchArchitectAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void XDPatchArchitectAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    lastSent_.fill(-1.f);
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new XDPatchArchitectAudioProcessor(); }
