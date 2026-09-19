#include "PluginEditor.h"

namespace
{
constexpr int kW = 920;
constexpr int kH = 520;
} // namespace

DisklordzRomplerEditor::DisklordzRomplerEditor (DisklordzRomplerProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setLookAndFeel (&laf_);
    setSize (kW, kH);

    title_.setFont (juce::Font (24.0f, juce::Font::bold));
    subtitle_.setFont (juce::Font (13.0f, juce::Font::plain));
    addAndMakeVisible (title_);
    addAndMakeVisible (subtitle_);

    for (int i = 0; i < p.getNumPrograms(); ++i)
        presetBox_.addItem (p.getProgramName (i), i + 1);
    presetBox_.setSelectedId (p.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox_.onChange = [this]
    {
        processor_.applyFactoryPreset (presetBox_.getSelectedId() - 1);
    };
    addAndMakeVisible (presetBox_);

    const char* macros[] = {
        disklordz::rompler::ParameterIDs::macroCharacter,
        disklordz::rompler::ParameterIDs::macroBrightness,
        disklordz::rompler::ParameterIDs::macroDrive,
        disklordz::rompler::ParameterIDs::macroSpace,
        disklordz::rompler::ParameterIDs::macroMovement,
        disklordz::rompler::ParameterIDs::macroWidth,
        disklordz::rompler::ParameterIDs::macroDecay,
        disklordz::rompler::ParameterIDs::macroCrush,
    };
    const char* names[] = { "Character", "Brightness", "Drive", "Space", "Movement", "Width", "Decay", "Crush" };

    for (int i = 0; i < 8; ++i)
        addMacroKnob (macros[i], names[i], 24 + i * 100, 100);

    addMacroKnob (disklordz::rompler::ParameterIDs::tone1, "Tone 1", 24, 240);
    addMacroKnob (disklordz::rompler::ParameterIDs::tone2, "Tone 2", 124, 240);
    addMacroKnob (disklordz::rompler::ParameterIDs::tone3, "Tone 3", 224, 240);
    addMacroKnob (disklordz::rompler::ParameterIDs::tone4, "Tone 4", 324, 240);
    addMacroKnob (disklordz::rompler::ParameterIDs::filterCutoff, "Cutoff", 424, 240);
    addMacroKnob (disklordz::rompler::ParameterIDs::outputGain, "Output", 524, 240);
}

void DisklordzRomplerEditor::addMacroKnob (const char* paramId, juce::String label, int x, int y)
{
    auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox);
    slider->setBounds (x, y, 80, 80);
    addAndMakeVisible (*slider);
    attachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, *slider));
    sliders_.push_back (std::move (slider));

    auto lab = std::make_unique<juce::Label> (label, label);
    lab->setJustificationType (juce::Justification::centred);
    lab->setBounds (x, y + 78, 80, 18);
    addAndMakeVisible (*lab);
    labels_.push_back (std::move (lab));
}

void DisklordzRomplerEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0b0f14));
    g.setColour (juce::Colour (0xff7861ff));
    g.drawRoundedRectangle (8.0f, 8.0f, static_cast<float> (getWidth()) - 16.0f, 72.0f, 8.0f, 1.5f);
    g.setColour (juce::Colour (0xff121b21));
    g.fillRoundedRectangle (16.0f, 88.0f, static_cast<float> (getWidth()) - 32.0f, 340.0f, 10.0f);
    g.setColour (juce::Colour (0xff303848));
    g.drawRoundedRectangle (16.0f, 88.0f, static_cast<float> (getWidth()) - 32.0f, 340.0f, 10.0f, 1.0f);
}

void DisklordzRomplerEditor::resized()
{
    title_.setBounds (24, 16, 360, 28);
    subtitle_.setBounds (24, 42, 360, 20);
    presetBox_.setBounds (getWidth() - 280, 24, 260, 28);
}

DisklordzRomplerEditor::~DisklordzRomplerEditor()
{
    setLookAndFeel (nullptr);
}
