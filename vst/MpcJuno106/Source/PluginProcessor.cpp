#include "PluginProcessor.h"

namespace {
void setP(juce::AudioProcessorValueTreeState &a, const char *id, float v) {
  if (auto *p = a.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
} // namespace

MpcJuno106AudioProcessor::MpcJuno106AudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Out", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "JUNO106", createParameterLayout()) {
  for (int i = 0; i < kVoices; ++i)
    synth_.addVoice(new JunoVoice());
  synth_.addSound(&sound_);
  applyProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcJuno106AudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout l;
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "cutoff", "Cutoff", juce::NormalisableRange<float>(20.0f, 12000.0f, 1.0f, 0.3f),
      2000.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>("res", "Resonance", 0.0f, 1.0f, 0.15f));
  l.add(std::make_unique<juce::AudioParameterFloat>("chorus", "Chorus", 0.0f, 1.0f, 0.35f));
  l.add(std::make_unique<juce::AudioParameterFloat>("dco_saw", "Saw", 0.0f, 1.0f, 0.8f));
  l.add(std::make_unique<juce::AudioParameterFloat>("dco_pulse", "Pulse", 0.0f, 1.0f, 0.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>("pulse_width", "PWM", 0.1f, 0.9f, 0.5f));
  l.add(std::make_unique<juce::AudioParameterFloat>("sub_level", "Sub", 0.0f, 1.0f, 0.3f));
  l.add(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.001f, 3.0f, 0.005f));
  l.add(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.001f, 5.0f, 0.4f));
  l.add(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.0f, 1.0f, 0.85f));
  l.add(std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.001f, 8.0f, 0.3f));
  l.add(std::make_unique<juce::AudioParameterFloat>("lfo_rate", "LFO Rate", 0.05f, 20.0f, 3.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>("lfo_to_pwm", "LFO PWM", 0.0f, 1.0f, 0.0f));
  return l;
}

bool MpcJuno106AudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
  auto o = layouts.getMainOutputChannelSet();
  return o == juce::AudioChannelSet::mono() || o == juce::AudioChannelSet::stereo();
}

void MpcJuno106AudioProcessor::prepareToPlay(double sampleRate, int block) {
  sr_ = sampleRate;
  synth_.setCurrentPlaybackSampleRate(sampleRate);
  juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32>(block), 1};
  for (int i = 0; i < synth_.getNumVoices(); ++i)
    if (auto *v = dynamic_cast<JunoVoice *>(synth_.getVoice(i)))
      v->prepare(spec);
  delayL_.fill(0.0f);
  delayR_.fill(0.0f);
  writePos_ = 0;
}

void MpcJuno106AudioProcessor::processChorus(juce::AudioBuffer<float> &buf, float mix) {
  if (mix < 0.001f || buf.getNumChannels() < 2)
    return;
  const float rate = 0.4f;
  const int base = static_cast<int>(0.012f * sr_);
  const int depth = static_cast<int>(0.004f * sr_);
  const int mask = static_cast<int>(delayL_.size()) - 1;

  auto *l = buf.getWritePointer(0);
  auto *r = buf.getWritePointer(1);
  const int n = buf.getNumSamples();

  for (int i = 0; i < n; ++i) {
    chorusPhase_ += rate / static_cast<float>(sr_);
    if (chorusPhase_ >= 1.0f)
      chorusPhase_ -= 1.0f;
    const float mod = std::sin(chorusPhase_ * juce::MathConstants<float>::twoPi);
    const int offL = base + static_cast<int>(mod * depth);
    const int offR = base + static_cast<int>(-mod * depth);
    const float dryL = l[i];
    const float dryR = r[i];
    delayL_[static_cast<size_t>(writePos_ & mask)] = dryL;
    delayR_[static_cast<size_t>(writePos_ & mask)] = dryR;
    const int rpL = (writePos_ - offL) & mask;
    const int rpR = (writePos_ - offR) & mask;
    l[i] = dryL * (1.0f - mix * 0.5f) + delayL_[static_cast<size_t>(rpL)] * mix;
    r[i] = dryR * (1.0f - mix * 0.5f) + delayR_[static_cast<size_t>(rpR)] * mix;
    ++writePos_;
  }
}

void MpcJuno106AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  const float c = apvts_.getRawParameterValue("cutoff")->load();
  const float res = apvts_.getRawParameterValue("res")->load();
  const float chorus = apvts_.getRawParameterValue("chorus")->load();
  const float saw = apvts_.getRawParameterValue("dco_saw")->load();
  const float pulse = apvts_.getRawParameterValue("dco_pulse")->load();
  const float pw = apvts_.getRawParameterValue("pulse_width")->load();
  const float sub = apvts_.getRawParameterValue("sub_level")->load();
  const float a = apvts_.getRawParameterValue("attack")->load();
  const float d = apvts_.getRawParameterValue("decay")->load();
  const float s = apvts_.getRawParameterValue("sustain")->load();
  const float rel = apvts_.getRawParameterValue("release")->load();
  const float lfoR = apvts_.getRawParameterValue("lfo_rate")->load();
  const float lfoP = apvts_.getRawParameterValue("lfo_to_pwm")->load();

  for (int i = 0; i < synth_.getNumVoices(); ++i) {
    if (auto *v = dynamic_cast<JunoVoice *>(synth_.getVoice(i))) {
      v->updateParams(c, res, saw, pulse, pw, sub, a, d, s, rel);
      v->setGlobals(lfoR, lfoP);
    }
  }

  buffer.clear();
  synth_.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
  processChorus(buffer, chorus);
}

void MpcJuno106AudioProcessor::setCurrentProgram(int i) {
  prog_ = juce::jlimit(0, getNumPrograms() - 1, i);
  applyProgram(prog_);
}

const juce::String MpcJuno106AudioProcessor::getProgramName(int i) {
  return (i >= 0 && i < getNumPrograms()) ? juno106::kPrograms[i].name : "";
}

void MpcJuno106AudioProcessor::applyProgram(int i) {
  const auto &p = juno106::kPrograms[i];
  setP(apvts_, "cutoff", p.cutoff);
  setP(apvts_, "res", p.res);
  setP(apvts_, "chorus", p.chorus);
  setP(apvts_, "dco_saw", p.saw);
  setP(apvts_, "dco_pulse", p.pulse);
  setP(apvts_, "pulse_width", p.pw);
  setP(apvts_, "sub_level", p.sub);
  setP(apvts_, "attack", p.attack);
  setP(apvts_, "decay", p.decay);
  setP(apvts_, "sustain", p.sustain);
  setP(apvts_, "release", p.release);
  setP(apvts_, "lfo_rate", p.lfoRate);
  setP(apvts_, "lfo_to_pwm", p.lfoPwm);
}

void MpcJuno106AudioProcessor::getStateInformation(juce::MemoryBlock &d) {
  auto xml = apvts_.copyState().createXml();
  xml->setAttribute("program", prog_);
  copyXmlToBinary(*xml, d);
}

void MpcJuno106AudioProcessor::setStateInformation(const void *d, int sz) {
  if (auto xml = getXmlFromBinary(d, sz))
    if (xml->hasTagName(apvts_.state.getType())) {
      apvts_.replaceState(juce::ValueTree::fromXml(*xml));
      prog_ = xml->getIntAttribute("program", 0);
    }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcJuno106AudioProcessor();
}