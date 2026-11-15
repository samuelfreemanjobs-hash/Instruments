#include "PluginProcessor.h"
#include "FactoryPrograms.h"
#include "SubPhattyDsp.h"

namespace {
void setParam(juce::AudioProcessorValueTreeState &apvts, const char *id,
              float value) {
  if (auto *p = apvts.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(value));
}

void setBoolParam(juce::AudioProcessorValueTreeState &apvts, const char *id,
                  bool value) {
  if (auto *p = apvts.getParameter(id))
    p->setValueNotifyingHost(value ? 1.0f : 0.0f);
}
} // namespace

MpcSubPhattyAudioProcessor::MpcSubPhattyAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "SUBPHATTY", createParameterLayout()) {
  ladderFilter_.setMode(juce::dsp::LadderFilterMode::LPF24);
  applyFactoryProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcSubPhattyAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"cutoff", 1}, "Cutoff",
      juce::NormalisableRange<float>(20.0f, 18000.0f, 1.0f, 0.28f), 420.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"res", 1}, "Resonance", 0.0f, 0.96f, 0.42f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"multidrive", 1}, "Multidrive", 0.0f, 1.0f, 0.45f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"envAmt", 1}, "Filter Env Amt", 0.0f, 14000.0f, 4500.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"subLvl", 1}, "Sub Osc Level", 0.0f, 1.0f, 0.75f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"waveShape", 1}, "Osc Wave Morph", 0.0f, 1.0f, 0.35f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"osc2Detune", 1}, "Osc 2 Detune", -0.5f, 0.5f, 0.09f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"glide", 1}, "Glide Time", 0.0f, 0.5f, 0.03f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"noiseLvl", 1}, "Noise Level", 0.0f, 0.4f, 0.02f));
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{"slope12dB", 1}, "12dB Filter Mode", false));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fAttack", 1}, "Filter Attack", 0.001f, 2.0f, 0.005f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fDecay", 1}, "Filter Decay", 0.02f, 3.0f, 0.28f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fSustain", 1}, "Filter Sustain", 0.0f, 1.0f, 0.15f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"fRelease", 1}, "Filter Release", 0.01f, 3.0f, 0.14f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aAttack", 1}, "Amp Attack", 0.001f, 2.0f, 0.004f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aDecay", 1}, "Amp Decay", 0.02f, 3.0f, 0.35f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aSustain", 1}, "Amp Sustain", 0.0f, 1.0f, 0.85f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"aRelease", 1}, "Amp Release", 0.01f, 3.0f, 0.14f));
  return layout;
}

bool MpcSubPhattyAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  const auto out = layouts.getMainOutputChannelSet();
  return out == juce::AudioChannelSet::mono() ||
         out == juce::AudioChannelSet::stereo();
}

void MpcSubPhattyAudioProcessor::prepareToPlay(double sampleRate,
                                               int samplesPerBlock) {
  sampleRate_ = sampleRate;
  juce::dsp::ProcessSpec spec{
      sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1};
  ladderFilter_.prepare(spec);
  ladderFilter_.reset();
  ampEnv_.setSampleRate(sampleRate);
  filterEnv_.setSampleRate(sampleRate);
}

void MpcSubPhattyAudioProcessor::processLadderSample(float mixerIn, float &out) {
  float sample[1] = {mixerIn};
  float *chans[1] = {sample};
  juce::dsp::AudioBlock<float> block(chans, 1, 1);
  juce::dsp::ProcessContextReplacing<float> ctx(block);
  ladderFilter_.process(ctx);
  out = block.getSample(0, 0);
}

int MpcSubPhattyAudioProcessor::getNumPrograms() {
  return static_cast<int>(subphatty::kNumFactoryPrograms);
}

void MpcSubPhattyAudioProcessor::setCurrentProgram(int index) {
  index = juce::jlimit(0, getNumPrograms() - 1, index);
  currentProgram_ = index;
  applyFactoryProgram(index);
}

const juce::String MpcSubPhattyAudioProcessor::getProgramName(int index) {
  if (index >= 0 && index < getNumPrograms())
    return juce::String(
        subphatty::kFactoryPrograms[static_cast<size_t>(index)].name);
  return {};
}

void MpcSubPhattyAudioProcessor::applyFactoryProgram(int index) {
  const auto &p = subphatty::kFactoryPrograms[static_cast<size_t>(index)];
  setParam(apvts_, "cutoff", p.cutoff);
  setParam(apvts_, "res", p.res);
  setParam(apvts_, "multidrive", p.multidrive);
  setParam(apvts_, "envAmt", p.envAmt);
  setParam(apvts_, "subLvl", p.subLvl);
  setParam(apvts_, "waveShape", p.waveShape);
  setParam(apvts_, "osc2Detune", p.osc2Detune);
  setParam(apvts_, "glide", p.glide);
  setParam(apvts_, "noiseLvl", p.noiseLvl);
  setBoolParam(apvts_, "slope12dB", p.slope12dB);
  setParam(apvts_, "fAttack", p.fAttack);
  setParam(apvts_, "fDecay", p.fDecay);
  setParam(apvts_, "fSustain", p.fSustain);
  setParam(apvts_, "fRelease", p.fRelease);
  setParam(apvts_, "aAttack", p.aAttack);
  setParam(apvts_, "aDecay", p.aDecay);
  setParam(apvts_, "aSustain", p.aSustain);
  setParam(apvts_, "aRelease", p.aRelease);
}

void MpcSubPhattyAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
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
  const float drive = apvts_.getRawParameterValue("multidrive")->load();
  const float envAmt = apvts_.getRawParameterValue("envAmt")->load();
  const float subLvl = apvts_.getRawParameterValue("subLvl")->load();
  const float shape = apvts_.getRawParameterValue("waveShape")->load();
  const float detuneSemi = apvts_.getRawParameterValue("osc2Detune")->load();
  const float glideSec = apvts_.getRawParameterValue("glide")->load();
  const float noiseLvl = apvts_.getRawParameterValue("noiseLvl")->load();
  const bool is12dB = apvts_.getRawParameterValue("slope12dB")->load() > 0.5f;

  ladderFilter_.setMode(is12dB ? juce::dsp::LadderFilterMode::LPF12
                               : juce::dsp::LadderFilterMode::LPF24);
  ladderFilter_.setResonance(res);
  ladderFilter_.setDrive(1.0f + drive * 3.5f);

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

  const float detuneRatio = std::pow(2.0f, detuneSemi / 12.0f);
  const float glideCoeff =
      (glideSec > 0.001f)
          ? std::exp(-1.0f / static_cast<float>(glideSec * sampleRate_))
          : 0.0f;

  const float preDrive = 1.0f + drive * 2.0f;
  const int numSamples = buffer.getNumSamples();

  for (int i = 0; i < numSamples; ++i) {
    currentFreq_ =
        targetFreq_ + glideCoeff * (currentFreq_ - targetFreq_);

    const float inc1 = currentFreq_ / static_cast<float>(sampleRate_);
    const float inc2 =
        (currentFreq_ * detuneRatio) / static_cast<float>(sampleRate_);
    const float incSub =
        (currentFreq_ * 0.5f) / static_cast<float>(sampleRate_);

    phase1_ += inc1;
    phase2_ += inc2;
    subPhase_ += incSub;
    if (phase1_ >= 1.0f) phase1_ -= 1.0f;
    if (phase2_ >= 1.0f) phase2_ -= 1.0f;
    if (subPhase_ >= 1.0f) subPhase_ -= 1.0f;

    const float o1 = subphatty::morphWave(phase1_, shape);
    const float o2 = subphatty::morphWave(phase2_, shape);
    const float sub = (subPhase_ < 0.5f ? 1.0f : -1.0f) * subLvl;
    const float noise = (noiseGen_.nextFloat() * 2.0f - 1.0f) * noiseLvl;

    float mixerOut =
        (o1 * 0.42f + o2 * 0.42f + sub * 0.55f + noise) * noteVelocity_;
    mixerOut = subphatty::applyMultidrive(mixerOut, drive * 0.35f * preDrive);

    const float fEnv = filterEnv_.getNextSample();
    const float modCutoff =
        juce::jlimit(20.0f, 19000.0f, baseCutoff + (fEnv * envAmt));
    ladderFilter_.setCutoffFrequencyHz(modCutoff);

    float filtered = 0.0f;
    processLadderSample(mixerOut, filtered);
    filtered = subphatty::applyMultidrive(filtered, drive);
    const float finalOut = filtered * ampEnv_.getNextSample() * 0.65f;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
      buffer.setSample(ch, i, finalOut);
  }
}

void MpcSubPhattyAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  xml->setAttribute("program", currentProgram_);
  copyXmlToBinary(*xml, destData);
}

void MpcSubPhattyAudioProcessor::setStateInformation(const void *data,
                                                     int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    currentProgram_ = xml->getIntAttribute("program", 0);
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcSubPhattyAudioProcessor();
}