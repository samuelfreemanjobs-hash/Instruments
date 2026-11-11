#include "PluginEditor.h"

namespace {
constexpr int kWidth = 560;
constexpr int kHeight = 452;

void styleSlider(juce::Slider& s) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 68, 18);
  s.setPopupDisplayEnabled(true, false, nullptr);
}

void styleTitle(juce::Label& l, const juce::String& text) {
  l.setText(text, juce::dontSendNotification);
  l.setFont(juce::Font(11.0f, juce::Font::bold));
  l.setColour(juce::Label::textColourId, juce::Colour(0xffa78bfa));
  l.setJustificationType(juce::Justification::centred);
}
} // namespace

GrimMIDIAudioProcessorEditor::GrimMIDIAudioProcessorEditor(GrimMIDIAudioProcessor& p)
    : AudioProcessorEditor(p), proc_(p) {
  setSize(kWidth, kHeight);

  titleLabel_.setText("DISKLORDZ GRIM-MIDI", juce::dontSendNotification);
  titleLabel_.setFont(juce::Font(22.0f, juce::Font::bold));
  titleLabel_.setColour(juce::Label::textColourId, juce::Colours::white);
  addAndMakeVisible(titleLabel_);

  subtitleLabel_.setText("Dark minor / phrygian 1-key chord engine", juce::dontSendNotification);
  subtitleLabel_.setFont(juce::Font(12.0f));
  subtitleLabel_.setColour(juce::Label::textColourId, juce::Colour(0xff9ca3af));
  addAndMakeVisible(subtitleLabel_);

  for (int i = 0; i < kProgressionPresetCount; ++i) {
    presetBox_.addItem(proc_.getProgramName(i), i + 1);
  }
  presetBox_.setSelectedId(proc_.getCurrentProgram() + 1, juce::dontSendNotification);
  presetBox_.addListener(this);
  addAndMakeVisible(presetBox_);

  for (int i = 0; i < 12; ++i) {
    keyBox_.addItem(juce::MidiMessage::getMidiNoteName(i + 60, true, true, 4), i + 1);
  }
  scaleBox_.addItemList(juce::StringArray{"Natural Minor", "Harmonic Minor", "Phrygian", "Phrygian Dominant"},
                        1);
  chordBox_.addItemList(
      juce::StringArray{"Minor Triad", "Minor 7", "Minor 9", "Minor 11", "Sus2 Minor", "Minor Add9",
                        "Diminished 7", "Power 5th"},
      1);
  for (int i = 0; i < 4; ++i) {
    inversionBox_.addItem("Inv " + juce::String(i), i + 1);
  }
  triggerModeBox_.addItemList(
      juce::StringArray{"Nearest Degree", "Degree Pads (white keys)", "Chromatic Root"}, 1);

  styleSlider(spreadSlider_);
  styleSlider(strumSlider_);
  styleSlider(velSlider_);
  styleSlider(octaveSlider_);

  strumSlider_.setTextValueSuffix(" ms");

  styleTitle(spreadLabel_, "SPREAD");
  styleTitle(strumLabel_, "STRUM");
  styleTitle(velLabel_, "VEL HUMAN");
  styleTitle(octaveLabel_, "OCTAVE");

  for (juce::Component* c :
       {static_cast<juce::Component*>(&spreadSlider_), static_cast<juce::Component*>(&strumSlider_),
        static_cast<juce::Component*>(&velSlider_), static_cast<juce::Component*>(&octaveSlider_),
        static_cast<juce::Component*>(&spreadLabel_), static_cast<juce::Component*>(&strumLabel_),
        static_cast<juce::Component*>(&velLabel_), static_cast<juce::Component*>(&octaveLabel_),
        static_cast<juce::Component*>(&keyBox_), static_cast<juce::Component*>(&scaleBox_),
        static_cast<juce::Component*>(&chordBox_), static_cast<juce::Component*>(&inversionBox_),
        static_cast<juce::Component*>(&triggerModeBox_)}) {
    addAndMakeVisible(c);
  }

  exportDragButton_.addListener(this);
  exportDragButton_.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff8b5cf6));
  exportDragButton_.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
  addAndMakeVisible(exportDragButton_);

  auto& apvts = proc_.getAPVTS();
  spreadAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, GrimMIDIAudioProcessor::kParamSpread, spreadSlider_);
  strumAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, GrimMIDIAudioProcessor::kParamStrumMs, strumSlider_);
  velAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, GrimMIDIAudioProcessor::kParamVelHumanize, velSlider_);
  octaveAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      apvts, GrimMIDIAudioProcessor::kParamOctave, octaveSlider_);

  scaleAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
      apvts, GrimMIDIAudioProcessor::kParamScale, scaleBox_);
  chordAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
      apvts, GrimMIDIAudioProcessor::kParamChordType, chordBox_);
  triggerAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
      apvts, GrimMIDIAudioProcessor::kParamTriggerMode, triggerModeBox_);

  keyBox_.setSelectedId(static_cast<int>(apvts.getRawParameterValue(GrimMIDIAudioProcessor::kParamRootKey)->load())
                        + 1);
  inversionBox_.setSelectedId(static_cast<int>(apvts.getRawParameterValue(GrimMIDIAudioProcessor::kParamInversion)->load())
                              + 1);
}

GrimMIDIAudioProcessorEditor::~GrimMIDIAudioProcessorEditor() = default;

void GrimMIDIAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(0xff0b0c10));
  g.setColour(juce::Colour(0xff1a1d28));
  g.fillRoundedRectangle(12.0f, 58.0f, static_cast<float>(getWidth()) - 24.0f,
                         static_cast<float>(getHeight()) - 70.0f, 10.0f);
  g.setColour(juce::Colour(0xff2a2e3d));
  g.drawRoundedRectangle(12.0f, 58.0f, static_cast<float>(getWidth()) - 24.0f,
                         static_cast<float>(getHeight()) - 70.0f, 10.0f, 1.0f);
}

void GrimMIDIAudioProcessorEditor::resized() {
  auto area = getLocalBounds().reduced(16);
  auto top = area.removeFromTop(44);
  titleLabel_.setBounds(top.removeFromTop(24));
  subtitleLabel_.setBounds(top);
  presetBox_.setBounds(area.removeFromTop(28).removeFromRight(300));

  auto selectors = area.removeFromTop(34);
  const int selW = selectors.getWidth() / 5;
  keyBox_.setBounds(selectors.removeFromLeft(selW).reduced(2));
  scaleBox_.setBounds(selectors.removeFromLeft(selW).reduced(2));
  chordBox_.setBounds(selectors.removeFromLeft(selW).reduced(2));
  inversionBox_.setBounds(selectors.removeFromLeft(selW).reduced(2));
  triggerModeBox_.setBounds(selectors.reduced(2));

  exportDragButton_.setBounds(area.removeFromBottom(32).reduced(60, 0));

  auto knobRow = area.reduced(4, 8);
  const int w = knobRow.getWidth() / 4;
  auto place = [&](juce::Slider& s, juce::Label& l) {
    auto cell = knobRow.removeFromLeft(w);
    l.setBounds(cell.removeFromTop(18));
    s.setBounds(cell);
  };
  place(spreadSlider_, spreadLabel_);
  place(strumSlider_, strumLabel_);
  place(velSlider_, velLabel_);
  place(octaveSlider_, octaveLabel_);
}

void GrimMIDIAudioProcessorEditor::comboBoxChanged(juce::ComboBox* box) {
  auto& apvts = proc_.getAPVTS();
  if (box == &presetBox_) {
    proc_.setCurrentProgram(presetBox_.getSelectedItemIndex());
    return;
  }
  if (box == &keyBox_) {
    if (auto* p = apvts.getParameter(GrimMIDIAudioProcessor::kParamRootKey)) {
      const float v = static_cast<float>(keyBox_.getSelectedItemIndex());
      p->setValueNotifyingHost(p->convertTo0to1(v));
    }
    return;
  }
  if (box == &inversionBox_) {
    if (auto* p = apvts.getParameter(GrimMIDIAudioProcessor::kParamInversion)) {
      const float v = static_cast<float>(inversionBox_.getSelectedItemIndex());
      p->setValueNotifyingHost(p->convertTo0to1(v));
    }
  }
}

juce::File GrimMIDIAudioProcessorEditor::writeProgressionToTempMidi() const {
  juce::MidiFile file;
  if (!proc_.buildProgressionMidiFile(file)) {
    return {};
  }

  const auto presetName = proc_.getProgramName(proc_.getCurrentProgram());
  const auto safe = presetName.replaceCharacters(" \\/:*?\"<>|", "__________");
  auto out = juce::File::getSpecialLocation(juce::File::tempDirectory)
                 .getChildFile("GrimMIDI_" + safe + ".mid");

  juce::FileOutputStream stream(out);
  if (stream.openedOk()) {
    file.writeTo(stream);
    stream.flush();
    return out;
  }
  return {};
}

void GrimMIDIAudioProcessorEditor::buttonClicked(juce::Button* button) {
  if (button != &exportDragButton_) {
    return;
  }

  const auto file = writeProgressionToTempMidi();
  if (!file.existsAsFile()) {
    return;
  }

  juce::StringArray files;
  files.add(file.getFullPathName());
  if (juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, this)) {
    exportDragButton_.setButtonText("Dropped — export again");
  } else {
    exportDragButton_.setButtonText("Drag MIDI to DAW (click + drag)");
  }
}
