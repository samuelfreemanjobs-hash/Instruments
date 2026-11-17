#include "PluginProcessor.h"
#include "FactoryPrograms.h"

namespace {
void setP(juce::AudioProcessorValueTreeState &a, const char *id, float v) {
  if (auto *p = a.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
} // namespace

MpcMinimoogAudioProcessor::MpcMinimoogAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "MINIMOOG", createParameterLayout()) {
  ladder_.setMode(juce::dsp::LadderFilterMode::LPF24);
  applyProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcMinimoogAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout l;
  l.add(std::make_unique<juce::AudioParameterFloat>(
      "cutoff", "Cutoff", 30.0f, 16000.0f, 520.0f));
  l.add(std::make_unique<juce::AudioParameterFloat>("res", "Emphasis", 0.0f,
                                                   0.95f, 0.45f));
  l.add(std::make_unique<juce::AudioParameterFloat>("glide", "Glide", 0.0f, 0.5f,
                                                   0.08f));
  l.add(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 1.0f, 5.0f,
                                                   1.6f));
  l.add(std::make_unique<juce::AudioParameterFloat>("osc2Det", "Osc2 Det", -1.0f,
                                                   1.0f, 0.07f));
  l.add(std::make_unique<juce::AudioParameterFloat>("osc3Det", "Osc3 Det", -1.0f,
                                                   1.0f, -0.07f));
  l.add(std::make_unique<juce::AudioParameterFloat>("noiseLvl", "Noise", 0.0f,
                                                   0.3f, 0.04f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fAttack", "F Atk", 0.001f,
                                                   3.0f, 0.01f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fDecay", "F Dec", 0.02f,
                                                   4.0f, 0.35f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fSustain", "F Sus", 0.0f,
                                                   1.0f, 0.28f));
  l.add(std::make_unique<juce::AudioParameterFloat>("fRelease", "F Rel", 0.01f,
                                                   4.0f, 0.25f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aAttack", "A Atk", 0.001f,
                                                   2.0f, 0.008f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aDecay", "A Dec", 0.02f,
                                                   3.0f, 0.4f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aSustain", "A Sus", 0.0f,
                                                   1.0f, 0.82f));
  l.add(std::make_unique<juce::AudioParameterFloat>("aRelease", "A Rel", 0.01f,
                                                   3.0f, 0.22f));
  return l;
}

bool MpcMinimoogAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  auto o = layouts.getMainOutputChannelSet();
  return o == juce::AudioChannelSet::mono() ||
         o == juce::AudioChannelSet::stereo();
}

void MpcMinimoogAudioProcessor::prepareToPlay(double sampleRate, int block) {
  sr_ = sampleRate;
  juce::dsp::ProcessSpec spec{
      sampleRate, static_cast<juce::uint32>(block), 1};
  ladder_.prepare(spec);
  ampEnv_.setSampleRate(sampleRate);
  filterEnv_.setSampleRate(sampleRate);
}

float MpcMinimoogAudioProcessor::ladderOne(float x) {
  float s[1] = {x};
  float *c[1] = {s};
  juce::dsp::AudioBlock<float> b(c, 1, 1);
  juce::dsp::ProcessContextReplacing<float> ctx(b);
  ladder_.process(ctx);
  return b.getSample(0, 0);
}

int MpcMinimoogAudioProcessor::getNumPrograms() {
  return minimoog::kNumPrograms;
}

void MpcMinimoogAudioProcessor::setCurrentProgram(int i) {
  prog_ = juce::jlimit(0, getNumPrograms() - 1, i);
  applyProgram(prog_);
}

const juce::String MpcMinimoogAudioProcessor::getProgramName(int i) {
  return (i >= 0 && i < getNumPrograms()) ? minimoog::kPrograms[i].name : "";
}

void MpcMinimoogAudioProcessor::applyProgram(int i) {
  const auto &p = minimoog::kPrograms[i];
  setP(apvts_, "cutoff", p.cutoff);
  setP(apvts_, "res", p.res);
  setP(apvts_, "glide", p.glide);
  setP(apvts_, "drive", p.drive);
  setP(apvts_, "osc2Det", p.osc2Det);
  setP(apvts_, "osc3Det", p.osc3Det);
  setP(apvts_, "noiseLvl", p.noiseLvl);
  setP(apvts_, "fAttack", p.fA);
  setP(apvts_, "fDecay", p.fD);
  setP(apvts_, "fSustain", p.fS);
  setP(apvts_, "fRelease", p.fR);
  setP(apvts_, "aAttack", p.aA);
  setP(apvts_, "aDecay", p.aD);
  setP(apvts_, "aSustain", p.aS);
  setP(apvts_, "aRelease", p.aR);
}

void MpcMinimoogAudioProcessor::processBlock(juce::AudioBuffer<float> &buf,
                                            juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buf.clear();
  for (const auto m : midi) {
    auto msg = m.getMessage();
    if (msg.isNoteOn()) {
      held_ = msg.getNoteNumber();
      target_ = 440.0f * std::pow(2.0f, (held_ - 69) / 12.0f);
      vel_ = msg.getFloatVelocity();
      if (!ampEnv_.isActive())
        freq_ = target_;
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
  const float glide = *apvts_.getRawParameterValue("glide");
  const float drive = *apvts_.getRawParameterValue("drive");
  const float d2 = *apvts_.getRawParameterValue("osc2Det");
  const float d3 = *apvts_.getRawParameterValue("osc3Det");
  const float noise = *apvts_.getRawParameterValue("noiseLvl");
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
  ladder_.setResonance(res);
  ladder_.setDrive(drive);
  const float g =
      glide > 0.001f ? std::exp(-1.0f / static_cast<float>(glide * sr_)) : 0.0f;
  const float r2 = std::pow(2.0f, d2 / 12.0f);
  const float r3 = std::pow(2.0f, d3 / 12.0f);

  const int n = buf.getNumSamples();
  for (int i = 0; i < n; ++i) {
    freq_ = target_ + g * (freq_ - target_);
    const float sr = static_cast<float>(sr_);
    p1_ += freq_ / sr;
    p2_ += (freq_ * r2) / sr;
    p3_ += (freq_ * r3) / sr;
    if (p1_ >= 1.0f)
      p1_ -= 1.0f;
    if (p2_ >= 1.0f)
      p2_ -= 1.0f;
    if (p3_ >= 1.0f)
      p3_ -= 1.0f;
    auto saw = [](float ph) { return 2.0f * (ph - 0.5f); };
    auto sqr = [](float ph) { return ph < 0.5f ? 1.0f : -1.0f; };
    const float dry = (saw(p1_) * 0.4f + sqr(p2_) * 0.35f + saw(p3_) * 0.3f +
                       (noise_.nextFloat() * 2.0f - 1.0f) * noise) *
                      vel_;
    const float fEnv = filterEnv_.getNextSample();
    ladder_.setCutoffFrequencyHz(cut + fEnv * 5000.0f);
    const float filt = ladderOne(dry);
    const float out = filt * ampEnv_.getNextSample() * 0.55f;
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
      buf.setSample(ch, i, out);
  }
}

void MpcMinimoogAudioProcessor::getStateInformation(juce::MemoryBlock &d) {
  auto s = apvts_.copyState();
  auto xml = s.createXml();
  xml->setAttribute("program", prog_);
  copyXmlToBinary(*xml, d);
}

void MpcMinimoogAudioProcessor::setStateInformation(const void *d, int sz) {
  if (auto xml = getXmlFromBinary(d, sz))
    if (xml->hasTagName(apvts_.state.getType())) {
      apvts_.replaceState(juce::ValueTree::fromXml(*xml));
      prog_ = xml->getIntAttribute("program", 0);
    }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcMinimoogAudioProcessor();
}