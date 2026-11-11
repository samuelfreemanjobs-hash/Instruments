#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Dsp/DrumPatternIO.h"

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
constexpr const char *kParamLoopBars = "mpf_loop_bars";
constexpr const char *kParamHalfTime = "mpf_half_time";
constexpr const char *kParamSyncHost = "mpf_sync_host";
constexpr const char *kParamDust = "mpf_dust";
constexpr const char *kParamTighten = "mpf_tighten";
constexpr const char *kParamExportStems = "mpf_export_stems";
constexpr const char *kParamExportMidi = "mpf_export_midi";
constexpr const char *kParamMpcManifest = "mpf_mpc_manifest";
constexpr const char *kParamBatchSweep = "mpf_batch_bpm_sweep";
constexpr const char *kParamBatchFrom = "mpf_batch_bpm_from";
constexpr const char *kParamBatchTo = "mpf_batch_bpm_to";
} // namespace

MemphisPhonkFactoryAudioProcessor::MemphisPhonkFactoryAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      engine_(44100.0),
      patternA_(DrumPattern::memphisClassic()),
      patternB_(DrumPattern::trapSparse()) {}

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
      kParamDust, "Dust", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.35f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamDrive, "Tape Drive", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamHiss, "Hiss", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.35f));
  params.push_back(std::make_unique<juce::AudioParameterInt>(kParamVariation, "Variation", 1, 4, 1));
  params.push_back(std::make_unique<juce::AudioParameterChoice>(
      kParamLoopBars, "Loop bars", juce::StringArray{"1", "2", "4"}, 1)); // default index 1 = 2 bars
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamHalfTime, "Half-time preview", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamSyncHost, "Sync host BPM", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamTighten, "Tighten kit to loop", true));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>(kParamPreview, "Loop Preview", true));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamMaster, "Master", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.85f));
  params.push_back(
      std::make_unique<juce::AudioParameterInt>(kParamBatchCount, "Batch kits", 1, 16, 4));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamClassicLoops,
                                                              "Classic loop tempos", true));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamExportStems, "Export stems", true));
  params.push_back(std::make_unique<juce::AudioParameterBool>(kParamExportMidi, "Export pattern MIDI", true));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>(kParamMpcManifest, "MPC manifest", true));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>(kParamBatchSweep, "Batch BPM sweep", false));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamBatchFrom, "Batch BPM from", juce::NormalisableRange<float>(70.0f, 170.0f, 1.0f), 110.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      kParamBatchTo, "Batch BPM to", juce::NormalisableRange<float>(70.0f, 170.0f, 1.0f), 130.0f));
  return {params.begin(), params.end()};
}

DrumPattern MemphisPhonkFactoryAudioProcessor::getPattern() const {
  return usePatternB_ ? patternB_ : patternA_;
}

void MemphisPhonkFactoryAudioProcessor::setPattern(const DrumPattern &pattern) {
  if (usePatternB_) {
    patternB_ = pattern;
  } else {
    patternA_ = pattern;
  }
  schedulePreviewRefresh();
}

void MemphisPhonkFactoryAudioProcessor::togglePatternAB() {
  usePatternB_ = !usePatternB_;
  schedulePreviewRefresh();
}

void MemphisPhonkFactoryAudioProcessor::shiftPattern(int stepsRight) {
  auto p = getPattern();
  DrumPatternIO::shiftRight(p, stepsRight);
  setPattern(p);
}

bool MemphisPhonkFactoryAudioProcessor::savePatternToFile(const juce::File &f) const {
  return f.replaceWithText(DrumPatternIO::toJson(getPattern()));
}

bool MemphisPhonkFactoryAudioProcessor::loadPatternFromFile(const juce::File &f) {
  const auto p = DrumPatternIO::fromJson(f.loadFileAsString());
  setPattern(p);
  return true;
}

float MemphisPhonkFactoryAudioProcessor::getEffectiveBpm() const {
  if (apvts_.getRawParameterValue(kParamSyncHost)->load() > 0.5f) {
    const float host = hostBpm_.load();
    if (host > 20.0f) {
      return host;
    }
  }
  return apvts_.getRawParameterValue(kParamLoopBpm)->load();
}

float MemphisPhonkFactoryAudioProcessor::getLoopDurationSec() const {
  const int barsIdx = static_cast<int>(apvts_.getRawParameterValue(kParamLoopBars)->load());
  const int bars = barsIdx == 0 ? 1 : (barsIdx == 1 ? 2 : 4);
  return PackExportOptions::loopDurationSec(getEffectiveBpm(), bars);
}

void MemphisPhonkFactoryAudioProcessor::applyEngineSettingsFromParams() {
  const int variation = static_cast<int>(apvts_.getRawParameterValue(kParamVariation)->load());
  auto s = MemphisPhonkEngine::settingsFromDust(apvts_.getRawParameterValue(kParamDust)->load(),
                                                variation);
  s.tightenToLoop = apvts_.getRawParameterValue(kParamTighten)->load() > 0.5f;
  s.tightenBpm = getEffectiveBpm();
  engine_.setSettings(s);
}

PackExportOptions MemphisPhonkFactoryAudioProcessor::buildPackOptions() const {
  PackExportOptions o;
  o.primaryBpm = getEffectiveBpm();
  o.swing = apvts_.getRawParameterValue(kParamSwing)->load();
  const int barsIdx = static_cast<int>(apvts_.getRawParameterValue(kParamLoopBars)->load());
  o.bars = barsIdx == 0 ? 1 : (barsIdx == 1 ? 2 : 4);
  o.pattern = getPattern();
  o.includeClassicTempos = apvts_.getRawParameterValue(kParamClassicLoops)->load() > 0.5f;
  o.exportStems = apvts_.getRawParameterValue(kParamExportStems)->load() > 0.5f;
  o.exportMidi = apvts_.getRawParameterValue(kParamExportMidi)->load() > 0.5f;
  o.writeMpcManifest = apvts_.getRawParameterValue(kParamMpcManifest)->load() > 0.5f;
  o.batchBpmSweep = apvts_.getRawParameterValue(kParamBatchSweep)->load() > 0.5f;
  o.batchBpmFrom = apvts_.getRawParameterValue(kParamBatchFrom)->load();
  o.batchBpmTo = apvts_.getRawParameterValue(kParamBatchTo)->load();
  return o;
}

MemphisPhonkEngine::Settings MemphisPhonkFactoryAudioProcessor::buildEngineSettings() const {
  const int variation = static_cast<int>(apvts_.getRawParameterValue(kParamVariation)->load());
  auto s = MemphisPhonkEngine::settingsFromDust(apvts_.getRawParameterValue(kParamDust)->load(),
                                                variation);
  s.tightenToLoop = apvts_.getRawParameterValue(kParamTighten)->load() > 0.5f;
  s.tightenBpm = getEffectiveBpm();
  return s;
}

void MemphisPhonkFactoryAudioProcessor::schedulePreviewRefresh() {
  previewRefreshPending_ = true;
  startTimer(180);
}

void MemphisPhonkFactoryAudioProcessor::timerCallback() {
  stopTimer();
  if (previewRefreshPending_) {
    previewRefreshPending_ = false;
    refreshPreviewLoop();
  }
}

void MemphisPhonkFactoryAudioProcessor::refreshPreviewLoop() {
  applyEngineSettingsFromParams();
  const auto opts = buildPackOptions();
  auto loop = std::make_shared<std::vector<float>>(
      engine_.generateDrumLoop(opts.primaryBpm, opts.bars, opts.swing, opts.pattern));
  previewLoop_ = loop;
  playhead_.store(0);
}

void MemphisPhonkFactoryAudioProcessor::startBackgroundExport(
    const juce::File &parent, bool batch, std::function<void(juce::String)> onComplete,
    std::function<void(float)> onProgress) {
  ExportJob job;
  job.batch = batch;
  job.batchCount = static_cast<int>(apvts_.getRawParameterValue(kParamBatchCount)->load());
  job.parentDir = parent;
  job.options = buildPackOptions();
  job.engineSettings = buildEngineSettings();
  job.sampleRate = engine_.getSampleRate();
  exportWorker_.startJob(job, std::move(onComplete), std::move(onProgress));
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

  if (auto *head = getPlayHead()) {
    if (const auto pos = head->getPosition()) {
      if (const auto bpm = pos->getBpm()) {
        hostBpm_.store(static_cast<float>(*bpm));
      }
    }
  }

  const bool preview = apvts_.getRawParameterValue(kParamPreview)->load() > 0.5f;
  if (!preview) {
    return;
  }

  const auto loop = previewLoop_;
  if (!loop || loop->empty()) {
    return;
  }

  const float master = apvts_.getRawParameterValue(kParamMaster)->load();
  const bool half = apvts_.getRawParameterValue(kParamHalfTime)->load() > 0.5f;
  const float rate = half ? 0.5f : 1.0f;
  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  const int loopLen = static_cast<int>(loop->size());

  double pos = static_cast<double>(playhead_.load());
  for (int i = 0; i < numSamples; ++i) {
    const int idx = static_cast<int>(pos) % loopLen;
    const float sample = (*loop)[static_cast<size_t>(idx)] * master;
    for (int ch = 0; ch < numChannels; ++ch) {
      buffer.setSample(ch, i, sample);
    }
    pos += rate;
  }
  playhead_.store(static_cast<int64_t>(pos));
}

juce::AudioProcessorEditor *MemphisPhonkFactoryAudioProcessor::createEditor() {
  return new MemphisPhonkFactoryAudioProcessorEditor(*this);
}

void MemphisPhonkFactoryAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts_.copyState();
  state.setProperty("kickMaskA", static_cast<int>(patternA_.kick), nullptr);
  state.setProperty("snareMaskA", static_cast<int>(patternA_.snare), nullptr);
  state.setProperty("openHatMaskA", static_cast<int>(patternA_.openHat), nullptr);
  state.setProperty("kickMaskB", static_cast<int>(patternB_.kick), nullptr);
  state.setProperty("snareMaskB", static_cast<int>(patternB_.snare), nullptr);
  state.setProperty("openHatMaskB", static_cast<int>(patternB_.openHat), nullptr);
  state.setProperty("usePatternB", usePatternB_, nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void MemphisPhonkFactoryAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr && xml->hasTagName(apvts_.state.getType())) {
    auto state = juce::ValueTree::fromXml(*xml);
    apvts_.replaceState(state);
    patternA_.kick = static_cast<uint16_t>(static_cast<int>(state.getProperty("kickMaskA", 0)));
    patternA_.snare = static_cast<uint16_t>(static_cast<int>(state.getProperty("snareMaskA", 0)));
    patternA_.openHat =
        static_cast<uint16_t>(static_cast<int>(state.getProperty("openHatMaskA", 0)));
    patternB_.kick = static_cast<uint16_t>(static_cast<int>(state.getProperty("kickMaskB", 0)));
    patternB_.snare = static_cast<uint16_t>(static_cast<int>(state.getProperty("snareMaskB", 0)));
    patternB_.openHat =
        static_cast<uint16_t>(static_cast<int>(state.getProperty("openHatMaskB", 0)));
    usePatternB_ = static_cast<bool>(state.getProperty("usePatternB", false));
    if (patternA_.kick == 0 && patternA_.snare == 0 && patternA_.openHat == 0) {
      patternA_ = DrumPattern::memphisClassic();
    }
    refreshPreviewLoop();
  }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MemphisPhonkFactoryAudioProcessor();
}
