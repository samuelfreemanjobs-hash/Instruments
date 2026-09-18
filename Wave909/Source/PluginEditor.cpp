#include "PluginEditor.h"

namespace
{
constexpr int kEditorW = 900;
constexpr int kEditorH = 520;
} // namespace

Wave909AudioProcessorEditor::Wave909AudioProcessorEditor (Wave909AudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setLookAndFeel (&laf_);
    setResizable (true, true);
    setResizeLimits (720, 480, 1280, 800);
    setSize (kEditorW, kEditorH);

    titleLabel_.setFont (juce::FontOptions (26.0f).withStyle ("Bold"));
    titleLabel_.setColour (juce::Label::textColourId, juce::Colour (0xfff2f2ff));
    addAndMakeVisible (titleLabel_);
    addAndMakeVisible (subtitleLabel_);
    addAndMakeVisible (ampSection_);
    addAndMakeVisible (filtSection_);

    presetBox_.addItemList (juce::StringArray(), 1);
    for (int i = 0; i < p.getNumPrograms(); ++i)
        presetBox_.addItem (p.getProgramName (i), i + 1);
    presetBox_.setSelectedId (p.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox_.onChange = [this]
    {
        const int idx = presetBox_.getSelectedId() - 1;
        processor_.applyFactoryPreset (idx);
        lastProgramSeen_ = idx;
    };
    addAndMakeVisible (presetBox_);

    addKnob (wave909::ParameterIDs::wtPosition, "Wavetable", 20, 90, 88, 88);
    addKnob (wave909::ParameterIDs::wtScan, "Scan", 115, 90, 88, 88);
    addKnob (wave909::ParameterIDs::filterCutoff, "Cutoff", 210, 90, 88, 88);
    addKnob (wave909::ParameterIDs::filterRes, "Res", 305, 90, 88, 88);
    addKnob (wave909::ParameterIDs::drive, "Drive", 400, 90, 88, 88);
    addKnob (wave909::ParameterIDs::crush, "Crush", 495, 90, 88, 88);
    addKnob (wave909::ParameterIDs::tapeWobble, "Tape", 590, 90, 88, 88);

    addKnob (wave909::ParameterIDs::subLevel, "Sub", 20, 210, 88, 88);
    addKnob (wave909::ParameterIDs::detune, "Detune", 115, 210, 88, 88);
    addKnob (wave909::ParameterIDs::glide, "Glide", 210, 210, 88, 88);
    addKnob (wave909::ParameterIDs::filtEnvAmt, "F.Env", 305, 210, 88, 88);
    addKnob (wave909::ParameterIDs::stereoWidth, "Width", 400, 210, 88, 88);
    addKnob (wave909::ParameterIDs::outputGain, "Output", 495, 210, 88, 88);

    addKnob (wave909::ParameterIDs::ampAttack, "A", 20, 340, 72, 72);
    addKnob (wave909::ParameterIDs::ampDecay, "D", 95, 340, 72, 72);
    addKnob (wave909::ParameterIDs::ampSustain, "S", 170, 340, 72, 72);
    addKnob (wave909::ParameterIDs::ampRelease, "R", 245, 340, 72, 72);

    addKnob (wave909::ParameterIDs::filtAttack, "A", 420, 340, 72, 72);
    addKnob (wave909::ParameterIDs::filtDecay, "D", 495, 340, 72, 72);
    addKnob (wave909::ParameterIDs::filtSustain, "S", 570, 340, 72, 72);
    addKnob (wave909::ParameterIDs::filtRelease, "R", 645, 340, 72, 72);

    circuitButton_.setClickingTogglesState (true);
    addAndMakeVisible (circuitButton_);
    circuitAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), wave909::ParameterIDs::filterCircuit, circuitButton_);

    lastProgramSeen_ = processor_.getCurrentProgram();
    startTimerHz (8);
}

void Wave909AudioProcessorEditor::timerCallback()
{
    const int prog = processor_.getCurrentProgram();
    if (prog != lastProgramSeen_)
    {
        lastProgramSeen_ = prog;
        presetBox_.setSelectedId (prog + 1, juce::dontSendNotification);
    }
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
    lab->setBounds (x, y + h - 2, w, 16);
    addAndMakeVisible (*lab);
    labels_.push_back (std::move (lab));
}

void Wave909AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0b0b10));
    g.setColour (juce::Colour (0xff242430));
    g.fillRoundedRectangle (8.0f, 8.0f, static_cast<float> (getWidth()) - 16.0f, 72.0f, 8.0f);
    g.setColour (juce::Colour (0xff303040));
    g.drawRoundedRectangle (12.0f, 78.0f, static_cast<float> (getWidth()) - 24.0f, 410.0f, 10.0f, 1.0f);
}

void Wave909AudioProcessorEditor::resized()
{
    titleLabel_.setBounds (24, 16, 320, 32);
    subtitleLabel_.setBounds (24, 44, 420, 20);
    presetBox_.setBounds (getWidth() - 280, 20, 260, 28);
    ampSection_.setBounds (20, 318, 80, 18);
    filtSection_.setBounds (420, 318, 120, 18);
    circuitButton_.setBounds (getWidth() - 220, 460, 200, 28);
}

Wave909AudioProcessorEditor::~Wave909AudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}
