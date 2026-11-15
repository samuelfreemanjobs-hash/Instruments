#include "PluginProcessor.h"
#include "FactoryPrograms.h"
#include "MessengerDsp.h"

namespace {
void setParam(juce::AudioProcessorValueTreeState &apvts, const char *id, float v) {
  if (auto *p = apvts.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
} // namespace

MpcMessengerAudioProcessor::MpcMessengerAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "MESSENGER", createParameterLayout()) {
  ladder_.setMode(juce::dsp::LadderFilterMode::LPF24);
  applyFactoryProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcMessengerAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"cutoff", 1}, "Cutoff",
      juce::NormalisableRange<float>(20.0f, 18000.0f, 1.0f, 0.28f), 520.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"res", 1}, "Resonance", 0.0f, 0.96f, 0.38f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"resBass", 1}, "Res Bass", 0.0f, 1.0f, 0.55f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fold", 1}, "Wavefold", 0.0f, 1.0f, 0.2f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fmAmt", 1}, "FM Amount", 0.0f, 1.0f, 0.1f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"envAmt", 1}, "Filter Env", 0.0f, 12000.0f, 3800.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"subLvl", 1}, "Sub Level", 0.0f, 1.0f, 0.7f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"noiseLvl", 1}, "Noise", 0.0f, 0.4f, 0.03f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"waveShape", 1}, "Wave Morph", 0.0f, 1.0f, 0.45f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"osc2Detune", 1}, "Osc2 Detune", -0.5f, 0.5f, 0.06f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"glide", 1}, "Glide", 0.0f, 0.5f, 0.04f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fAttack", 1}, "F Attack", 0.001f, 2.0f, 0.008f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fDecay", 1}, "F Decay", 0.02f, 3.0f, 0.32f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fSustain", 1}, "F Sustain", 0.0f, 1.0f, 0.2f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fRelease", 1}, "F Release", 0.01f, 3.0f, 0.2f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aAttack", 1}, "A Attack", 0.001f, 2.0f, 0.004f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aDecay", 1}, "A Decay", 0.02f, 3.0f, 0.35f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aSustain", 1}, "A Sustain", 0.0f, 1.0f, 0.82f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aRelease", 1}, "A Release", 0.01f, 3.0f, 0.22f));
  return layout;
}

bool MpcMessengerAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  const auto out = layouts.getMainOutputChannelSet();
  return out == juce::AudioChannelSet::mono() ||
         out == juce::AudioChannelSet::stereo();
}

void MpcMessengerAudioProcessor::prepareToPlay(double sampleRate,
                                             int samplesPerBlock) {
  sampleRate_ = sampleRate;
  juce::dsp::ProcessSpec spec{
      sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1};
  ladder_.prepare(spec);
  ladder_.reset();
  ampEnv_.setSampleRate(sampleRate);
  filterEnv_.setSampleRate(sampleRate);
  bassLpState_ = 0.0f;
}

float MpcMessengerAudioProcessor::processLadder(float in) {
  float sample[1] = {in};
  float *ch[1] = {sample};
  juce::dsp::AudioBlock<float> block(ch, 1, 1);
  juce::dsp::ProcessContextReplacing<float> ctx(block);
  ladder_.process(ctx);
  return block.getSample(0, 0);
}

int MpcMessengerAudioProcessor::getNumPrograms() {
  return static_cast<int>(messenger::kNumFactoryPrograms);
}

void MpcMessengerAudioProcessor::setCurrentProgram(int index) {
  index = juce::jlimit(0, getNumPrograms() - 1, index);
  currentProgram_ = index;
  applyFactoryProgram(index);
}

const juce::String MpcMessengerAudioProcessor::getProgramName(int index) {
  if (index >= 0 && index < getNumPrograms())
    return juce::String(
        messenger::kFactoryPrograms[static_cast<size_t>(index)].name);
  return {};
}

void MpcMessengerAudioProcessor::applyFactoryProgram(int index) {
  const auto &p = messenger::kFactoryPrograms[static_cast<size_t>(index)];
  setParam(apvts_, "cutoff", p.cutoff);
  setParam(apvts_, "res", p.res);
  setParam(apvts_, "resBass", p.resBass);
  setParam(apvts_, "fold", p.fold);
  setParam(apvts_, "fmAmt", p.fmAmt);
  setParam(apvts_, "envAmt", p.envAmt);
  setParam(apvts_, "subLvl", p.subLvl);
  setParam(apvts_, "noiseLvl", p.noiseLvl);
  setParam(apvts_, "waveShape", p.waveShape);
  setParam(apvts_, "osc2Detune", p.osc2Detune);
  setParam(apvts_, "glide", p.glide);
  setParam(apvts_, "fAttack", p.fAttack);
  setParam(apvts_, "fDecay", p.fDecay);
  setParam(apvts_, "fSustain", p.fSustain);
  setParam(apvts_, "fRelease", p.fRelease);
  setParam(apvts_, "aAttack", p.aAttack);
  setParam(apvts_, "aDecay", p.aDecay);
  setParam(apvts_, "aSustain", p.aSustain);
  setParam(apvts_, "aRelease", p.aRelease);
}

void MpcMessengerAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                              juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();

  for (const auto metadata : midi) {
    const auto msg = metadata.getMessage();
    if (msg.isNoteOn()) {
      heldNote_ = msg.getNoteNumber();
      targetFreq_ =
          static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(heldNote_));
      noteVelocity_ = msg.getFloatVelocity();
      if (!ampEnv_.isActive())
        currentFreq_ = targetFreq_;
      ampEnv_.noteOn();
      filterEnv_.noteOn();
    } else if (msg.isNoteOff() && msg.getNoteNumber() == heldNote_) {
      ampEnv_.noteOff();
      filterEnv_.noteOff();
      heldNote_ = -1;
    }
  }

  if (!ampEnv_.isActive())
    return;

  const float baseCutoff = apvts_.getRawParameterValue("cutoff")->load();
  const float res = apvts_.getRawParameterValue("res")->load();
  const float resBass = apvts_.getRawParameterValue("resBass")->load();
  const float fold = apvts_.getRawParameterValue("fold")->load();
  const float fmAmt = apvts_.getRawParameterValue("fmAmt")->load();
  const float envAmt = apvts_.getRawParameterValue("envAmt")->load();
  const float subLvl = apvts_.getRawParameterValue("subLvl")->load();
  const float noiseLvl = apvts_.getRawParameterValue("noiseLvl")->load();
  const float shape = apvts_.getRawParameterValue("waveShape")->load();
  const float detuneSemi = apvts_.getRawParameterValue("osc2Detune")->load();
  const float glideSec = apvts_.getRawParameterValue("glide")->load();

  filterEnv_.setParameters(
      {apvts_.getRawParameterValue("fAttack")->load(),
       apvts_.getRawParameterValue("fDecay")->load(),
       apvts_.getRawParameterValue("fSustain")->load(),
       apvts_.getRawParameterValue("fRelease")->load()});
  ampEnv_.setParameters(
      {apvts_.getRawParameterValue("aAttack")->load(),
       apvts_.getRawParameterValue("aDecay")->load(),
       apvts_.getRawParameterValue("aSustain")->load(),
       apvts_.getRawParameterValue("aRelease")->load()});

  ladder_.setResonance(res);
  ladder_.setDrive(1.0f + res * 1.5f);

  const float detuneRatio = std::pow(2.0f, detuneSemi / 12.0f);
  const float glideCoeff =
      (glideSec > 0.001f)
          ? std::exp(-1.0f / static_cast<float>(glideSec * sampleRate_))
          : 0.0f;

  const int n = buffer.getNumSamples();
  for (int i = 0; i < n; ++i) {
    currentFreq_ =
        targetFreq_ + glideCoeff * (currentFreq_ - targetFreq_);

    float inc2 = (currentFreq_ * detuneRatio) / static_cast<float>(sampleRate_);
    phase2_ += inc2;
    if (phase2_ >= 1.0f)
      phase2_ -= 1.0f;
    const float o2 = messenger::wavefold(messenger::morphWave(phase2_, shape), fold);

    // Audio-rate FM: osc2 into osc1 frequency
    float inc1 = currentFreq_ / static_cast<float>(sampleRate_);
    inc1 += o2 * fmAmt * inc1 * 4.0f;
    phase1_ += inc1;
    if (phase1_ >= 1.0f)
      phase1_ -= 1.0f;

    const float o1 =
        messenger::wavefold(messenger::morphWave(phase1_, shape), fold);

    subPhase_ += (currentFreq_ * 0.5f) / static_cast<float>(sampleRate_);
    if (subPhase_ >= 1.0f)
      subPhase_ -= 1.0f;
    const float sub = (subPhase_ < 0.5f ? 1.0f : -1.0f) * subLvl;
    const float noise = (noiseGen_.nextFloat() * 2.0f - 1.0f) * noiseLvl;

    const float dryMix =
        (o1 * 0.52f + o2 * 0.38f + sub * 0.55f + noise) * noteVelocity_;

    const float fEnv = filterEnv_.getNextSample();
    const float modCutoff =
        juce::jlimit(20.0f, 19000.0f, baseCutoff + fEnv * envAmt);
    ladder_.setCutoffFrequencyHz(modCutoff);

    const float filtered = processLadder(dryMix);
    const float out = messenger::resBassBlend(filtered, dryMix, res, resBass,
                                              bassLpState_);
    const float finalOut = out * ampEnv_.getNextSample() * 0.62f;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
      buffer.setSample(ch, i, finalOut);
  }
}

void MpcMessengerAudioProcessor::getStateInformation(juce::MemoryBlock &dest) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  xml->setAttribute("program", currentProgram_);
  copyXmlToBinary(*xml, dest);
}

void MpcMessengerAudioProcessor::setStateInformation(const void *data, int sz) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sz));
  if (xml && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    currentProgram_ = xml->getIntAttribute("program", 0);
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcMessengerAudioProcessor();
}