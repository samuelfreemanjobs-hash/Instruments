#include "PluginProcessor.h"

namespace {
struct ProgramPreset {
  const char *name;
  float cutoff;
  float res;
  float detune;
  float attack;
  float decay;
  float sustain;
  float release;
};

constexpr ProgramPreset kPrograms[] = {
    {"Init", 1200.0f, 1.2f, 0.08f, 0.02f, 0.30f, 0.70f, 0.25f},
    {"Warm Pad", 2800.0f, 0.9f, 0.12f, 0.35f, 0.80f, 0.85f, 1.20f},
    {"Acid Lead", 900.0f, 5.5f, 0.18f, 0.005f, 0.15f, 0.40f, 0.12f},
    {"Soft Brass", 4200.0f, 1.8f, 0.05f, 0.08f, 0.45f, 0.75f, 0.40f},
};

void setParam(juce::AudioProcessorValueTreeState &apvts, const char *id,
              float value) {
  if (auto *p = apvts.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(value));
}
} // namespace

MpcAnalogSynthAudioProcessor::MpcAnalogSynthAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMS", createParameterLayout()) {
  for (int i = 0; i < kNumVoices; ++i)
    synth_.addVoice(new MpcSynthVoice());
  synth_.addSound(new MpcSynthSound());
  applyProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcAnalogSynthAudioProcessor::createParameterLayout() {
  // Order for Melodee K1–K3: musically important controls first.
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"cutoff", 1}, "Cutoff",
      juce::NormalisableRange<float>(40.0f, 16000.0f, 1.0f, 0.3f), 1200.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"res", 1}, "Resonance",
      juce::NormalisableRange<float>(0.7f, 8.0f, 0.01f, 1.0f), 1.2f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"detune", 1}, "Osc Detune",
      juce::NormalisableRange<float>(-0.5f, 0.5f, 0.001f, 1.0f), 0.08f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"attack", 1}, "Attack",
      juce::NormalisableRange<float>(0.005f, 2.0f, 0.001f, 0.4f), 0.02f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"decay", 1}, "Decay",
      juce::NormalisableRange<float>(0.01f, 2.0f, 0.001f, 0.4f), 0.3f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"sustain", 1}, "Sustain",
      juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f), 0.7f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"release", 1}, "Release",
      juce::NormalisableRange<float>(0.01f, 3.0f, 0.001f, 0.4f), 0.25f));
  return layout;
}

bool MpcAnalogSynthAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;
  return true;
}

void MpcAnalogSynthAudioProcessor::prepareToPlay(double sampleRate,
                                                 int samplesPerBlock) {
  synth_.setCurrentPlaybackSampleRate(sampleRate);
  juce::dsp::ProcessSpec spec{
      sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1};
  for (int i = 0; i < synth_.getNumVoices(); ++i)
    if (auto *v = dynamic_cast<MpcSynthVoice *>(synth_.getVoice(i)))
      v->prepare(spec);
}

void MpcAnalogSynthAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                              juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();

  const float c = apvts_.getRawParameterValue("cutoff")->load();
  const float r = apvts_.getRawParameterValue("res")->load();
  const float dt = apvts_.getRawParameterValue("detune")->load();
  const float a = apvts_.getRawParameterValue("attack")->load();
  const float d = apvts_.getRawParameterValue("decay")->load();
  const float s = apvts_.getRawParameterValue("sustain")->load();
  const float rel = apvts_.getRawParameterValue("release")->load();

  for (int i = 0; i < synth_.getNumVoices(); ++i)
    if (auto *v = dynamic_cast<MpcSynthVoice *>(synth_.getVoice(i)))
      v->updateParams(c, r, dt, a, d, s, rel);

  synth_.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
}

void MpcAnalogSynthAudioProcessor::setCurrentProgram(int index) {
  index = juce::jlimit(0, kNumPrograms - 1, index);
  currentProgram_ = index;
  applyProgram(index);
}

const juce::String MpcAnalogSynthAudioProcessor::getProgramName(int index) {
  if (index >= 0 && index < kNumPrograms)
    return juce::String(kPrograms[static_cast<size_t>(index)].name);
  return {};
}

void MpcAnalogSynthAudioProcessor::applyProgram(int index) {
  const auto &p = kPrograms[static_cast<size_t>(index)];
  setParam(apvts_, "cutoff", p.cutoff);
  setParam(apvts_, "res", p.res);
  setParam(apvts_, "detune", p.detune);
  setParam(apvts_, "attack", p.attack);
  setParam(apvts_, "decay", p.decay);
  setParam(apvts_, "sustain", p.sustain);
  setParam(apvts_, "release", p.release);
}

void MpcAnalogSynthAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  xml->setAttribute("program", currentProgram_);
  copyXmlToBinary(*xml, destData);
}

void MpcAnalogSynthAudioProcessor::setStateInformation(const void *data,
                                                       int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    currentProgram_ = xml->getIntAttribute("program", 0);
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcAnalogSynthAudioProcessor();
}