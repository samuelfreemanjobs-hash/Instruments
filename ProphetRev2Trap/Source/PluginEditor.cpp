#include "PluginEditor.h"

ProphetRev2TrapAudioProcessorEditor::ProphetRev2TrapAudioProcessorEditor(
    ProphetRev2TrapAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor_(p)
{
    setSize(480, 320);

    for (int i = 0; i < processor_.getNumPrograms(); ++i)
        programBox_.addItem(processor_.getProgramName(i), i + 1);
    programBox_.setSelectedId(processor_.getCurrentProgram() + 1, juce::dontSendNotification);
    programBox_.onChange = [this] {
        processor_.setCurrentProgram(programBox_.getSelectedId() - 1);
    };
    addAndMakeVisible(programBox_);

    auto setup = [this](juce::Slider& s, const juce::String& name) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        s.setName(name);
        addAndMakeVisible(s);
    };
    setup(cutoffSlider_, "Cutoff");
    setup(resSlider_, "Res");
    setup(slopSlider_, "Slop");
    setup(glideSlider_, "Glide");

    satBox_.addItem("Sat Off", 1);
    satBox_.addItem("Sat On", 2);
    addAndMakeVisible(satBox_);

    auto& apvts = processor_.getApvts();
    cutoffAttach_ = std::make_unique<Attach>(apvts, "filter_cutoff", cutoffSlider_);
    resAttach_ = std::make_unique<Attach>(apvts, "filter_res", resSlider_);
    slopAttach_ = std::make_unique<Attach>(apvts, "slop", slopSlider_);
    glideAttach_ = std::make_unique<Attach>(apvts, "glide_ms", glideSlider_);
    satAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "saturation_on", satBox_);
}

ProphetRev2TrapAudioProcessorEditor::~ProphetRev2TrapAudioProcessorEditor() = default;

void ProphetRev2TrapAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1410));
    g.setColour(juce::Colours::orange);
    g.setFont(18.0f);
    g.drawText("Prophet Rev2 Trap", getLocalBounds().removeFromTop(36), juce::Justification::centred);
}

void ProphetRev2TrapAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(12);
    area.removeFromTop(40);
    programBox_.setBounds(area.removeFromTop(28).removeFromLeft(200));

    auto row = area.removeFromTop(120);
    const int w = row.getWidth() / 4;
    cutoffSlider_.setBounds(row.removeFromLeft(w).reduced(4));
    resSlider_.setBounds(row.removeFromLeft(w).reduced(4));
    slopSlider_.setBounds(row.removeFromLeft(w).reduced(4));
    glideSlider_.setBounds(row.removeFromLeft(w).reduced(4));

    satBox_.setBounds(area.removeFromTop(32).removeFromLeft(140));
}
