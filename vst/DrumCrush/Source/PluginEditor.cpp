#include "PluginEditor.h"

namespace {
constexpr int kWidth = 520;
constexpr int kHeight = 320;

void styleKnob(juce::Slider& s, const juce::String& text) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
  s.setPopupDisplayEnabled(true, false, nullptr);
  s.setTextValueSuffix(text);
}
} // namespace

DrumCrushAudioProcessorEditor::KnobLookAndFeel::KnobLookAndFeel() {
  setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff10b981));
  setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2e3d));
  setColour(juce::Slider::thumbColourId, juce::Colour(0xfff3f4f6));
  setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff3f4f6));
  setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void DrumCrushAudioProcessorEditor::KnobLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) {
  const float radius = static_cast<float>(juce::jmin(width, height)) * 0.42f;
  const float cx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
  const float cy = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
  const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

  g.setColour(juce::Colour(0xff1a1d28));
  g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

  juce::Path arc;
  arc.addCentredArc(cx, cy, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
  g.setColour(juce::Colour(0xff2a2e3d));
  g.strokePath(arc, juce::PathStrokeType(4.0f));

  juce::Path active;
  active.addCentredArc(cx, cy, radius, radius, 0.0f, rotaryStartAngle, angle, true);
  g.setColour(juce::Colour(0xff10b981));
  g.strokePath(active, juce::PathStrokeType(4.0f));

  const float nx = cx + std::sin(angle) * (radius - 6.0f);
  const float ny = cy - std::cos(angle) * (radius - 6.0f);
  g.setColour(juce::Colours::white);
  g.fillEllipse(nx - 4.0f, ny - 4.0f, 8.0f, 8.0f);
}

DrumCrushAudioProcessorEditor::DrumCrushAudioProcessorEditor(DrumCrushAudioProcessor& p)
    : AudioProcessorEditor(p), proc_(p) {
  setLookAndFeel(&laf_);
  setSize(kWidth, kHeight);

  titleLabel_.setText("DISKLORDZ DRUM-CRUSH", juce::dontSendNotification);
  titleLabel_.setFont(juce::Font(22.0f, juce::Font::bold));
  titleLabel_.setColour(juce::Label::textColourId, juce::Colours::white);
  titleLabel_.setJustificationType(juce::Justification::centredLeft);
  addAndMakeVisible(titleLabel_);

  subtitleLabel_.setText("Parallel bus punch & brickwall soft-clip", juce::dontSendNotification);
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

  styleKnob(punchKnob_, " %");
  styleKnob(crushKnob_, " %");
  styleKnob(clipKnob_, " %");
  styleKnob(mixKnob_, " %");
  styleKnob(outputKnob_, " dB");

  auto styleTitle = [](juce::Label& l, const juce::String& t) {
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(11.0f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, juce::Colour(0xff34d399));
    l.setJustificationType(juce::Justification::centred);
  };
  styleTitle(punchLabel_, "PUNCH");
  styleTitle(crushLabel_, "CRUSH");
  styleTitle(clipLabel_, "CLIP");
  styleTitle(mixLabel_, "MIX");
  styleTitle(outputLabel_, "OUTPUT");

  for (juce::Component* c :
       {static_cast<juce::Component*>(&punchKnob_), static_cast<juce::Component*>(&crushKnob_),
        static_cast<juce::Component*>(&clipKnob_), static_cast<juce::Component*>(&mixKnob_),
        static_cast<juce::Component*>(&outputKnob_), static_cast<juce::Component*>(&punchLabel_),
        static_cast<juce::Component*>(&crushLabel_), static_cast<juce::Component*>(&clipLabel_),
        static_cast<juce::Component*>(&mixLabel_), static_cast<juce::Component*>(&outputLabel_)}) {
    addAndMakeVisible(c);
  }

  auto& apvts = proc_.getAPVTS();
  punchAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, DrumCrushAudioProcessor::kParamPunch, punchKnob_);
  crushAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, DrumCrushAudioProcessor::kParamCrush, crushKnob_);
  clipAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, DrumCrushAudioProcessor::kParamClip, clipKnob_);
  mixAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, DrumCrushAudioProcessor::kParamMix, mixKnob_);
  outputAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, DrumCrushAudioProcessor::kParamOutputGain, outputKnob_);
}

DrumCrushAudioProcessorEditor::~DrumCrushAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void DrumCrushAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(0xff0b0c10));
  g.setColour(juce::Colour(0xff1a1d28));
  g.fillRoundedRectangle(12.0f, 58.0f, static_cast<float>(getWidth()) - 24.0f,
                         static_cast<float>(getHeight()) - 70.0f, 10.0f);
  g.setColour(juce::Colour(0xff2a2e3d));
  g.drawRoundedRectangle(12.0f, 58.0f, static_cast<float>(getWidth()) - 24.0f,
                         static_cast<float>(getHeight()) - 70.0f, 10.0f, 1.0f);
}

void DrumCrushAudioProcessorEditor::resized() {
  auto r = getLocalBounds().reduced(16);
  auto top = r.removeFromTop(40);
  titleLabel_.setBounds(top.removeFromTop(24));
  subtitleLabel_.setBounds(top);
  presetBox_.setBounds(r.removeFromTop(28).removeFromRight(220));

  auto panel = r.reduced(8, 12);
  const int knobW = panel.getWidth() / 3;
  const int knobH = panel.getHeight() - 20;

  auto row = panel;
  auto placeKnob = [&](juce::Slider& knob, juce::Label& lab) {
    auto cell = row.removeFromLeft(knobW);
    lab.setBounds(cell.removeFromTop(18));
    knob.setBounds(cell.withSizeKeepingCentre(knobW, knobH));
  };

  placeKnob(punchKnob_, punchLabel_);
  placeKnob(crushKnob_, crushLabel_);
  placeKnob(clipKnob_, clipLabel_);

  row = panel;
  row.removeFromLeft(knobW / 2);
  placeKnob(mixKnob_, mixLabel_);
  placeKnob(outputKnob_, outputLabel_);
}

void DrumCrushAudioProcessorEditor::comboBoxChanged(juce::ComboBox* box) {
  if (box == &presetBox_) {
    proc_.setCurrentProgram(presetBox_.getSelectedItemIndex());
  }
}
