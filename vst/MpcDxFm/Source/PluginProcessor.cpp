#include "PluginProcessor.h"
#include "FactoryPrograms.h"

namespace {
void setParam(juce::AudioProcessorValueTreeState &apvts, const char *id, float v) {
  if (auto *p = apvts.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
} // namespace

MpcDxFmAudioProcessor::MpcDxFmAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "DXFM", createParameterLayout()) {
  applyFactoryProgram(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
MpcDxFmAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"brightness", 1}, "Brightness", 0.0f, 1.0f, 0.55f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"feedback", 1}, "Feedback", 0.0f, 1.0f, 0.12f));
  layout.add(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID{"algo", 1}, "Algorithm", 0, 7, 3));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op1_level", 1}, "Op1 Level", 0.0f, 1.0f, 1.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op2_level", 1}, "Op2 Level", 0.0f, 1.0f, 0.7f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op3_level", 1}, "Op3 Level", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op4_level", 1}, "Op4 Level", 0.0f, 1.0f, 0.6f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op1_ratio", 1}, "Op1 Ratio", 0.5f, 16.0f, 1.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op2_ratio", 1}, "Op2 Ratio", 0.5f, 16.0f, 2.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op3_ratio", 1}, "Op3 Ratio", 0.5f, 16.0f, 1.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"op4_ratio", 1}, "Op4 Ratio", 0.5f, 16.0f, 3.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"attack", 1}, "Attack", 0.001f, 3.0f, 0.01f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"decay", 1}, "Decay", 0.02f, 3.0f, 0.4f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"sustain", 1}, "Sustain", 0.0f, 1.0f, 0.75f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"release", 1}, "Release", 0.01f, 4.0f, 0.3f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"lfo_rate", 1}, "LFO Rate", 0.0f, 12.0f, 0.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"lfo_depth", 1}, "LFO Depth", 0.0f, 1.0f, 0.0f));
  return layout;
}

bool MpcDxFmAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  const auto out = layouts.getMainOutputChannelSet();
  return out == juce::AudioChannelSet::mono() ||
         out == juce::AudioChannelSet::stereo();
}

void MpcDxFmAudioProcessor::prepareToPlay(double sampleRate,
                                          int /*samplesPerBlock*/) {
  sampleRate_ = sampleRate;
  lfoPhase_ = 0.0f;
}

int MpcDxFmAudioProcessor::findFreeVoice() {
  for (int i = 0; i < kMaxVoices; ++i)
    if (!voices_[static_cast<size_t>(i)].active)
      return i;
  return 0;
}

int MpcDxFmAudioProcessor::findVoiceForNote(int note) {
  for (int i = 0; i < kMaxVoices; ++i)
    if (voices_[static_cast<size_t>(i)].active &&
        voices_[static_cast<size_t>(i)].note == note)
      return i;
  return -1;
}

void MpcDxFmAudioProcessor::syncVoiceParams(dxfm::VoiceParams &vp) const {
  vp.algo = static_cast<int>(*apvts_.getRawParameterValue("algo"));
  vp.feedback = apvts_.getRawParameterValue("feedback")->load();
  vp.opLevel[0] = apvts_.getRawParameterValue("op1_level")->load();
  vp.opLevel[1] = apvts_.getRawParameterValue("brightness")->load();
  vp.opLevel[2] = apvts_.getRawParameterValue("op3_level")->load();
  vp.opLevel[3] = apvts_.getRawParameterValue("op4_level")->load();
  vp.opRatio[0] = apvts_.getRawParameterValue("op1_ratio")->load();
  vp.opRatio[1] = apvts_.getRawParameterValue("op2_ratio")->load();
  vp.opRatio[2] = apvts_.getRawParameterValue("op3_ratio")->load();
  vp.opRatio[3] = apvts_.getRawParameterValue("op4_ratio")->load();
}

int MpcDxFmAudioProcessor::getNumPrograms() {
  return static_cast<int>(dxfm::kNumFactoryPrograms);
}

void MpcDxFmAudioProcessor::setCurrentProgram(int index) {
  index = juce::jlimit(0, getNumPrograms() - 1, index);
  currentProgram_ = index;
  applyFactoryProgram(index);
}

const juce::String MpcDxFmAudioProcessor::getProgramName(int index) {
  if (index >= 0 && index < getNumPrograms())
    return juce::String(
        dxfm::kFactoryPrograms[static_cast<size_t>(index)].name);
  return {};
}

void MpcDxFmAudioProcessor::applyFactoryProgram(int index) {
  const auto &p = dxfm::kFactoryPrograms[static_cast<size_t>(index)];
  setParam(apvts_, "brightness", p.brightness);
  setParam(apvts_, "feedback", p.feedback);
  setParam(apvts_, "algo", static_cast<float>(p.algo));
  setParam(apvts_, "op1_level", p.op1_level);
  setParam(apvts_, "op2_level", p.op2_level);
  setParam(apvts_, "op3_level", p.op3_level);
  setParam(apvts_, "op4_level", p.op4_level);
  setParam(apvts_, "op1_ratio", p.op1_ratio);
  setParam(apvts_, "op2_ratio", p.op2_ratio);
  setParam(apvts_, "op3_ratio", p.op3_ratio);
  setParam(apvts_, "op4_ratio", p.op4_ratio);
  setParam(apvts_, "attack", p.attack);
  setParam(apvts_, "decay", p.decay);
  setParam(apvts_, "sustain", p.sustain);
  setParam(apvts_, "release", p.release);
}

void MpcDxFmAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                         juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();

  const float attack = apvts_.getRawParameterValue("attack")->load();
  const float decay = apvts_.getRawParameterValue("decay")->load();
  const float sustain = apvts_.getRawParameterValue("sustain")->load();
  const float release = apvts_.getRawParameterValue("release")->load();
  const float lfoRate = apvts_.getRawParameterValue("lfo_rate")->load();
  const float lfoDepth = apvts_.getRawParameterValue("lfo_depth")->load();

  dxfm::VoiceParams vp{};
  syncVoiceParams(vp);

  for (const auto metadata : midi) {
    const auto msg = metadata.getMessage();
    if (msg.isNoteOn()) {
      int slot = findVoiceForNote(msg.getNoteNumber());
      if (slot < 0)
        slot = findFreeVoice();
      voices_[static_cast<size_t>(slot)].startNote(
          msg.getNoteNumber(), msg.getFloatVelocity(), sampleRate_, attack,
          decay, sustain, release, vp);
    } else if (msg.isNoteOff()) {
      const int slot = findVoiceForNote(msg.getNoteNumber());
      if (slot >= 0)
        voices_[static_cast<size_t>(slot)].noteOff();
    }
  }

  const int n = buffer.getNumSamples();
  const float lfoInc = lfoRate / static_cast<float>(sampleRate_);

  for (int i = 0; i < n; ++i) {
    if (lfoRate > 0.01f) {
      lfoPhase_ += lfoInc;
      if (lfoPhase_ > 1.0f)
        lfoPhase_ -= 1.0f;
    }
    const float lfoMod =
        std::sin(lfoPhase_ * 6.28318530718f) * lfoDepth * 0.02f;

    float sum = 0.0f;
    for (auto &v : voices_) {
      if (v.active) {
        v.params = vp;
        sum += v.renderSample(sampleRate_, lfoMod);
      }
    }

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
      buffer.setSample(ch, i, sum);
  }
}

void MpcDxFmAudioProcessor::getStateInformation(juce::MemoryBlock &dest) {
  auto state = apvts_.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  xml->setAttribute("program", currentProgram_);
  copyXmlToBinary(*xml, dest);
}

void MpcDxFmAudioProcessor::setStateInformation(const void *data, int sz) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sz));
  if (xml && xml->hasTagName(apvts_.state.getType())) {
    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    currentProgram_ = xml->getIntAttribute("program", 0);
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MpcDxFmAudioProcessor();
}