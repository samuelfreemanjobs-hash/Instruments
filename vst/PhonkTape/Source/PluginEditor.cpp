#include "PluginEditor.h"

namespace {
constexpr int kWidth = 640;
constexpr int kHeight = 360;

void styleKnob(juce::Slider& s, const juce::String& text) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 16);
  s.setPopupDisplayEnabled(true, false, nullptr);
  s.setTextValueSuffix(text);
}
} // namespace

PhonkTapeAudioProcessorEditor::KnobLookAndFeel::KnobLookAndFeel() {
  setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff8b5cf6));
  setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2e3d));
  setColour(juce::Slider::thumbColourId, juce::Colour(0xfff3f4f6));
  setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff3f4f6));
  setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void PhonkTapeAudioProcessorEditor::KnobLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider&) {
  const float radius = static_cast<float>(juce::jmin(width, height)) * 0.40f;
  const float cx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
  const float cy = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
  const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

  g.setColour(juce::Colour(0xff1a1d28));
  g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

  juce::Path arc;
  arc.addCentredArc(cx, cy, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
  g.setColour(juce::Colour(0xff2a2e3d));
  g.strokePath(arc, juce::PathStrokeType(3.5f));

  juce::Path active;
  active.addCentredArc(cx, cy, radius, radius, 0.0f, rotaryStartAngle, angle, true);
  g.setColour(juce::Colour(0xfff59e0b));
  g.strokePath(active, juce::PathStrokeType(3.5f));

  const float nx = cx + std::sin(angle) * (radius - 5.0f);
  const float ny = cy - std::cos(angle) * (radius - 5.0f);
  g.setColour(juce::Colours::white);
  g.fillEllipse(nx - 3.5f, ny - 3.5f, 7.0f, 7.0f);
}

PhonkTapeAudioProcessorEditor::PhonkTapeAudioProcessorEditor(PhonkTapeAudioProcessor& p)
    : AudioProcessorEditor(p), proc_(p) {
  setLookAndFeel(&laf_);
  setSize(kWidth, kHeight);

  titleLabel_.setText("DISKLORDZ PHONK-TAPE", juce::dontSendNotification);
  titleLabel_.setFont(juce::Font(20.0f, juce::Font::bold));
  titleLabel_.setColour(juce::Label::textColourId, juce::Colours::white);
  titleLabel_.setJustificationType(juce::Justification::centredLeft);
  addAndMakeVisible(titleLabel_);

  subtitleLabel_.setText("Memphis cassette wow/flutter & tape drive", juce::dontSendNotification);
  subtitleLabel_.setFont(juce::Font(12.0f));
  subtitleLabel_.setColour(juce::Label::textColourId, juce::Colour(0xff9ca3af));
  subtitleLabel_.setJustificationType(juce::Justification::centredLeft);
  addAndMakeVisible(subtitleLabel_);

  for (int i = 0; i < kFactoryPresetCount; ++i) {
    presetBox_.addItem(proc_.getProgramName(i), i + 1);
  }
  presetBox_.setSelectedId(proc_.getCurrentProgram() + 1, juce::dontSendNotification);
  presetBox_.addListener(this);
  addAndMakeVisible(presetBox_);

  styleKnob(driveKnob_, " %");
  styleKnob(wowKnob_, " %");
  styleKnob(flutterKnob_, " %");
  styleKnob(ageKnob_, " %");
  styleKnob(noiseKnob_, " %");
  styleKnob(mixKnob_, " %");
  styleKnob(outputKnob_, " dB");

  auto styleTitle = [](juce::Label& l, const juce::String& t) {
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(10.0f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, juce::Colour(0xffa78bfa));
    l.setJustificationType(juce::Justification::centred);
  };
  styleTitle(driveLabel_, "DRIVE");
  styleTitle(wowLabel_, "WOW");
  styleTitle(flutterLabel_, "FLUTTER");
  styleTitle(ageLabel_, "AGE");
  styleTitle(noiseLabel_, "NOISE");
  styleTitle(mixLabel_, "MIX");
  styleTitle(outputLabel_, "OUTPUT");

  for (juce::Component* c :
       {static_cast<juce::Component*>(&driveKnob_), static_cast<juce::Component*>(&wowKnob_),
        static_cast<juce::Component*>(&flutterKnob_), static_cast<juce::Component*>(&ageKnob_),
        static_cast<juce::Component*>(&noiseKnob_), static_cast<juce::Component*>(&mixKnob_),
        static_cast<juce::Component*>(&outputKnob_), static_cast<juce::Component*>(&driveLabel_),
        static_cast<juce::Component*>(&wowLabel_), static_cast<juce::Component*>(&flutterLabel_),
        static_cast<juce::Component*>(&ageLabel_), static_cast<juce::Component*>(&noiseLabel_),
        static_cast<juce::Component*>(&mixLabel_), static_cast<juce::Component*>(&outputLabel_)}) {
    addAndMakeVisible(c);
  }

  auto& apvts = proc_.getAPVTS();
  driveAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, PhonkTapeAudioProcessor::kParamDrive, driveKnob_);
  wowAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, PhonkTapeAudioProcessor::kParamWow, wowKnob_);
  flutterAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, PhonkTapeAudioProcessor::kParamFlutter, flutterKnob_);
  ageAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, PhonkTapeAudioProcessor::kParamAge, ageKnob_);
  noiseAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, PhonkTapeAudioProcessor::kParamNoise, noiseKnob_);
  mixAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, PhonkTapeAudioProcessor::kParamMix, mixKnob_);
  outputAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, PhonkTapeAudioProcessor::kParamOutputGain, outputKnob_);
}

PhonkTapeAudioProcessorEditor::~PhonkTapeAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void PhonkTapeAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(0xff0b0c10));
  g.setColour(juce::Colour(0xff1a1d28));
  g.fillRoundedRectangle(10.0f, 52.0f, static_cast<float>(getWidth()) - 20.0f,
                         static_cast<float>(getHeight()) - 62.0f, 10.0f);
  g.setColour(juce::Colour(0xfff59e0b).withAlpha(0.35f));
  g.drawRoundedRectangle(10.0f, 52.0f, static_cast<float>(getWidth()) - 20.0f,
                         static_cast<float>(getHeight()) - 62.0f, 10.0f, 1.0f);
}

void PhonkTapeAudioProcessorEditor::resized() {
  auto r = getLocalBounds().reduced(14);
  auto top = r.removeFromTop(38);
  titleLabel_.setBounds(top.removeFromTop(22));
  subtitleLabel_.setBounds(top);
  presetBox_.setBounds(r.removeFromTop(26).removeFromRight(240));

  auto panel = r.reduced(6, 10);
  const int cols = 4;
  const int knobW = panel.getWidth() / cols;
  const int knobH = (panel.getHeight() - 24) / 2;

  auto row1 = panel.removeFromTop(knobH);
  auto place = [&](juce::Slider& knob, juce::Label& lab) {
    auto cell = row1.removeFromLeft(knobW);
    lab.setBounds(cell.removeFromTop(16));
    knob.setBounds(cell.withSizeKeepingCentre(knobW - 4, knobH - 8));
  };
  place(driveKnob_, driveLabel_);
  place(wowKnob_, wowLabel_);
  place(flutterKnob_, flutterLabel_);
  place(ageKnob_, ageLabel_);

  panel.removeFromTop(8);
  auto row2 = panel;
  row2.removeFromLeft(knobW / 2);
  auto place2 = [&](juce::Slider& knob, juce::Label& lab) {
    auto cell = row2.removeFromLeft(knobW);
    lab.setBounds(cell.removeFromTop(16));
    knob.setBounds(cell.withSizeKeepingCentre(knobW - 4, knobH - 8));
  };
  place2(noiseKnob_, noiseLabel_);
  place2(mixKnob_, mixLabel_);
  place2(outputKnob_, outputLabel_);
}

void PhonkTapeAudioProcessorEditor::comboBoxChanged(juce::ComboBox* box) {
  if (box == &presetBox_) {
    proc_.setCurrentProgram(presetBox_.getSelectedItemIndex());
  }
}
