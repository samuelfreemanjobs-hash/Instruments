#include "PluginProcessor.h"

namespace {
constexpr const char *kLayerBlend = "layer_blend";
constexpr const char *kK1 = "k1";
constexpr const char *kK2 = "k2";
constexpr const char *kK3 = "k3";
constexpr const char *kMaster = "master";
} // namespace

MpcPlayTemplateAudioProcessor::MpcPlayTemplateAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Out", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "MPCPLAY", createParameterLayout()) {
  for (int i = 0; i < mpc_play::kVoiceCount; ++i)
    synth_.addVoice(new play_series::PlaySynthVoice());
  synth_.addSound(&sound_);
  setCurrentProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcPlayTemplateAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(kK1, "K1", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(kK2, "K2", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(kK3, "K3", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(kLayerBlend, "A/B Blend", 0.0f, 1.0f, 0.5f));
  for (int i = 4; i <= play_series::kMacroCount; ++i)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "macro" + juce::String(i), "Macro " + juce::String(i), 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(kMaster, "Master", 0.0f, 1.0f, 0.8f));
  return layout;
}

bool MpcPlayTemplateAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
  return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void MpcPlayTemplateAudioProcessor::prepareToPlay(double sampleRate, int) {
  synth_.setCurrentPlaybackSampleRate(sampleRate);
}

play_series::MacroTargets MpcPlayTemplateAudioProcessor::readTargets() const noexcept {
  std::array<float, play_series::kMacroCount> macros{};
  macros[0] = apvts_.getRawParameterValue(kK1)->load();
  macros[1] = apvts_.getRawParameterValue(kK2)->load();
  macros[2] = apvts_.getRawParameterValue(kK3)->load();
  for (int i = 3; i < play_series::kMacroCount; ++i) {
    const auto id = "macro" + juce::String(i + 1);
    if (auto *p = apvts_.getRawParameterValue(id))
      macros[static_cast<size_t>(i)] = p->load();
  }
  auto t = router_.compute(macros, apvts_.getRawParameterValue(kK2)->load(), 1200.0f);
  t.layerABlend = apvts_.getRawParameterValue(kLayerBlend)->load();
  return t;
}

void MpcPlayTemplateAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                 juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  const auto targets = readTargets();
  const float master = apvts_.getRawParameterValue(kMaster)->load();
  for (int i = 0; i < synth_.getNumVoices(); ++i) {
    if (auto *v = dynamic_cast<play_series::PlaySynthVoice *>(synth_.getVoice(i))) {
      v->setMacroTargets(targets);
      v->setMasterGain(master);
    }
  }
  buffer.clear();
  synth_.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
}

void MpcPlayTemplateAudioProcessor::setCurrentProgram(int index) {
  currentProgram_ = juce::jlimit(0, mpc_play::kProgramCount - 1, index);
  const auto &p = mpc_play::kPrograms[static_cast<size_t>(currentProgram_)];
  if (auto *par = apvts_.getParameter(kLayerBlend))
    par->setValueNotifyingHost(par->convertTo0to1(p.blend));
  if (auto *par = apvts_.getParameter(kK1))
    par->setValueNotifyingHost(par->convertTo0to1(p.k1));
  if (auto *par = apvts_.getParameter(kK2))
    par->setValueNotifyingHost(par->convertTo0to1(p.k2));
  if (auto *par = apvts_.getParameter(kK3))
    par->setValueNotifyingHost(par->convertTo0to1(p.k3));
}

const juce::String MpcPlayTemplateAudioProcessor::getProgramName(int index) {
  if (index < 0 || index >= mpc_play::kProgramCount)
    return {};
  return mpc_play::kPrograms[static_cast<size_t>(index)].name;
}

void MpcPlayTemplateAudioProcessor::getStateInformation(juce::MemoryBlock &dest) {
  if (auto xml = apvts_.copyState().createXml()) {
    xml->setAttribute("program", currentProgram_);
    copyXmlToBinary(*xml, dest);
  }
}

void MpcPlayTemplateAudioProcessor::setStateInformation(const void *data, int size) {
  if (auto xml = getXmlFromBinary(data, size))
    if (xml->hasTagName(apvts_.state.getType())) {
      apvts_.replaceState(juce::ValueTree::fromXml(*xml));
      currentProgram_ = xml->getIntAttribute("program", 0);
    }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcPlayTemplateAudioProcessor();
}