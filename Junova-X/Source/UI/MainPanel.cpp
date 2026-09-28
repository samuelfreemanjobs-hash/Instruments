#include "MainPanel.h"

#include "PluginProcessor.h"

namespace junovax::ui
{
MainPanel::MainPanel (JunovaXAudioProcessor& processor)
    : processor_ (processor)
{
    setLookAndFeel (&laf_);

    title_.setFont (juce::Font (28.0f, juce::Font::bold));
    addAndMakeVisible (title_);
    addAndMakeVisible (subtitle_);

    hpfButton_.setClickingTogglesState (true);
    addAndMakeVisible (hpfButton_);
    hpfAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), ParameterIDs::hpfEnabled, hpfButton_);

    chorusBox_.addItemList ({ "Off", "I", "II" }, 1);
    addAndMakeVisible (chorusBox_);
    chorusAttachment_ = std::make_unique<ComboAttachment> (processor_.getApvts(), ParameterIDs::chorusMode, chorusBox_);

    const int baseX = Layout::margin;
    const int baseY = Layout::headerHeight + Layout::tabBarHeight + Layout::margin;
    int x = baseX;
    const int y = baseY;
    const int step = Layout::knobSize + Layout::knobGap;

    addKnob (ParameterIDs::filterCutoff, "Cutoff", x, y);
    x += step;
    addKnob (ParameterIDs::filterRes, "Res", x, y);
    x += step;
    addKnob (ParameterIDs::masterGain, "Master", x, y);

    x = baseX;
    const int y2 = y + step + 8;
    addKnob (ParameterIDs::ampAttack, "A.A", x, y2);
    x += step;
    addKnob (ParameterIDs::ampDecay, "A.D", x, y2);
    x += step;
    addKnob (ParameterIDs::ampSustain, "A.S", x, y2);
    x += step;
    addKnob (ParameterIDs::ampRelease, "A.R", x, y2);

    x = baseX;
    const int y3 = y2 + step + 8;
    addKnob (ParameterIDs::filtAttack, "F.A", x, y3);
    x += step;
    addKnob (ParameterIDs::filtDecay, "F.D", x, y3);
    x += step;
    addKnob (ParameterIDs::filtSustain, "F.S", x, y3);
    x += step;
    addKnob (ParameterIDs::filtRelease, "F.R", x, y3);
}

MainPanel::~MainPanel()
{
    setLookAndFeel (nullptr);
}

void MainPanel::addKnob (const char* paramId, juce::String label, int x, int y)
{
    auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    slider->setName (label);
    slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, Layout::knobSize, 16);
    slider->setBounds (x, y, Layout::knobSize, Layout::knobSize + 20);
    addAndMakeVisible (*slider);
    sliderAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, *slider));
    sliders_.push_back (std::move (slider));

    auto lab = std::make_unique<juce::Label> (label, label);
    lab->setJustificationType (juce::Justification::centred);
    lab->setBounds (x, y - 18, Layout::knobSize, 16);
    addAndMakeVisible (*lab);
    labels_.push_back (std::move (lab));
}

void MainPanel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff12121a));
    g.setColour (juce::Colour (0xff252532));
    g.fillRoundedRectangle (8.0f, 8.0f, static_cast<float> (getWidth()) - 16.0f, static_cast<float> (Layout::headerHeight), 8.0f);
    g.setColour (juce::Colour (0xff3a3a50));
    g.drawRoundedRectangle (12.0f, static_cast<float> (Layout::headerHeight + Layout::tabBarHeight),
                            static_cast<float> (getWidth()) - 24.0f,
                            static_cast<float> (getHeight()) - Layout::headerHeight - Layout::tabBarHeight - 12.0f,
                            10.0f, 1.0f);
}

void MainPanel::resized()
{
    title_.setBounds (Layout::margin + 4, 14, 280, 32);
    subtitle_.setBounds (Layout::margin + 4, 44, 420, 20);
    chorusBox_.setBounds (getWidth() - 200, 22, 180, 28);
    hpfButton_.setBounds (getWidth() - 200, 52, 180, 28);
}
} // namespace junovax::ui
