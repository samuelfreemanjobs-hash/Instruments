#include "PluginEditor.h"

namespace
{
constexpr int kEditorW = 940;
constexpr int kEditorH = 420;
constexpr int kSidebarW = 300;
} // namespace

Wave9090AudioProcessorEditor::Wave9090AudioProcessorEditor (Wave9090AudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setLookAndFeel (&laf_);
    setSize (kEditorW, kEditorH);
    setResizeLimits (820, 380, 1400, 720);
    setResizable (true, true);

    titleLabel_.setFont (juce::Font (26.0f, juce::Font::bold));
    titleLabel_.setColour (juce::Label::textColourId, juce::Colour (0xfff2f2ff));
    addAndMakeVisible (titleLabel_);
    subtitleLabel_.setText ("200 trap / phonk / drill factory presets", juce::dontSendNotification);
    addAndMakeVisible (subtitleLabel_);

    presetBrowser_ = std::make_unique<PresetBrowserComponent> (
        [] { return static_cast<int> (wave9090::presets::getFactoryPresetCount()); },
        [] (int index)
        {
            const auto& presets = wave9090::presets::getFactoryPresets();
            if (index < 0 || index >= static_cast<int> (presets.size()))
                return juce::String {};
            return juce::String (presets[static_cast<std::size_t> (index)].name.data());
        },
        [] (int index)
        {
            return juce::String (wave9090::presets::getFactoryPresetCategory (static_cast<std::size_t> (index)));
        },
        [this] (int index)
        {
            processor_.setCurrentProgram (index);
        });

    presetBrowser_->setCategoryFilterOptions ({ "TRAP", "PHONK", "DRILL", "BELL", "PAD", "LEAD", "PLUCK", "KEYS" });
    presetBrowser_->setCurrentProgram (p.getCurrentProgram());
    addAndMakeVisible (*presetBrowser_);

    addKnob (wave9090::ParameterIDs::wtPosition, "Wavetable", 20, 90, 90, 90);
    addKnob (wave9090::ParameterIDs::wtScan, "Scan", 120, 90, 90, 90);
    addKnob (wave9090::ParameterIDs::filterCutoff, "Cutoff", 220, 90, 90, 90);
    addKnob (wave9090::ParameterIDs::filterRes, "Res", 320, 90, 90, 90);
    addKnob (wave9090::ParameterIDs::drive, "Drive", 420, 90, 90, 90);
    addKnob (wave9090::ParameterIDs::crush, "Crush", 520, 90, 90, 90);
    addKnob (wave9090::ParameterIDs::tapeWobble, "Tape", 620, 90, 90, 90);

    addKnob (wave9090::ParameterIDs::subLevel, "Sub", 20, 220, 90, 90);
    addKnob (wave9090::ParameterIDs::detune, "Detune", 120, 220, 90, 90);
    addKnob (wave9090::ParameterIDs::glide, "Glide", 220, 220, 90, 90);
    addKnob (wave9090::ParameterIDs::filtEnvAmt, "F.Env", 320, 220, 90, 90);
    addKnob (wave9090::ParameterIDs::stereoWidth, "Width", 420, 220, 90, 90);
    addKnob (wave9090::ParameterIDs::outputGain, "Output", 520, 220, 90, 90);

    circuitButton_.setClickingTogglesState (true);
    addAndMakeVisible (circuitButton_);
    circuitAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), wave9090::ParameterIDs::filterCircuit, circuitButton_);
}

void Wave9090AudioProcessorEditor::addKnob (const char* paramId, juce::String label, int x, int y, int w, int h)
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

void Wave9090AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0b0b10));
    const auto main = getLocalBounds().withTrimmedRight (kSidebarW + 8);
    g.setColour (juce::Colour (0xff242430));
    g.fillRoundedRectangle (main.toFloat().reduced (4.0f).withHeight (70.0f), 8.0f);
    g.setColour (juce::Colour (0xff303040));
    g.drawRoundedRectangle (main.reduced (8).withTrimmedTop (70).toFloat(), 10.0f, 1.0f);
}

void Wave9090AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto sidebar = bounds.removeFromRight (kSidebarW).reduced (6, 8);
    auto main = bounds.reduced (8, 8);

    titleLabel_.setBounds (main.removeFromTop (32).removeFromLeft (360));
    subtitleLabel_.setBounds (main.getX(), 44, main.getWidth(), 20);

    circuitButton_.setBounds (main.getX() + 16, main.getBottom() - 36, 180, 28);

    if (presetBrowser_ != nullptr)
        presetBrowser_->setBounds (sidebar);
}

Wave9090AudioProcessorEditor::~Wave9090AudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}
