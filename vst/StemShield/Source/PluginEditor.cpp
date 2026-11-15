#include "PluginEditor.h"

namespace {
constexpr int kWidth = 580;
constexpr int kHeight = 340;

void styleKnob(juce::Slider& s, const juce::String& text) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
  s.setPopupDisplayEnabled(true, false, nullptr);
  s.setTextValueSuffix(text);
}
} // namespace

StemShieldAudioProcessorEditor::KnobLookAndFeel::KnobLookAndFeel() {
  setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff10b981));
  setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2e3d));
  setColour(juce::Slider::thumbColourId, juce::Colour(0xfff3f4f6));
  setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff3f4f6));
  setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void StemShieldAudioProcessorEditor::KnobLookAndFeel::drawRotarySlider(
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

StemShieldAudioProcessorEditor::StemShieldAudioProcessorEditor(StemShieldAudioProcessor& p)
    : AudioProcessorEditor(p), proc_(p) {
  setLookAndFeel(&laf_);
  setSize(kWidth, kHeight);
  startTimerHz(30);

  titleLabel_.setText("DISKLORDZ STEM-SHIELD", juce::dontSendNotification);
  titleLabel_.setFont(juce::Font(22.0f, juce::Font::bold));
  titleLabel_.setColour(juce::Label::textColourId, juce::Colours::white);
  titleLabel_.setJustificationType(juce::Justification::centredLeft);
  addAndMakeVisible(titleLabel_);

  subtitleLabel_.setText("Sub-bus peak containment & stem balancer", juce::dontSendNotification);
  subtitleLabel_.setFont(juce::Font(12.0f));
  subtitleLabel_.setColour(juce::Label::textColourId, juce::Colour(0xff9ca3af));
  subtitleLabel_.setJustificationType(juce::Justification::centredLeft);
  addAndMakeVisible(subtitleLabel_);

  for (int i = 0; i < kStemShieldFactoryPresetCount; ++i) {
    presetBox_.addItem(proc_.getProgramName(i), i + 1);
  }
  presetBox_.setSelectedId(proc_.getCurrentProgram() + 1, juce::dontSendNotification);
  presetBox_.addListener(this);
  addAndMakeVisible(presetBox_);

  styleKnob(ceilingKnob_, " dBTP");
  styleKnob(subBalanceKnob_, " %");
  styleKnob(crossoverKnob_, " Hz");
  styleKnob(releaseKnob_, " ms");
  styleKnob(lookaheadKnob_, " ms");
  styleKnob(gainKnob_, " dB");

  auto styleTitle = [](juce::Label& l, const juce::String& t) {
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(10.0f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, juce::Colour(0xff6ee7b7));
    l.setJustificationType(juce::Justification::centred);
  };
  styleTitle(ceilingLabel_, "CEILING");
  styleTitle(subBalanceLabel_, "SUB BALANCE");
  styleTitle(crossoverLabel_, "SUB XOVER");
  styleTitle(releaseLabel_, "RELEASE");
  styleTitle(lookaheadLabel_, "LOOKAHEAD");
  styleTitle(gainLabel_, "GAIN");

  for (juce::Component* c :
       {static_cast<juce::Component*>(&ceilingKnob_), static_cast<juce::Component*>(&subBalanceKnob_),
        static_cast<juce::Component*>(&crossoverKnob_), static_cast<juce::Component*>(&releaseKnob_),
        static_cast<juce::Component*>(&lookaheadKnob_), static_cast<juce::Component*>(&gainKnob_),
        static_cast<juce::Component*>(&ceilingLabel_), static_cast<juce::Component*>(&subBalanceLabel_),
        static_cast<juce::Component*>(&crossoverLabel_), static_cast<juce::Component*>(&releaseLabel_),
        static_cast<juce::Component*>(&lookaheadLabel_), static_cast<juce::Component*>(&gainLabel_)}) {
    addAndMakeVisible(c);
  }

  auto& apvts = proc_.getAPVTS();
  ceilingAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, StemShieldAudioProcessor::kParamCeiling, ceilingKnob_);
  subBalanceAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, StemShieldAudioProcessor::kParamSubBalance, subBalanceKnob_);
  crossoverAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, StemShieldAudioProcessor::kParamSubCrossover, crossoverKnob_);
  releaseAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, StemShieldAudioProcessor::kParamRelease, releaseKnob_);
  lookaheadAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, StemShieldAudioProcessor::kParamLookahead, lookaheadKnob_);
  gainAttach_ =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, StemShieldAudioProcessor::kParamGain, gainKnob_);
}

StemShieldAudioProcessorEditor::~StemShieldAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void StemShieldAudioProcessorEditor::timerCallback() {
  meterOutDb_ = proc_.getMeters().outputPeakDb.load();
  meterGrDb_ = proc_.getMeters().gainReductionDb.load();
  repaint(outputMeterRect_.expanded(2));
  repaint(grMeterRect_.expanded(2));
}

void StemShieldAudioProcessorEditor::paintMeters(juce::Graphics& g) {
  auto drawBar = [&](juce::Rectangle<int> r, float norm, juce::Colour c, const juce::String& title) {
    g.setColour(juce::Colour(0xff12141c));
    g.fillRoundedRectangle(r.toFloat(), 4.0f);
    g.setColour(juce::Colour(0xff2a2e3d));
    g.drawRoundedRectangle(r.toFloat(), 4.0f, 1.0f);
    auto fill = r.reduced(2);
    fill.setHeight(static_cast<int>(fill.getHeight() * norm));
    fill.setY(r.getBottom() - fill.getHeight());
    g.setColour(c);
    g.fillRoundedRectangle(fill.toFloat(), 3.0f);
    g.setColour(juce::Colour(0xff9ca3af));
    g.setFont(10.0f);
    g.drawText(title, r.getX(), r.getY() - 14, r.getWidth(), 12, juce::Justification::centred);
  };

  const float outNorm = juce::jlimit(0.0f, 1.0f, (meterOutDb_ + 60.0f) / 60.0f);
  const float grNorm = juce::jlimit(0.0f, 1.0f, std::fabs(meterGrDb_) / 24.0f);
  drawBar(outputMeterRect_, outNorm, juce::Colour(0xff10b981), "OUT");
  drawBar(grMeterRect_, grNorm, juce::Colour(0xffef4444), "GR");
}

void StemShieldAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(0xff0b0c10));
  g.setColour(juce::Colour(0xff1a1d28));
  g.fillRoundedRectangle(12.0f, 58.0f, static_cast<float>(getWidth()) - 24.0f,
                         static_cast<float>(getHeight()) - 70.0f, 10.0f);
  g.setColour(juce::Colour(0xff2a2e3d));
  g.drawRoundedRectangle(12.0f, 58.0f, static_cast<float>(getWidth()) - 24.0f,
                         static_cast<float>(getHeight()) - 70.0f, 10.0f, 1.0f);
  paintMeters(g);
}

void StemShieldAudioProcessorEditor::resized() {
  auto r = getLocalBounds().reduced(16);
  auto top = r.removeFromTop(40);
  titleLabel_.setBounds(top.removeFromTop(24));
  subtitleLabel_.setBounds(top);
  presetBox_.setBounds(r.removeFromTop(28).removeFromRight(240));

  auto meters = r.removeFromRight(56);
  outputMeterRect_ = meters.removeFromTop(meters.getHeight() / 2 - 4).reduced(4, 8);
  grMeterRect_ = meters.reduced(4, 8);

  auto panel = r.reduced(8, 12);
  const int knobW = panel.getWidth() / 3;
  const int knobH = panel.getHeight() - 20;

  auto row = panel;
  auto placeKnob = [&](juce::Slider& knob, juce::Label& lab) {
    auto cell = row.removeFromLeft(knobW);
    lab.setBounds(cell.removeFromTop(18));
    knob.setBounds(cell.withSizeKeepingCentre(knobW, knobH));
  };

  placeKnob(ceilingKnob_, ceilingLabel_);
  placeKnob(subBalanceKnob_, subBalanceLabel_);
  placeKnob(crossoverKnob_, crossoverLabel_);

  row = panel;
  row.removeFromLeft(knobW / 2);
  placeKnob(releaseKnob_, releaseLabel_);
  placeKnob(lookaheadKnob_, lookaheadLabel_);
  placeKnob(gainKnob_, gainLabel_);
}

void StemShieldAudioProcessorEditor::comboBoxChanged(juce::ComboBox* box) {
  if (box == &presetBox_) {
    proc_.setCurrentProgram(presetBox_.getSelectedItemIndex());
  }
}
