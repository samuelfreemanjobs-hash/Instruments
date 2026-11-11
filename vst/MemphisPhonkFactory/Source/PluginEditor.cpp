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

void styleSlider(juce::Slider &s, const juce::String &suffix = {}) {
  s.setSliderStyle(juce::Slider::LinearHorizontal);
  s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 56, 18);
  s.setTextValueSuffix(suffix);
}
} // namespace

MemphisPhonkFactoryAudioProcessorEditor::MemphisPhonkFactoryAudioProcessorEditor(
    MemphisPhonkFactoryAudioProcessor &p)
    : AudioProcessorEditor(&p), processor_(p) {
  setSize(720, 520);

  patternGrid_.setPattern(processor_.getPattern());
  patternGrid_.onPatternChanged = [this](const DrumPattern &pat) {
    processor_.setPattern(pat);
  };
  addAndMakeVisible(patternGrid_);

  bpmSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  bpmSlider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  bpmSlider_.setRange(70.0, 170.0, 0.1);
  bpmSlider_.setSkewFactorFromMidPoint(120.0);
  bpmValueLabel_.setJustificationType(juce::Justification::centred);
  bpmValueLabel_.setFont(juce::Font(18.0f, juce::Font::bold));
  bpmValueLabel_.setColour(juce::Label::textColourId, juce::Colour(0xffe86c2a));

  styleSlider(swingSlider_);
  styleSlider(driveSlider_);
  styleSlider(hissSlider_);
  styleSlider(masterSlider_);
  variationSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
  variationSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 18);
  variationSlider_.setRange(1, 4, 1);
  batchCountSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
  batchCountSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 18);
  batchCountSlider_.setRange(1, 16, 1);

  addAndMakeVisible(bpmSlider_);
  addAndMakeVisible(bpmDown_);
  addAndMakeVisible(bpmUp_);
  addAndMakeVisible(bpmValueLabel_);
  addAndMakeVisible(classicLoopsButton_);
  addAndMakeVisible(swingSlider_);
  addAndMakeVisible(driveSlider_);
  addAndMakeVisible(hissSlider_);
  addAndMakeVisible(masterSlider_);
  addAndMakeVisible(variationSlider_);
  addAndMakeVisible(batchCountSlider_);
  addAndMakeVisible(previewButton_);
  addAndMakeVisible(exportButton_);
  addAndMakeVisible(batchExportButton_);
  addAndMakeVisible(statusLabel_);

  previewButton_.setButtonText("Loop preview");
  classicLoopsButton_.setButtonText("Also export 112 / 118 / 124 loops");

  bpmDown_.onClick = [this] { nudgeBpm(-1.0f); };
  bpmUp_.onClick = [this] { nudgeBpm(1.0f); };
  bpmSlider_.onValueChange = [this] { syncBpmLabel(); };

  statusLabel_.setJustificationType(juce::Justification::centredLeft);
  statusLabel_.setText("Edit pattern, preview, export one kit or a timestamped batch.", juce::dontSendNotification);

  auto &apvts = processor_.getAPVTS();
  bpmAtt_ = std::make_unique<Att>(apvts, kParamLoopBpm, bpmSlider_);
  swingAtt_ = std::make_unique<Att>(apvts, kParamSwing, swingSlider_);
  driveAtt_ = std::make_unique<Att>(apvts, kParamDrive, driveSlider_);
  hissAtt_ = std::make_unique<Att>(apvts, kParamHiss, hissSlider_);
  masterAtt_ = std::make_unique<Att>(apvts, kParamMaster, masterSlider_);
  variationAtt_ = std::make_unique<Att>(apvts, kParamVariation, variationSlider_);
  batchAtt_ = std::make_unique<Att>(apvts, kParamBatchCount, batchCountSlider_);
  previewAtt_ =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, kParamPreview,
                                                                             previewButton_);
  classicAtt_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      apvts, kParamClassicLoops, classicLoopsButton_);

  exportButton_.onClick = [this] { exportKit(); };
  batchExportButton_.onClick = [this] { exportBatch(); };

  apvts.addParameterListener(kParamLoopBpm, this);
  apvts.addParameterListener(kParamSwing, this);
  apvts.addParameterListener(kParamDrive, this);
  apvts.addParameterListener(kParamHiss, this);
  apvts.addParameterListener(kParamVariation, this);

  syncBpmLabel();
  processor_.refreshPreviewLoop();
}

void MemphisPhonkFactoryAudioProcessorEditor::nudgeBpm(float delta) {
  auto &apvts = processor_.getAPVTS();
  if (auto *p = apvts.getParameter(kParamLoopBpm)) {
    const float next =
        juce::jlimit(70.0f, 170.0f, apvts.getRawParameterValue(kParamLoopBpm)->load() + delta);
    p->setValueNotifyingHost(p->convertTo0to1(next));
    syncBpmLabel();
    processor_.refreshPreviewLoop();
  }
}

void MemphisPhonkFactoryAudioProcessorEditor::syncBpmLabel() {
  const float bpm = processor_.getAPVTS().getRawParameterValue(kParamLoopBpm)->load();
  bpmValueLabel_.setText(juce::String(bpm, 1) + " BPM", juce::dontSendNotification);
}

MemphisPhonkFactoryAudioProcessorEditor::~MemphisPhonkFactoryAudioProcessorEditor() {
  auto &apvts = processor_.getAPVTS();
  apvts.removeParameterListener(kParamLoopBpm, this);
  apvts.removeParameterListener(kParamSwing, this);
  apvts.removeParameterListener(kParamDrive, this);
  apvts.removeParameterListener(kParamHiss, this);
  apvts.removeParameterListener(kParamVariation, this);
}

void MemphisPhonkFactoryAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colour(0xff1a1410));
  g.setColour(juce::Colour(0xffe86c2a));
  g.setFont(juce::Font(22.0f, juce::Font::bold));
  g.drawText("Memphis Phonk Factory", 16, 8, getWidth() - 32, 28, juce::Justification::centredLeft);

  g.setColour(juce::Colours::white.withAlpha(0.7f));
  g.setFont(12.0f);
  const int baseY = 300;
  g.drawText("Loop tempo", 16, baseY - 4, 120, 18, juce::Justification::centredLeft);
  g.drawText("Swing", 200, baseY + 8, 72, 18, juce::Justification::centredLeft);
  g.drawText("Drive", 200, baseY + 40, 72, 18, juce::Justification::centredLeft);
  g.drawText("Hiss", 360, baseY, 72, 18, juce::Justification::centredLeft);
  g.drawText("Variation", 360, baseY + 32, 72, 18, juce::Justification::centredLeft);
  g.drawText("Batch #", 360, baseY + 64, 72, 18, juce::Justification::centredLeft);
  g.drawText("Master", 16, baseY + 96, 72, 18, juce::Justification::centredLeft);
}

void MemphisPhonkFactoryAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds().reduced(12);
  bounds.removeFromTop(36);
  patternGrid_.setBounds(bounds.removeFromTop(150));
  bounds.removeFromTop(8);

  const int baseY = 300;
  auto place = [&](juce::Slider &s, int x, int y, int w) {
    s.setBounds(x, y, w, 24);
  };

  auto bpmBlock = juce::Rectangle<int>(16, baseY, 160, 120);
  bpmSlider_.setBounds(bpmBlock.removeFromLeft(88).reduced(2));
  auto bpmSide = bpmBlock.reduced(4);
  bpmValueLabel_.setBounds(bpmSide.removeFromTop(28));
  bpmUp_.setBounds(bpmSide.removeFromTop(26).reduced(2));
  bpmDown_.setBounds(bpmSide.removeFromTop(26).reduced(2));

  place(swingSlider_, 272, baseY + 4, 220);
  place(driveSlider_, 272, baseY + 36, 220);
  place(masterSlider_, 88, baseY + 96, 404);
  classicLoopsButton_.setBounds(16, baseY + 124, 280, 24);
  place(hissSlider_, 430, baseY, 260);
  place(variationSlider_, 430, baseY + 32, 260);
  place(batchCountSlider_, 430, baseY + 64, 260);

  auto bottom = getLocalBounds().removeFromBottom(56).reduced(12);
  previewButton_.setBounds(bottom.removeFromLeft(110).reduced(2));
  exportButton_.setBounds(bottom.removeFromLeft(100).reduced(2));
  batchExportButton_.setBounds(bottom.removeFromLeft(110).reduced(2));
  statusLabel_.setBounds(bottom.reduced(2));
}

void MemphisPhonkFactoryAudioProcessorEditor::parameterChanged(const juce::String &parameterID,
                                                                float) {
  if (parameterID == kParamLoopBpm) {
    syncBpmLabel();
  }
  if (parameterID == kParamLoopBpm || parameterID == kParamSwing || parameterID == kParamDrive ||
      parameterID == kParamHiss || parameterID == kParamVariation) {
    processor_.refreshPreviewLoop();
  }
}

void MemphisPhonkFactoryAudioProcessorEditor::timerCallback() {}

void MemphisPhonkFactoryAudioProcessorEditor::exportKit() {
  auto chooser = std::make_shared<juce::FileChooser>(
      "Export Memphis_Phonk_MPC_Kit", juce::File::getSpecialLocation(juce::File::userHomeDirectory));
  chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                       [this, chooser](const juce::FileChooser &fc) {
                         const auto dir = fc.getResult();
                         if (dir == juce::File{}) {
                           return;
                         }
                         statusLabel_.setText(processor_.exportKitToFolder(dir),
                                              juce::dontSendNotification);
                       });
}

void MemphisPhonkFactoryAudioProcessorEditor::exportBatch() {
  auto chooser = std::make_shared<juce::FileChooser>(
      "Export batch kits", juce::File::getSpecialLocation(juce::File::userHomeDirectory));
  chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                       [this, chooser](const juce::FileChooser &fc) {
                         const auto dir = fc.getResult();
                         if (dir == juce::File{}) {
                           return;
                         }
                         statusLabel_.setText(processor_.exportBatchToFolder(dir),
                                              juce::dontSendNotification);
                       });
}
