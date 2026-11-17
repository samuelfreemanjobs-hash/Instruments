#include "PluginProcessor.h"
#include "FactoryPrograms.h"

namespace {
void setP(juce::AudioProcessorValueTreeState &a, const char *id, float v) {
  if (auto *p = a.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
} // namespace

MpcSh101AudioProcessor::MpcSh101AudioProcessor()
    : AudioProcessor(
          BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(),
                                     true)),
      apvts_(*this, nullptr, "SH101", createParameterLayout()) {
  applyProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcSh101AudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout l;
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "cutoff", "Cutoff", 40.0f, 12000.0f, 680.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>("res", "Resonance", 0.0f,
                                                   0.95f, 0.55f));
  l.add(std::make_unique<juce::AudioParameterFloat>("subLvl", "Sub", 0.0f, 1.0f,
                                                   0.65f));
  l.add(std::make_unique<juce::AudioParameterFloat>("pwm", "PWM", 0.0f, 1.0f,
                                                   0.35f));
  l.add(std::make_unique<juce::AudioParameterFloat>("glide", "Glide", 0.0f,
                                                   0.5f, 0.05f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fAttack", "F Atk", 0.001f,
                                                   2.0f, 0.003f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fDecay", "F Dec", 0.02f,
                                                   2.0f, 0.25f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fSustain", "F Sus", 0.0f,
                                                   1.0f, 0.2f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fRelease", "F Rel", 0.01f,
                                                   2.0f, 0.18f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aAttack", "A Atk", 0.001f,
                                                   2.0f, 0.002f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aDecay", "A Dec", 0.02f,
                                                   2.0f, 0.2f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aSustain", "A Sus", 0.0f,
                                                   1.0f, 0.85f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aRelease", "A Rel", 0.01f,
                                                   2.0f, 0.12f));
  return l;
}

bool MpcSh101AudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  auto o = layouts.getMainOutputChannelSet();
  return o == juce::AudioChannelSet::mono() ||
         o == juce::AudioChannelSet::stereo();
}

void MpcSh101AudioProcessor::prepareToPlay(double sampleRate, int block) {
  sr_ = sampleRate;
  juce::dsp::ProcessSpec spec{
      sampleRate, static_cast<juce::uint32>(block), 1};
  lpf_.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
  lpf_.prepare(spec);
  ampEnv_.setSampleRate(sampleRate);
  filterEnv_.setSampleRate(sampleRate);
}

float MpcSh101AudioProcessor::processFilter(float x) {
  float s[1] = {x};
  float *c[1] = {s};
  juce::dsp::AudioBlock<float> b(c, 1, 1);
  juce::dsp::ProcessContextReplacing<float> ctx(b);
  lpf_.process(ctx);
  return b.getSample(0, 0);
}

int MpcSh101AudioProcessor::getNumPrograms() { return sh101::kNumPrograms; }

void MpcSh101AudioProcessor::setCurrentProgram(int i) {
  prog_ = juce::jlimit(0, getNumPrograms() - 1, i);
  applyProgram(prog_);
}

const juce::String MpcSh101AudioProcessor::getProgramName(int i) {
  return (i >= 0 && i < getNumPrograms()) ? sh101::kPrograms[i].name : "";
}

void MpcSh101AudioProcessor::applyProgram(int i) {
  const auto &p = sh101::kPrograms[i];
  setP(apvts_, "cutoff", p.cutoff);
  setP(apvts_, "res", p.res);
  setP(apvts_, "subLvl", p.subLvl);
  setP(apvts_, "pwm", p.pwm);
  setP(apvts_, "glide", p.glide);
  setP(apvts_, "fAttack", p.fA);
  setP(apvts_, "fDecay", p.fD);
  setP(apvts_, "fSustain", p.fS);
  setP(apvts_, "fRelease", p.fR);
  setP(apvts_, "aAttack", p.aA);
  setP(apvts_, "aDecay", p.aD);
  setP(apvts_, "aSustain", p.aS);
  setP(apvts_, "aRelease", p.aR);
}

void MpcSh101AudioProcessor::processBlock(juce::AudioBuffer<float> &buf,
                                         juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buf.clear();
  for (const auto m : midi) {
    auto msg = m.getMessage();
    if (msg.isNoteOn()) {
      held_ = msg.getNoteNumber();
      targetFreq_ = 440.0f * std::pow(2.0f, (held_ - 69) / 12.0f);
      vel_ = msg.getFloatVelocity();
      if (!ampEnv_.isActive())
        freq_ = targetFreq_;
      ampEnv_.noteOn();
      filterEnv_.noteOn();
    } else if (msg.isNoteOff() && msg.getNoteNumber() == held_) {
      ampEnv_.noteOff();
      filterEnv_.noteOff();
      held_ = -1;
    }
  }
  if (!ampEnv_.isActive())
    return;

  const float cut = *apvts_.getRawParameterValue("cutoff");
  const float res = *apvts_.getRawParameterValue("res");
  const float sub = *apvts_.getRawParameterValue("subLvl");
  const float pwm = *apvts_.getRawParameterValue("pwm");
  const float glide = *apvts_.getRawParameterValue("glide");
  filterEnv_.setParameters(
      {*apvts_.getRawParameterValue("fAttack"),
       *apvts_.getRawParameterValue("fDecay"),
       *apvts_.getRawParameterValue("fSustain"),
       *apvts_.getRawParameterValue("fRelease")});
  ampEnv_.setParameters(
      {*apvts_.getRawParameterValue("aAttack"),
       *apvts_.getRawParameterValue("aDecay"),
       *apvts_.getRawParameterValue("aSustain"),
       *apvts_.getRawParameterValue("aRelease")});
  const float g =
      glide > 0.001f ? std::exp(-1.0f / static_cast<float>(glide * sr_)) : 0.0f;
  lpf_.setResonance(res);

  const int n = buf.getNumSamples();
  for (int i = 0; i < n; ++i) {
    freq_ = targetFreq_ + g * (freq_ - targetFreq_);
    phase_ += freq_ / static_cast<float>(sr_);
    subPhase_ += (freq_ * 0.5f) / static_cast<float>(sr_);
    pwmLfo_ += 5.0f / static_cast<float>(sr_);
    if (phase_ >= 1.0f)
      phase_ -= 1.0f;
    if (subPhase_ >= 1.0f)
      subPhase_ -= 1.0f;
    if (pwmLfo_ >= 1.0f)
      pwmLfo_ -= 1.0f;
    const float w = 0.15f + pwm * 0.35f +
                    std::sin(pwmLfo_ * 6.28318530718f) * pwm * 0.08f;
    const float saw = 2.0f * (phase_ - 0.5f);
    const float pulse = phase_ < w ? 1.0f : -1.0f;
    const float subSq = (subPhase_ < 0.5f ? 1.0f : -1.0f) * sub;
    const float dry = (saw * 0.55f + pulse * 0.45f + subSq) * vel_;
    const float fEnv = filterEnv_.getNextSample();
    lpf_.setCutoffFrequency(cut + fEnv * 4000.0f);
    const float out = processFilter(dry) * ampEnv_.getNextSample() * 0.58f;
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
      buf.setSample(ch, i, out);
  }
}

void MpcSh101AudioProcessor::getStateInformation(juce::MemoryBlock &d) {
  auto s = apvts_.copyState();
  auto xml = s.createXml();
  xml->setAttribute("program", prog_);
  copyXmlToBinary(*xml, d);
}

void MpcSh101AudioProcessor::setStateInformation(const void *d, int sz) {
  if (auto xml = getXmlFromBinary(d, sz))
    if (xml->hasTagName(apvts_.state.getType())) {
      apvts_.replaceState(juce::ValueTree::fromXml(*xml));
      prog_ = xml->getIntAttribute("program", 0);
    }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcSh101AudioProcessor();
}