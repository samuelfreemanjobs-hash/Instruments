#include "PluginEditor.h"

namespace
{
constexpr int kEditorW = 720;
constexpr int kEditorH = 420;
} // namespace

Wave909AudioProcessorEditor::Wave909AudioProcessorEditor (Wave909AudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setLookAndFeel (&laf_);
    setSize (kEditorW, kEditorH);

    titleLabel_.setFont (juce::Font (26.0f, juce::Font::bold));
    titleLabel_.setColour (juce::Label::textColourId, juce::Colour (0xfff2f2ff));
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

    addKnob (wave909::ParameterIDs::wtPosition, "Wavetable", 20, 90, 90, 90);
    addKnob (wave909::ParameterIDs::wtScan, "Scan", 120, 90, 90, 90);
    addKnob (wave909::ParameterIDs::filterCutoff, "Cutoff", 220, 90, 90, 90);
    addKnob (wave909::ParameterIDs::filterRes, "Res", 320, 90, 90, 90);
    addKnob (wave909::ParameterIDs::drive, "Drive", 420, 90, 90, 90);
    addKnob (wave909::ParameterIDs::crush, "Crush", 520, 90, 90, 90);
    addKnob (wave909::ParameterIDs::tapeWobble, "Tape", 620, 90, 90, 90);

    addKnob (wave909::ParameterIDs::subLevel, "Sub", 20, 220, 90, 90);
    addKnob (wave909::ParameterIDs::detune, "Detune", 120, 220, 90, 90);
    addKnob (wave909::ParameterIDs::glide, "Glide", 220, 220, 90, 90);
    addKnob (wave909::ParameterIDs::filtEnvAmt, "F.Env", 320, 220, 90, 90);
    addKnob (wave909::ParameterIDs::stereoWidth, "Width", 420, 220, 90, 90);
    addKnob (wave909::ParameterIDs::outputGain, "Output", 520, 220, 90, 90);

    circuitButton_.setClickingTogglesState (true);
    addAndMakeVisible (circuitButton_);
    circuitAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), wave909::ParameterIDs::filterCircuit, circuitButton_);
}

void Wave909AudioProcessorEditor::addKnob (const char* paramId, juce::String label, int x, int y, int w, int h)
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

void Wave909AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0b0b10));
    g.setColour (juce::Colour (0xff242430));
    g.fillRoundedRectangle (8.0f, 8.0f, static_cast<float> (getWidth()) - 16.0f, 70.0f, 8.0f);
    g.setColour (juce::Colour (0xff303040));
    g.drawRoundedRectangle (12.0f, 78.0f, static_cast<float> (getWidth()) - 24.0f, 310.0f, 10.0f, 1.0f);
}

void Wave909AudioProcessorEditor::resized()
{
    titleLabel_.setBounds (24, 16, 300, 32);
    subtitleLabel_.setBounds (24, 44, 400, 20);
    presetBox_.setBounds (getWidth() - 260, 20, 240, 28);
    circuitButton_.setBounds (520, 330, 180, 28);
}

Wave909AudioProcessorEditor::~Wave909AudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}
