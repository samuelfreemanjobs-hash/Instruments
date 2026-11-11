#include "PluginEditor.h"

namespace {
constexpr const char *kParamLoopBpm = "mpf_loop_bpm";
constexpr const char *kParamSwing = "mpf_swing";
constexpr const char *kParamDust = "mpf_dust";
constexpr const char *kParamVariation = "mpf_variation";
constexpr const char *kParamPreview = "mpf_preview";
constexpr const char *kParamMaster = "mpf_master";
constexpr const char *kParamBatchCount = "mpf_batch_count";
constexpr const char *kParamClassicLoops = "mpf_classic_loops";
constexpr const char *kParamLoopBars = "mpf_loop_bars";
constexpr const char *kParamHalfTime = "mpf_half_time";
constexpr const char *kParamSyncHost = "mpf_sync_host";
constexpr const char *kParamTighten = "mpf_tighten";
constexpr const char *kParamExportStems = "mpf_export_stems";
constexpr const char *kParamExportMidi = "mpf_export_midi";
constexpr const char *kParamMpcManifest = "mpf_mpc_manifest";
constexpr const char *kParamBatchSweep = "mpf_batch_bpm_sweep";
constexpr const char *kParamBatchFrom = "mpf_batch_bpm_from";
constexpr const char *kParamBatchTo = "mpf_batch_bpm_to";

void styleHSlider(juce::Slider &s) {
  s.setSliderStyle(juce::Slider::LinearHorizontal);
  s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 52, 18);
}
} // namespace

MemphisPhonkFactoryAudioProcessorEditor::MemphisPhonkFactoryAudioProcessorEditor(
    MemphisPhonkFactoryAudioProcessor &p)
    : AudioProcessorEditor(&p), processor_(p) {
  setSize(920, 620);

  patternGrid_.setPattern(processor_.getPattern());
  patternGrid_.onPatternChanged = [this](const DrumPattern &pat) { processor_.setPattern(pat); };
  wirePatternTools();
  addAndMakeVisible(patternGrid_);

  bpmSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  bpmSlider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  bpmSlider_.setRange(70.0, 170.0, 0.1);
  bpmSlider_.setSkewFactorFromMidPoint(120.0);
  bpmValueLabel_.setJustificationType(juce::Justification::centred);
  bpmValueLabel_.setFont(juce::Font(18.0f, juce::Font::bold));
  loopInfoLabel_.setFont(juce::Font(12.0f));
  loopInfoLabel_.setJustificationType(juce::Justification::centredLeft);

  styleHSlider(swingSlider_);
  styleHSlider(dustSlider_);
  styleHSlider(masterSlider_);
  variationSlider_.setRange(1, 4, 1);
  batchCountSlider_.setRange(1, 16, 1);
  styleHSlider(variationSlider_);
  styleHSlider(batchCountSlider_);
  styleHSlider(batchFromSlider_);
  styleHSlider(batchToSlider_);

  const std::array<juce::Component *, 24> comps = {
      &bpmSlider_,        &bpmDown_,           &bpmUp_,         &tapTempo_,    &bpmValueLabel_,
      &loopInfoLabel_,    &swingSlider_,       &dustSlider_,    &masterSlider_, &variationSlider_,
      &batchCountSlider_, &batchFromSlider_,   &batchToSlider_, &barsBox_,     &previewButton_,
      &halfTimeButton_,   &syncHostButton_,    &tightenButton_, &classicLoopsButton_, &stemsButton_,
      &midiButton_,       &manifestButton_,    &batchSweepButton_, &exportButton_};
  for (auto *c : comps) {
    addAndMakeVisible(c);
  }
  addAndMakeVisible(batchExportButton_);
  addAndMakeVisible(statusLabel_);
  addAndMakeVisible(exportProgress_);
  exportProgress_.setPercentageDisplay(true);

  previewButton_.setButtonText("Preview loop");
  halfTimeButton_.setButtonText("Half-time");
  syncHostButton_.setButtonText("Sync DAW BPM");
  tightenButton_.setButtonText("Tighten drums to tempo");
  classicLoopsButton_.setButtonText("Classic 112/118/124");
  stemsButton_.setButtonText("Stems");
  midiButton_.setButtonText("Pattern MIDI");
  manifestButton_.setButtonText("Pack manifest");
  batchSweepButton_.setButtonText("Batch BPM sweep");

  barsBox_.addItem("1 bar", 1);
  barsBox_.addItem("2 bars", 2);
  barsBox_.addItem("4 bars", 3);

  auto &apvts = processor_.getAPVTS();
  bpmAtt_ = std::make_unique<SAtt>(apvts, kParamLoopBpm, bpmSlider_);
  swingAtt_ = std::make_unique<SAtt>(apvts, kParamSwing, swingSlider_);
  dustAtt_ = std::make_unique<SAtt>(apvts, kParamDust, dustSlider_);
  masterAtt_ = std::make_unique<SAtt>(apvts, kParamMaster, masterSlider_);
  variationAtt_ = std::make_unique<SAtt>(apvts, kParamVariation, variationSlider_);
  batchAtt_ = std::make_unique<SAtt>(apvts, kParamBatchCount, batchCountSlider_);
  batchFromAtt_ = std::make_unique<SAtt>(apvts, kParamBatchFrom, batchFromSlider_);
  batchToAtt_ = std::make_unique<SAtt>(apvts, kParamBatchTo, batchToSlider_);
  barsAtt_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
      apvts, kParamLoopBars, barsBox_);

  previewAtt_ =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, kParamPreview,
                                                                              previewButton_);
  halfAtt_ =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, kParamHalfTime,
                                                                              halfTimeButton_);
  syncAtt_ =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, kParamSyncHost,
                                                                              syncHostButton_);
  tightenAtt_ =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, kParamTighten,
                                                                              tightenButton_);
  classicAtt_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      apvts, kParamClassicLoops, classicLoopsButton_);
  stemsAtt_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      apvts, kParamExportStems, stemsButton_);
  midiAtt_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      apvts, kParamExportMidi, midiButton_);
  manifestAtt_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      apvts, kParamMpcManifest, manifestButton_);
  sweepAtt_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      apvts, kParamBatchSweep, batchSweepButton_);

  bpmDown_.onClick = [this] { nudgeBpm(-1.0f); };
  bpmUp_.onClick = [this] { nudgeBpm(1.0f); };
  tapTempo_.onClick = [this] { registerTapTempo(); };
  bpmSlider_.onValueChange = [this] { syncBpmLabel(); };

  exportButton_.onClick = [this] { runExport(false); };
  batchExportButton_.onClick = [this] { runExport(true); };

  apvts.addParameterListener(kParamLoopBpm, this);
  apvts.addParameterListener(kParamSwing, this);
  apvts.addParameterListener(kParamDust, this);
  apvts.addParameterListener(kParamVariation, this);
  apvts.addParameterListener(kParamLoopBars, this);
  apvts.addParameterListener(kParamSyncHost, this);

  syncBpmLabel();
  syncLoopInfo();
  startTimerHz(4);
}

MemphisPhonkFactoryAudioProcessorEditor::~MemphisPhonkFactoryAudioProcessorEditor() {
  auto &apvts = processor_.getAPVTS();
  apvts.removeParameterListener(kParamLoopBpm, this);
  apvts.removeParameterListener(kParamSwing, this);
  apvts.removeParameterListener(kParamDust, this);
  apvts.removeParameterListener(kParamVariation, this);
  apvts.removeParameterListener(kParamLoopBars, this);
  apvts.removeParameterListener(kParamSyncHost, this);
}

void MemphisPhonkFactoryAudioProcessorEditor::wirePatternTools() {
  patternGrid_.onShiftLeft = [this] {
    processor_.shiftPattern(15);
    patternGrid_.setPattern(processor_.getPattern());
  };
  patternGrid_.onShiftRight = [this] {
    processor_.shiftPattern(1);
    patternGrid_.setPattern(processor_.getPattern());
  };
  patternGrid_.onToggleAB = [this] {
    processor_.togglePatternAB();
    patternGrid_.setPattern(processor_.getPattern());
    statusLabel_.setText(processor_.isUsingPatternB() ? "Pattern B active" : "Pattern A active",
                         juce::dontSendNotification);
  };
  patternGrid_.onSavePattern = [this] {
    auto chooser = std::make_shared<juce::FileChooser>("Save pattern", juce::File(), "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::saveMode, [this, chooser](const juce::FileChooser &fc) {
      const auto f = fc.getResult();
      if (f != juce::File{} && processor_.savePatternToFile(f.withFileExtension("json"))) {
        statusLabel_.setText("Pattern saved.", juce::dontSendNotification);
      }
    });
  };
  patternGrid_.onLoadPattern = [this] {
    auto chooser = std::make_shared<juce::FileChooser>("Load pattern", juce::File(), "*.json");
    chooser->launchAsync(juce::FileBrowserComponent::openMode, [this, chooser](const juce::FileChooser &fc) {
      const auto f = fc.getResult();
      if (f != juce::File{} && processor_.loadPatternFromFile(f)) {
        patternGrid_.setPattern(processor_.getPattern());
        statusLabel_.setText("Pattern loaded.", juce::dontSendNotification);
      }
    });
  };
}

void MemphisPhonkFactoryAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colour(0xff1a1410));
  g.setColour(juce::Colour(0xffe86c2a));
  g.setFont(juce::Font(22.0f, juce::Font::bold));
  g.drawText("Memphis Phonk Factory — MPC pack maker", 16, 8, getWidth() - 32, 28,
             juce::Justification::centredLeft);
  g.setColour(juce::Colours::white.withAlpha(0.65f));
  g.setFont(12.0f);
  g.drawText("Infinite dusty kits · stems · MIDI · manifest", 16, 34, 400, 18,
             juce::Justification::centredLeft);
}

void MemphisPhonkFactoryAudioProcessorEditor::resized() {
  auto r = getLocalBounds().reduced(12);
  r.removeFromTop(44);
  patternGrid_.setBounds(r.removeFromTop(158));
  r.removeFromTop(6);

  auto left = r.removeFromLeft(200);
  bpmSlider_.setBounds(left.removeFromTop(100).reduced(4));
  auto bpmSide = left.removeFromTop(88);
  bpmValueLabel_.setBounds(bpmSide.removeFromTop(24));
  tapTempo_.setBounds(bpmSide.removeFromTop(24).reduced(2));
  auto nudge = bpmSide.removeFromTop(24);
  bpmDown_.setBounds(nudge.removeFromLeft(nudge.getWidth() / 2).reduced(2));
  bpmUp_.setBounds(nudge.reduced(2));
  loopInfoLabel_.setBounds(left.removeFromTop(40));

  auto mid = r.removeFromLeft(340);
  auto row = [&](juce::Component &c, int h = 26) {
    c.setBounds(mid.removeFromTop(h).reduced(0, 2));
  };
  row(barsBox_, 28);
  row(swingSlider_);
  row(dustSlider_);
  row(masterSlider_);
  row(variationSlider_);
  row(batchCountSlider_);
  row(batchFromSlider_);
  row(batchToSlider_);

  auto toggles = r;
  for (auto *b :
       {&previewButton_, &halfTimeButton_, &syncHostButton_, &tightenButton_, &classicLoopsButton_,
        &stemsButton_, &midiButton_, &manifestButton_, &batchSweepButton_}) {
    b->setBounds(toggles.removeFromTop(24).reduced(0, 1));
  }

  auto bottom = getLocalBounds().removeFromBottom(64).reduced(12);
  exportButton_.setBounds(bottom.removeFromLeft(140).reduced(2));
  batchExportButton_.setBounds(bottom.removeFromLeft(120).reduced(2));
  exportProgress_.setBounds(bottom.removeFromLeft(160).reduced(2));
  statusLabel_.setBounds(bottom.reduced(2));
}

void MemphisPhonkFactoryAudioProcessorEditor::parameterChanged(const juce::String &parameterID,
                                                                float) {
  if (parameterID == kParamLoopBpm || parameterID == kParamSyncHost) {
    syncBpmLabel();
  }
  if (parameterID == kParamLoopBpm || parameterID == kParamLoopBars || parameterID == kParamSyncHost) {
    syncLoopInfo();
  }
  if (parameterID == kParamLoopBpm || parameterID == kParamSwing || parameterID == kParamDust ||
      parameterID == kParamVariation || parameterID == kParamLoopBars) {
    processor_.schedulePreviewRefresh();
  }
}

void MemphisPhonkFactoryAudioProcessorEditor::timerCallback() { syncLoopInfo(); }

void MemphisPhonkFactoryAudioProcessorEditor::nudgeBpm(float delta) {
  auto &apvts = processor_.getAPVTS();
  if (auto *p = apvts.getParameter(kParamLoopBpm)) {
    const float next =
        juce::jlimit(70.0f, 170.0f, processor_.getEffectiveBpm() + delta);
    p->setValueNotifyingHost(p->convertTo0to1(next));
    syncBpmLabel();
    processor_.schedulePreviewRefresh();
  }
}

void MemphisPhonkFactoryAudioProcessorEditor::syncBpmLabel() {
  const float bpm = processor_.getEffectiveBpm();
  bpmValueLabel_.setText(juce::String(bpm, 1) + " BPM", juce::dontSendNotification);
}

void MemphisPhonkFactoryAudioProcessorEditor::syncLoopInfo() {
  const float sec = processor_.getLoopDurationSec();
  loopInfoLabel_.setText("Loop: " + juce::String(sec, 2) + " s @ " +
                             juce::String(processor_.getEffectiveBpm(), 1) + " BPM",
                         juce::dontSendNotification);
}

void MemphisPhonkFactoryAudioProcessorEditor::registerTapTempo() {
  const double now = juce::Time::getMillisecondCounterHiRes() * 0.001;
  tapTimesSec_.push_back(now);
  if (tapTimesSec_.size() > 4) {
    tapTimesSec_.erase(tapTimesSec_.begin());
  }
  if (tapTimesSec_.size() >= 2) {
    double sum = 0.0;
    for (size_t i = 1; i < tapTimesSec_.size(); ++i) {
      sum += tapTimesSec_[i] - tapTimesSec_[i - 1];
    }
    const double avg = sum / static_cast<double>(tapTimesSec_.size() - 1);
    if (avg > 0.2) {
      const float bpm = juce::jlimit(70.0f, 170.0f, static_cast<float>(60.0 / avg));
      if (auto *p = processor_.getAPVTS().getParameter(kParamLoopBpm)) {
        p->setValueNotifyingHost(p->convertTo0to1(bpm));
        syncBpmLabel();
        processor_.schedulePreviewRefresh();
      }
    }
  }
}

void MemphisPhonkFactoryAudioProcessorEditor::runExport(bool batch) {
  if (processor_.isExportBusy()) {
    statusLabel_.setText("Export already running…", juce::dontSendNotification);
    return;
  }
  auto chooser = std::make_shared<juce::FileChooser>(
      batch ? "Batch export folder" : "MPC pack folder",
      juce::File::getSpecialLocation(juce::File::userHomeDirectory));
  chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                       [this, batch, chooser](const juce::FileChooser &fc) {
                         const auto dir = fc.getResult();
                         if (dir == juce::File{}) {
                           return;
                         }
                         exportProgressValue_ = 0.02;
                         exportButton_.setEnabled(false);
                         batchExportButton_.setEnabled(false);
                         processor_.startBackgroundExport(
                             dir, batch,
                             [this](juce::String msg) {
                               statusLabel_.setText(msg, juce::dontSendNotification);
                               exportProgressValue_ = 1.0;
                               exportButton_.setEnabled(true);
                               batchExportButton_.setEnabled(true);
                             },
                             [this](float p) { exportProgressValue_ = p; });
                       });
}
