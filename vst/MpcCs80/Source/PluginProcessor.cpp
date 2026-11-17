#include "PluginProcessor.h"
#include "Cs80Dsp.h"
#include "FactoryPrograms.h"

namespace {
void setParam(juce::AudioProcessorValueTreeState &apvts, const char *id, float v) {
  if (auto *p = apvts.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
} // namespace

MpcCs80AudioProcessor::MpcCs80AudioProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "CS80", createParameterLayout()) {
  applyFactoryProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcCs80AudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"cutoff", 1}, "Cutoff",
      juce::NormalisableRange<float>(20.0f, 16000.0f, 1.0f, 0.3f), 2200.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"res", 1}, "Resonance", 0.0f, 0.9f, 0.42f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"layerBalance", 1}, "Layer Balance", 0.0f, 1.0f,
      0.48f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"pwm", 1}, "PWM", 0.0f, 1.0f, 0.4f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"ringAmt", 1}, "Ring Mod", 0.0f, 0.5f, 0.1f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"subLvl", 1}, "Sub", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aftertouchAmt", 1}, "Aftertouch", 0.0f, 1.0f, 0.55f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"hpf", 1}, "HPF",
      juce::NormalisableRange<float>(30.0f, 2000.0f, 1.0f, 0.4f), 120.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fAttack", 1}, "F Attack", 0.002f, 3.0f, 0.02f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fDecay", 1}, "F Decay", 0.02f, 4.0f, 0.45f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fSustain", 1}, "F Sustain", 0.0f, 1.0f, 0.35f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fRelease", 1}, "F Release", 0.01f, 4.0f, 0.4f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aAttack", 1}, "A Attack", 0.002f, 3.0f, 0.008f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aDecay", 1}, "A Decay", 0.02f, 4.0f, 0.28f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aSustain", 1}, "A Sustain", 0.0f, 1.0f, 0.78f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aRelease", 1}, "A Release", 0.01f, 4.0f, 0.35f));
  return layout;
}

bool MpcCs80AudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  const auto out = layouts.getMainOutputChannelSet();
  return out == juce::AudioChannelSet::mono() ||
         out == juce::AudioChannelSet::stereo();
}

void MpcCs80AudioProcessor::prepareToPlay(double sampleRate,
                                         int samplesPerBlock) {
  sampleRate_ = sampleRate;
  juce::dsp::ProcessSpec spec{
      sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1};
  for (auto &v : voices_) {
    v.hpf.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    v.lpf.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    v.hpf.prepare(spec);
    v.lpf.prepare(spec);
    v.ampEnv.setSampleRate(sampleRate);
    v.filterEnv.setSampleRate(sampleRate);
  }
}

float MpcCs80AudioProcessor::renderLayer(float phase1, float phase2,
                                        float subPhase, float shape, float pwm,
                                        float ringAmt, float subLvl) {
  const float o1 = cs80::applyPwm(phase1, pwm) * 0.55f +
                   cs80::morphWave(phase1, shape) * 0.45f;
  const float o2 = cs80::morphWave(phase2, shape * 0.9f);
  const float ring = o1 * o2 * ringAmt;
  const float sub = (subPhase < 0.5f ? 1.0f : -1.0f) * subLvl;
  return o1 * 0.5f + o2 * 0.4f + ring + sub * 0.35f;
}

int MpcCs80AudioProcessor::getNumPrograms() {
  return static_cast<int>(cs80::kNumFactoryPrograms);
}

void MpcCs80AudioProcessor::setCurrentProgram(int index) {
  index = juce::jlimit(0, getNumPrograms() - 1, index);
  currentProgram_ = index;
  applyFactoryProgram(index);
}

const juce::String MpcCs80AudioProcessor::getProgramName(int index) {
  if (index >= 0 && index < getNumPrograms())
    return juce::String(
        cs80::kFactoryPrograms[static_cast<size_t>(index)].name);
  return {};
}

void MpcCs80AudioProcessor::applyFactoryProgram(int index) {
  const auto &p = cs80::kFactoryPrograms[static_cast<size_t>(index)];
  setParam(apvts_, "cutoff", p.cutoff);
  setParam(apvts_, "res", p.res);
  setParam(apvts_, "layerBalance", p.layerBalance);
  setParam(apvts_, "pwm", p.pwm);
  setParam(apvts_, "ringAmt", p.ringAmt);
  setParam(apvts_, "subLvl", p.subLvl);
  setParam(apvts_, "aftertouchAmt", p.aftertouchAmt);
  setParam(apvts_, "fAttack", p.fAttack);
  setParam(apvts_, "fDecay", p.fDecay);
  setParam(apvts_, "fSustain", p.fSustain);
  setParam(apvts_, "fRelease", p.fRelease);
  setParam(apvts_, "aAttack", p.aAttack);
  setParam(apvts_, "aDecay", p.aDecay);
  setParam(apvts_, "aSustain", p.aSustain);
  setParam(apvts_, "aRelease", p.aRelease);
}

void MpcCs80AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();

  const float baseCut = apvts_.getRawParameterValue("cutoff")->load();
  const float res = apvts_.getRawParameterValue("res")->load();
  const float layerBal = apvts_.getRawParameterValue("layerBalance")->load();
  const float pwm = apvts_.getRawParameterValue("pwm")->load();
  const float ringAmt = apvts_.getRawParameterValue("ringAmt")->load();
  const float subLvl = apvts_.getRawParameterValue("subLvl")->load();
  const float atAmt = apvts_.getRawParameterValue("aftertouchAmt")->load();
  const float hpfHz = apvts_.getRawParameterValue("hpf")->load();

  juce::ADSR::Parameters fParams{
      apvts_.getRawParameterValue("fAttack")->load(),
      apvts_.getRawParameterValue("fDecay")->load(),
      apvts_.getRawParameterValue("fSustain")->load(),
      apvts_.getRawParameterValue("fRelease")->load()};
  juce::ADSR::Parameters aParams{
      apvts_.getRawParameterValue("aAttack")->load(),
      apvts_.getRawParameterValue("aDecay")->load(),
      apvts_.getRawParameterValue("aSustain")->load(),
      apvts_.getRawParameterValue("aRelease")->load()};

  for (const auto metadata : midi) {
    const auto msg = metadata.getMessage();
    if (msg.isNoteOn()) {
      int slot = -1;
      for (int i = 0; i < kMaxVoices; ++i) {
        if (!voices_[static_cast<size_t>(i)].active) {
          slot = i;
          break;
        }
      }
      if (slot < 0)
        slot = 0;
      auto &v = voices_[static_cast<size_t>(slot)];
      v.active = true;
      v.note = msg.getNoteNumber();
      v.vel = msg.getFloatVelocity();
      v.freq = 440.0f * std::pow(2.0f, (v.note - 69) / 12.0f);
      v.pressure = 0.0f;
      v.ampEnv.setParameters(aParams);
      v.filterEnv.setParameters(fParams);
      v.ampEnv.noteOn();
      v.filterEnv.noteOn();
    } else if (msg.isNoteOff()) {
      for (auto &v : voices_) {
        if (v.active && v.note == msg.getNoteNumber()) {
          v.ampEnv.noteOff();
          v.filterEnv.noteOff();
        }
      }
    } else if (msg.isAftertouch() || msg.isChannelPressure()) {
      const float p =
          msg.isAftertouch() ? msg.getAfterTouchValue() / 127.0f : 0.5f;
      for (auto &v : voices_) {
        if (v.active)
          v.pressure = p;
      }
    }
  }

  const int n = buffer.getNumSamples();
  for (int i = 0; i < n; ++i) {
    float sum = 0.0f;
    for (auto &v : voices_) {
      if (!v.ampEnv.isActive())
        continue;

    const float inc = v.freq / static_cast<float>(sampleRate_);
    v.phase1 += inc;
    v.phase2 += inc * 1.007f;
    v.subPhase += inc * 0.5f;
    if (v.phase1 >= 1.0f)
      v.phase1 -= 1.0f;
    if (v.phase2 >= 1.0f)
      v.phase2 -= 1.0f;
    if (v.subPhase >= 1.0f)
      v.subPhase -= 1.0f;

    const float layerI =
        renderLayer(v.phase1, v.phase2, v.subPhase, 0.35f, pwm, ringAmt,
                    subLvl);
    const float layerII =
        renderLayer(v.phase1, v.phase2 + 0.03f, v.subPhase, 0.55f, pwm * 0.8f,
                    ringAmt * 0.5f, subLvl * 0.6f);
    const float dry = layerI * (1.0f - layerBal) + layerII * layerBal;

    const float fEnv = v.filterEnv.getNextSample();
    const float atBoost = v.pressure * atAmt * 4000.0f;
    const float cut = baseCut + fEnv * 6000.0f + atBoost;
    v.hpf.setCutoffFrequency(hpfHz);
    v.lpf.setCutoffFrequency(juce::jlimit(40.0f, 18000.0f, cut));
    v.lpf.setResonance(res);

    float s[1] = {dry * v.vel};
    float *ch[1] = {s};
    juce::dsp::AudioBlock<float> block(ch, 1, 1);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    v.hpf.process(ctx);
    v.lpf.process(ctx);

    const float out = s[0] * v.ampEnv.getNextSample() * 0.5f;
    sum += out;
    if (!v.ampEnv.isActive())
      v.active = false;
  }
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
      buffer.setSample(ch, i, sum);
  }
}

void MpcCs80AudioProcessor::getStateInformation(juce::MemoryBlock &dest) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  xml->setAttribute("program", currentProgram_);
  copyXmlToBinary(*xml, dest);
}

void MpcCs80AudioProcessor::setStateInformation(const void *data, int sz) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sz));
  if (xml && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    currentProgram_ = xml->getIntAttribute("program", 0);
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcCs80AudioProcessor();
}