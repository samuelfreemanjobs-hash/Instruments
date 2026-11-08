#include "PluginEditor.h"

namespace
{
void styleRotary(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
}

void bindLabel(juce::Label& label, const juce::String& text, juce::Component& target)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&target, false);
}
} // namespace

MyFirstPluginAudioProcessorEditor::MyFirstPluginAudioProcessorEditor(MyFirstPluginAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p)
{
    for (auto* slider : { &cutoffSlider, &resonanceSlider, &driveSlider, &lfoRateSlider,
                          &lfoDepthSlider, &mixSlider, &outputSlider })
    {
        styleRotary(*slider);
        addAndMakeVisible(*slider);
    }

    bindLabel(cutoffLabel, "Cutoff", cutoffSlider);
    bindLabel(resonanceLabel, "Resonance", resonanceSlider);
    bindLabel(driveLabel, "Drive", driveSlider);
    bindLabel(lfoRateLabel, "LFO Rate", lfoRateSlider);
    bindLabel(lfoDepthLabel, "LFO Depth", lfoDepthSlider);
    bindLabel(mixLabel, "Mix", mixSlider);
    bindLabel(outputLabel, "Output", outputSlider);

    auto& apvts = processorRef.getApvts();

    cutoffAttachment = std::make_unique<SliderAttachment>(apvts, std::string(ParamIDs::cutoffHz), cutoffSlider);
    resonanceAttachment = std::make_unique<SliderAttachment>(apvts, std::string(ParamIDs::resonance), resonanceSlider);
    driveAttachment = std::make_unique<SliderAttachment>(apvts, std::string(ParamIDs::drive), driveSlider);
    lfoRateAttachment = std::make_unique<SliderAttachment>(apvts, std::string(ParamIDs::lfoRateHz), lfoRateSlider);
    lfoDepthAttachment = std::make_unique<SliderAttachment>(apvts, std::string(ParamIDs::lfoDepth), lfoDepthSlider);
    mixAttachment = std::make_unique<SliderAttachment>(apvts, std::string(ParamIDs::mix), mixSlider);
    outputAttachment = std::make_unique<SliderAttachment>(apvts, std::string(ParamIDs::outputGainDb), outputSlider);

    setSize(720, 280);
}

MyFirstPluginAudioProcessorEditor::~MyFirstPluginAudioProcessorEditor() = default;

void MyFirstPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawFittedText("MyFirstPlugin", getLocalBounds().removeFromTop(36), juce::Justification::centred, 1);
}

void MyFirstPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(12).withTrimmedTop(40);
    const int sliderWidth = area.getWidth() / 7;

    juce::Slider* sliders[] = { &cutoffSlider, &resonanceSlider, &driveSlider, &lfoRateSlider,
                                &lfoDepthSlider, &mixSlider, &outputSlider };

    for (auto* slider : sliders)
        slider->setBounds(area.removeFromLeft(sliderWidth).reduced(4));
}
