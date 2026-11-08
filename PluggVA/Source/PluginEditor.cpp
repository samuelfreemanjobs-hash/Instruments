#include "PluginEditor.h"

namespace
{
constexpr int kEditorW = 760;
constexpr int kEditorH = 440;
} // namespace

PluggVAAudioProcessorEditor::PluggVAAudioProcessorEditor (PluggVAAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setLookAndFeel (&laf_);
    setSize (kEditorW, kEditorH);

    titleLabel_.setFont (juce::Font (26.0f, juce::Font::bold));
    titleLabel_.setColour (juce::Label::textColourId, juce::Colour (0xfff0e8ff));
    addAndMakeVisible (titleLabel_);
    addAndMakeVisible (subtitleLabel_);

    presetBox_.addItemList (juce::StringArray(), 1);
    for (int i = 0; i < p.getNumPrograms(); ++i)
        presetBox_.addItem (p.getProgramName (i), i + 1);
    presetBox_.setSelectedId (p.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox_.onChange = [this]
    {
        const int idx = presetBox_.getSelectedId() - 1;
        processor_.applyFactoryPreset (idx);
    };
    addAndMakeVisible (presetBox_);

    using PID = pluggva::ParameterIDs;
    addKnob (PID::filterCutoff, "Cutoff", 20, 100, 88, 88);
    addKnob (PID::filterRes, "Res", 115, 100, 88, 88);
    addKnob (PID::osc1Level, "OSC1", 210, 100, 88, 88);
    addKnob (PID::osc2Level, "OSC2", 305, 100, 88, 88);
    addKnob (PID::osc2Detune, "Detune", 400, 100, 88, 88);
    addKnob (PID::unison, "Unison", 495, 100, 88, 88);
    addKnob (PID::unisonSpread, "Spread", 590, 100, 88, 88);

    addKnob (PID::subLevel, "Sub", 20, 230, 88, 88);
    addKnob (PID::drive, "Drive", 115, 230, 88, 88);
    addKnob (PID::crush, "Crush", 210, 230, 88, 88);
    addKnob (PID::chorus, "Chorus", 305, 230, 88, 88);
    addKnob (PID::drift, "Drift", 400, 230, 88, 88);
    addKnob (PID::glide, "Glide", 495, 230, 88, 88);
    addKnob (PID::filtEnvAmt, "F.Env", 590, 230, 88, 88);
    addKnob (PID::stereoWidth, "Width", 685, 230, 64, 88);
    addKnob (PID::outputGain, "Output", 685, 100, 64, 88);
}

PluggVAAudioProcessorEditor::~PluggVAAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void PluggVAAudioProcessorEditor::addKnob (const char* paramId, juce::String label, int x, int y, int w, int h)
{
    auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox);
    slider->setBounds (x, y, w, h);
    addAndMakeVisible (*slider);
    sliderAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, *slider));
    sliders_.push_back (std::move (slider));

    auto lab = std::make_unique<juce::Label> (label, label);
    lab->setJustificationType (juce::Justification::centred);
    lab->setBounds (x, y + h - 4, w, 18);
    addAndMakeVisible (*lab);
    labels_.push_back (std::move (lab));
}

void PluggVAAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0a080c));
    g.setColour (juce::Colour (0xff221a28));
    g.fillRoundedRectangle (8.0f, 8.0f, static_cast<float> (getWidth()) - 16.0f, 72.0f, 8.0f);
    g.setColour (juce::Colour (0xff352840));
    g.drawRoundedRectangle (12.0f, 88.0f, static_cast<float> (getWidth()) - 24.0f, 330.0f, 10.0f, 1.0f);
}

void PluggVAAudioProcessorEditor::resized()
{
    titleLabel_.setBounds (24, 16, 320, 32);
    subtitleLabel_.setBounds (24, 44, 480, 20);
    presetBox_.setBounds (getWidth() - 300, 22, 280, 28);
}
