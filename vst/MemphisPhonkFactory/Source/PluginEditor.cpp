#include "PluginEditor.h"

namespace {
constexpr const char *kParamLoopBpm = "mpf_loop_bpm";
constexpr const char *kParamSwing = "mpf_swing";
constexpr const char *kParamDrive = "mpf_drive";
constexpr const char *kParamHiss = "mpf_hiss";
constexpr const char *kParamVariation = "mpf_variation";
constexpr const char *kParamPreview = "mpf_preview";
constexpr const char *kParamMaster = "mpf_master";

void styleSlider(juce::Slider &s, const juce::String &suffix = {}) {
  s.setSliderStyle(juce::Slider::LinearHorizontal);
  s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 64, 20);
  s.setTextValueSuffix(suffix);
}
} // namespace

MemphisPhonkFactoryAudioProcessorEditor::MemphisPhonkFactoryAudioProcessorEditor(
    MemphisPhonkFactoryAudioProcessor &p)
    : AudioProcessorEditor(&p), processor_(p) {
  setSize(560, 360);

  styleSlider(bpmSlider_);
  styleSlider(swingSlider_);
  styleSlider(driveSlider_);
  styleSlider(hissSlider_);
  styleSlider(masterSlider_);
  variationSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
  variationSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
  variationSlider_.setRange(1, 4, 1);

  addAndMakeVisible(bpmSlider_);
  addAndMakeVisible(swingSlider_);
  addAndMakeVisible(driveSlider_);
  addAndMakeVisible(hissSlider_);
  addAndMakeVisible(masterSlider_);
  addAndMakeVisible(variationSlider_);
  addAndMakeVisible(previewButton_);
  addAndMakeVisible(exportButton_);
  addAndMakeVisible(statusLabel_);

  previewButton_.setButtonText("Loop preview");

  statusLabel_.setJustificationType(juce::Justification::centredLeft);
  statusLabel_.setText("Generate Memphis phonk one-shots + 112/118/124 BPM loops.", juce::dontSendNotification);

  auto &apvts = processor_.getAPVTS();
  bpmAtt_ = std::make_unique<Att>(apvts, kParamLoopBpm, bpmSlider_);
  swingAtt_ = std::make_unique<Att>(apvts, kParamSwing, swingSlider_);
  driveAtt_ = std::make_unique<Att>(apvts, kParamDrive, driveSlider_);
  hissAtt_ = std::make_unique<Att>(apvts, kParamHiss, hissSlider_);
  masterAtt_ = std::make_unique<Att>(apvts, kParamMaster, masterSlider_);
  variationAtt_ = std::make_unique<Att>(apvts, kParamVariation, variationSlider_);
  previewAtt_ =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, kParamPreview,
                                                                             previewButton_);

  exportButton_.onClick = [this] { exportKit(); };

  apvts.addParameterListener(kParamLoopBpm, this);
  apvts.addParameterListener(kParamSwing, this);
  apvts.addParameterListener(kParamDrive, this);
  apvts.addParameterListener(kParamHiss, this);
  apvts.addParameterListener(kParamVariation, this);

  startTimerHz(4);
  processor_.refreshPreviewLoop();
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
  g.drawText("Memphis Phonk Factory", 16, 8, getWidth() - 32, 32, juce::Justification::centredLeft);
  g.setColour(juce::Colours::white.withAlpha(0.75f));
  g.setFont(13.0f);
  g.drawText("Loop BPM", 16, 52, 90, 20, juce::Justification::centredLeft);
  g.drawText("Swing", 16, 92, 90, 20, juce::Justification::centredLeft);
  g.drawText("Tape drive", 16, 132, 90, 20, juce::Justification::centredLeft);
  g.drawText("Hiss", 16, 172, 90, 20, juce::Justification::centredLeft);
  g.drawText("Variation", 16, 212, 90, 20, juce::Justification::centredLeft);
  g.drawText("Master", 16, 252, 90, 20, juce::Justification::centredLeft);
}

void MemphisPhonkFactoryAudioProcessorEditor::resized() {
  auto r = getLocalBounds().reduced(16);
  r.removeFromTop(40);
  auto row = [&](int y) {
    auto line = r.withY(y).withHeight(28);
    line.removeFromLeft(100);
    return line.reduced(0, 4);
  };
  bpmSlider_.setBounds(row(48));
  swingSlider_.setBounds(row(88));
  driveSlider_.setBounds(row(128));
  hissSlider_.setBounds(row(168));
  variationSlider_.setBounds(row(208));
  masterSlider_.setBounds(row(248));

  auto bottom = getLocalBounds().removeFromBottom(72).reduced(16);
  previewButton_.setBounds(bottom.removeFromLeft(140).reduced(4));
  exportButton_.setBounds(bottom.removeFromLeft(160).reduced(4));
  statusLabel_.setBounds(bottom.reduced(4));
}

void MemphisPhonkFactoryAudioProcessorEditor::parameterChanged(const juce::String &parameterID,
                                                                float) {
  if (parameterID == kParamLoopBpm || parameterID == kParamSwing || parameterID == kParamDrive ||
      parameterID == kParamHiss || parameterID == kParamVariation) {
    processor_.refreshPreviewLoop();
  }
}

void MemphisPhonkFactoryAudioProcessorEditor::timerCallback() {}

void MemphisPhonkFactoryAudioProcessorEditor::exportKit() {
  auto chooser = std::make_shared<juce::FileChooser>(
      "Choose folder for Memphis_Phonk_MPC_Kit", juce::File::getSpecialLocation(juce::File::userHomeDirectory));
  chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                       [this, chooser](const juce::FileChooser &fc) {
                         const auto dir = fc.getResult();
                         if (dir == juce::File{}) {
                           return;
                         }
                         const juce::String msg = processor_.exportKitToFolder(dir);
                         statusLabel_.setText(msg, juce::dontSendNotification);
                       });
}
