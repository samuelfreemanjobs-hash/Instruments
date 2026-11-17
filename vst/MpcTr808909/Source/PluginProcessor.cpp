#include "PluginProcessor.h"
#include "FactoryPrograms.h"

namespace {
void setP(juce::AudioProcessorValueTreeState &a, const char *id, float v) {
  if (auto *p = a.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
constexpr int kKick = 36, kSnare = 38, kHat = 42;
} // namespace

MpcTr808909AudioProcessor::MpcTr808909AudioProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "TR808909", createParameterLayout()) {
  applyProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcTr808909AudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout l;
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "focus_decay", "Focus Decay", 0.01f, 3.0f, 0.55f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "focus_tune", "Focus Tune", 20.0f, 400.0f, 55.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.0f, 1.0f,
                                                   0.2f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "kickDecay", "Kick Decay", 0.05f, 2.0f, 0.55f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "kickStart", "Kick Start Hz", 40.0f, 400.0f, 220.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "kickEnd", "Kick End Hz", 20.0f, 120.0f, 48.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "kickSweep", "Kick Sweep", 0.01f, 0.5f, 0.12f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "snareTone", "Snare Tone Hz", 80.0f, 400.0f, 180.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "snareNoise", "Snare Noise", 0.0f, 1.0f, 0.72f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "snareDecay", "Snare Decay", 0.02f, 1.0f, 0.22f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "hatDecay", "Hat Decay", 0.005f, 0.3f, 0.08f));
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "hatNoise", "Hat Noise", 0.0f, 1.0f, 0.85f));
  l.add(std::make_unique<juce::AudioParameterChoice>(
      "focus_voice", "Focus Voice",
      juce::StringArray{"Kick", "Snare", "Hat", "Cowbell"}, 0));
  l.add(std::make_unique<juce::AudioParameterChoice>(
      "kit", "Kit", juce::StringArray{"808", "909", "Hybrid"}, 0));
  return l;
}

bool MpcTr808909AudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  auto o = layouts.getMainOutputChannelSet();
  return o == juce::AudioChannelSet::mono() ||
         o == juce::AudioChannelSet::stereo();
}

void MpcTr808909AudioProcessor::prepareToPlay(double sampleRate, int) {
  sr_ = sampleRate;
}

int MpcTr808909AudioProcessor::getNumPrograms() {
  return tr808909::kNumPrograms;
}

void MpcTr808909AudioProcessor::setCurrentProgram(int i) {
  prog_ = juce::jlimit(0, getNumPrograms() - 1, i);
  applyProgram(prog_);
}

const juce::String MpcTr808909AudioProcessor::getProgramName(int i) {
  return (i >= 0 && i < getNumPrograms()) ? tr808909::kPrograms[i].name : "";
}

void MpcTr808909AudioProcessor::applyProgram(int i) {
  const auto &p = tr808909::kPrograms[i];
  setP(apvts_, "kickDecay", p.kickDecay);
  setP(apvts_, "kickStart", p.kickStart);
  setP(apvts_, "kickEnd", p.kickEnd);
  setP(apvts_, "kickSweep", p.kickSweep);
  setP(apvts_, "snareTone", p.snareTone);
  setP(apvts_, "snareNoise", p.snareNoise);
  setP(apvts_, "snareDecay", p.snareDecay);
  setP(apvts_, "hatDecay", p.hatDecay);
  setP(apvts_, "hatNoise", p.hatNoise);
  setP(apvts_, "drive", p.drive);
  setP(apvts_, "focus_tune", p.kickStart);
  setP(apvts_, "focus_decay", p.kickDecay);
}

float MpcTr808909AudioProcessor::renderKick() {
  if (!kickOn_ || kickAmp_ < 1.0e-5f)
    return 0.0f;
  const float decay = *apvts_.getRawParameterValue("kickDecay");
  const float start = *apvts_.getRawParameterValue("kickStart");
  const float end = *apvts_.getRawParameterValue("kickEnd");
  const float sweep = *apvts_.getRawParameterValue("kickSweep");
  const float tune = *apvts_.getRawParameterValue("focus_tune");
  (void)tune;
  kickEnvT_ += 1.0f / static_cast<float>(sr_);
  const float t = juce::jmin(kickEnvT_ / sweep, 1.0f);
  kickFreq_ = start + (end - start) * t;
  kickPhase_ += kickFreq_ / static_cast<float>(sr_);
  if (kickPhase_ >= 1.0f)
    kickPhase_ -= 1.0f;
  kickAmp_ *= std::exp(-1.0f / (decay * static_cast<float>(sr_)));
  return std::sin(kickPhase_ * 6.28318530718f) * kickAmp_ * kickVel_;
}

float MpcTr808909AudioProcessor::renderSnare() {
  if (!snareOn_ || snareAmp_ < 1.0e-5f)
    return 0.0f;
  const float decay = *apvts_.getRawParameterValue("snareDecay");
  const float toneHz = *apvts_.getRawParameterValue("snareTone");
  const float noiseAmt = *apvts_.getRawParameterValue("snareNoise");
  snareTone_ = toneHz;
  snarePhase_ += snareTone_ / static_cast<float>(sr_);
  if (snarePhase_ >= 1.0f)
    snarePhase_ -= 1.0f;
  snareAmp_ *= std::exp(-1.0f / (decay * static_cast<float>(sr_)));
  const float tone = std::sin(snarePhase_ * 6.28318530718f) * (1.0f - noiseAmt);
  const float noise = (rng_.nextFloat() * 2.0f - 1.0f) * noiseAmt;
  return (tone + noise) * snareAmp_ * snareVel_;
}

float MpcTr808909AudioProcessor::renderHat() {
  if (!hatOn_ || hatAmp_ < 1.0e-5f)
    return 0.0f;
  const float decay = *apvts_.getRawParameterValue("hatDecay");
  const float noiseAmt = *apvts_.getRawParameterValue("hatNoise");
  hatPhase_ += 8000.0f / static_cast<float>(sr_);
  if (hatPhase_ >= 1.0f)
    hatPhase_ -= 1.0f;
  hatAmp_ *= std::exp(-1.0f / (decay * static_cast<float>(sr_)));
  const float sq = hatPhase_ < 0.5f ? 1.0f : -1.0f;
  const float noise = (rng_.nextFloat() * 2.0f - 1.0f) * noiseAmt;
  return (sq * (1.0f - noiseAmt) * 0.35f + noise) * hatAmp_ * hatVel_;
}

void MpcTr808909AudioProcessor::processBlock(juce::AudioBuffer<float> &buf,
                                              juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  for (const auto m : midi) {
    auto msg = m.getMessage();
    if (!msg.isNoteOn())
      continue;
    const int n = msg.getNoteNumber();
    const float v = msg.getFloatVelocity();
  if (n == kKick) {
      kickOn_ = true;
      kickAmp_ = 1.0f;
      kickEnvT_ = 0.0f;
      kickPhase_ = 0.0f;
      kickVel_ = v;
    } else if (n == kSnare) {
      snareOn_ = true;
      snareAmp_ = 1.0f;
      snarePhase_ = 0.0f;
      snareVel_ = v;
    } else if (n == kHat) {
      hatOn_ = true;
      hatAmp_ = 1.0f;
      hatPhase_ = 0.0f;
      hatVel_ = v;
    }
  }
  const float drive = *apvts_.getRawParameterValue("drive");
  const int ns = buf.getNumSamples();
  for (int i = 0; i < ns; ++i) {
    float out = renderKick() + renderSnare() + renderHat();
    out *= 1.0f + drive * 2.5f;
    out = std::tanh(out * (1.0f + drive));
    if (kickAmp_ < 1.0e-5f)
      kickOn_ = false;
    if (snareAmp_ < 1.0e-5f)
      snareOn_ = false;
    if (hatAmp_ < 1.0e-5f)
      hatOn_ = false;
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
      buf.setSample(ch, i, out * 0.75f);
  }
}

void MpcTr808909AudioProcessor::getStateInformation(juce::MemoryBlock &d) {
  auto s = apvts_.copyState();
  auto xml = s.createXml();
  xml->setAttribute("program", prog_);
  copyXmlToBinary(*xml, d);
}

void MpcTr808909AudioProcessor::setStateInformation(const void *d, int sz) {
  if (auto xml = getXmlFromBinary(d, sz))
    if (xml->hasTagName(apvts_.state.getType())) {
      apvts_.replaceState(juce::ValueTree::fromXml(*xml));
      prog_ = xml->getIntAttribute("program", 0);
    }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcTr808909AudioProcessor();
}