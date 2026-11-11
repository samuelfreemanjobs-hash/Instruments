#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
constexpr const char *kParamLoopBpm = "mpf_loop_bpm";
constexpr const char *kParamSwing = "mpf_swing";
constexpr const char *kParamDrive = "mpf_drive";
constexpr const char *kParamHiss = "mpf_hiss";
constexpr const char *kParamVariation = "mpf_variation";
constexpr const char *kParamPreview = "mpf_preview";
constexpr const char *kParamMaster = "mpf_master";
constexpr const char *kParamBatchCount = "mpf_batch_count";
constexpr const char *kParamClassicLoops = "mpf_classic_loops";
} // namespace

MemphisPhonkFactoryAudioProcessor::MemphisPhonkFactoryAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      engine_(44100.0),
      pattern_(DrumPattern::memphisClassic()) {}

MemphisPhonkFactoryAudioProcessor::~MemphisPhonkFactoryAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
MemphisPhonkFactoryAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  juce::NormalisableRange<float> bpmRange(70.0f, 170.0f, 0.1f);
  bpmRange.setSkewForCentre(120.0f);
  params.push_back(std::make_unique<juce::AudioParameterFloat>(kParamLoopBpm, "Loop BPM", bpmRange,
                                                               118.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamSwing, "Swing", juce::NormalisableRange<float>(0.0f, 0.08f, 0.001f), 0.03f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamDrive, "Tape Drive", juce::NormalisableRange<float>(1.5f, 4.5f, 0.01f), 2.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamHiss, "Hiss", juce::NormalisableRange<float>(0.0f, 0.02f, 0.0001f), 0.004f));
  params.push_back(std::make_unique<juce::AudioParameterInt>(kParamVariation, "Variation", 1, 4, 1));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>(kParamPreview, "Loop Preview", true));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamMaster, "Master", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.85f));
  params.push_back(
      std::make_unique<juce::AudioParameterInt>(kParamBatchCount, "Batch kits", 1, 16, 4));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamClassicLoops,
                                                              "Classic loop tempos (112/118/124)",
                                                              true));
  return {params.begin(), params.end()};
}

DrumPattern MemphisPhonkFactoryAudioProcessor::getPattern() const { return pattern_; }

void MemphisPhonkFactoryAudioProcessor::setPattern(const DrumPattern &pattern) {
  pattern_ = pattern;
  refreshPreviewLoop();
}

void MemphisPhonkFactoryAudioProcessor::applyEngineSettingsFromParams() {
  MemphisPhonkEngine::Settings s;
  s.drive = apvts_.getRawParameterValue(kParamDrive)->load();
  s.hissLevel = apvts_.getRawParameterValue(kParamHiss)->load();
  s.bitDepth = 12;
  const int variation = static_cast<int>(apvts_.getRawParameterValue(kParamVariation)->load());
  s.seed = 1000 + variation * 7919;
  engine_.setSettings(s);
}

void MemphisPhonkFactoryAudioProcessor::refreshPreviewLoop() {
  applyEngineSettingsFromParams();
  const float bpm = apvts_.getRawParameterValue(kParamLoopBpm)->load();
  const float swing = apvts_.getRawParameterValue(kParamSwing)->load();
  auto loop =
      std::make_shared<std::vector<float>>(engine_.generateDrumLoop(bpm, 2, swing, pattern_));
  previewLoop_ = loop;
  playhead_.store(0);
}

juce::String MemphisPhonkFactoryAudioProcessor::exportKitToFolder(const juce::File &parentDir) {
  applyEngineSettingsFromParams();
  const float bpm = apvts_.getRawParameterValue(kParamLoopBpm)->load();
  const float swing = apvts_.getRawParameterValue(kParamSwing)->load();
  const bool classic = apvts_.getRawParameterValue(kParamClassicLoops)->load() > 0.5f;
  return engine_.exportMpcKit(parentDir, bpm, swing, pattern_, classic);
}

juce::String MemphisPhonkFactoryAudioProcessor::exportBatchToFolder(const juce::File &parentDir) {
  applyEngineSettingsFromParams();
  const float bpm = apvts_.getRawParameterValue(kParamLoopBpm)->load();
  const float swing = apvts_.getRawParameterValue(kParamSwing)->load();
  const int count = static_cast<int>(apvts_.getRawParameterValue(kParamBatchCount)->load());
  const bool classic = apvts_.getRawParameterValue(kParamClassicLoops)->load() > 0.5f;
  return engine_.exportBatchKits(parentDir, count, bpm, swing, pattern_, classic);
}

void MemphisPhonkFactoryAudioProcessor::prepareToPlay(double sampleRate, int) {
  engine_.setSampleRate(sampleRate);
  refreshPreviewLoop();
}

void MemphisPhonkFactoryAudioProcessor::releaseResources() {}

bool MemphisPhonkFactoryAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  const auto &out = layouts.getMainOutputChannelSet();
  return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void MemphisPhonkFactoryAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                     juce::MidiBuffer &) {
  juce::ScopedNoDenormals noDenormals;
  buffer.clear();

  const bool preview = apvts_.getRawParameterValue(kParamPreview)->load() > 0.5f;
  if (!preview) {
    return;
  }

  const auto loop = previewLoop_;
  if (!loop || loop->empty()) {
    return;
  }

  const float master = apvts_.getRawParameterValue(kParamMaster)->load();
  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  const int loopLen = static_cast<int>(loop->size());

  int64_t pos = playhead_.load();
  for (int i = 0; i < numSamples; ++i) {
    const float sample = (*loop)[static_cast<size_t>(pos % loopLen)] * master;
    for (int ch = 0; ch < numChannels; ++ch) {
      buffer.setSample(ch, i, sample);
    }
    ++pos;
  }
  playhead_.store(pos);
}

juce::AudioProcessorEditor *MemphisPhonkFactoryAudioProcessor::createEditor() {
  return new MemphisPhonkFactoryAudioProcessorEditor(*this);
}

void MemphisPhonkFactoryAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts_.copyState();
  state.setProperty("kickMask", static_cast<int>(pattern_.kick), nullptr);
  state.setProperty("snareMask", static_cast<int>(pattern_.snare), nullptr);
  state.setProperty("openHatMask", static_cast<int>(pattern_.openHat), nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void MemphisPhonkFactoryAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr && xml->hasTagName(apvts_.state.getType())) {
    auto state = juce::ValueTree::fromXml(*xml);
    apvts_.replaceState(state);
    pattern_.kick = static_cast<uint16_t>(static_cast<int>(state.getProperty("kickMask", 0)));
    pattern_.snare = static_cast<uint16_t>(static_cast<int>(state.getProperty("snareMask", 0)));
    pattern_.openHat =
        static_cast<uint16_t>(static_cast<int>(state.getProperty("openHatMask", 0)));
    if (pattern_.kick == 0 && pattern_.snare == 0 && pattern_.openHat == 0) {
      pattern_ = DrumPattern::memphisClassic();
    }
    refreshPreviewLoop();
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MemphisPhonkFactoryAudioProcessor();
}
