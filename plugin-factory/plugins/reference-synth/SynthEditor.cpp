#include "SynthEditor.h"

MyFirstSynthAudioProcessorEditor::MyFirstSynthAudioProcessorEditor(MyFirstSynthAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p)
{
    for (auto* slider : { &outputSlider, &attackSlider, &decaySlider, &sustainSlider, &releaseSlider })
    {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 16);
        addAndMakeVisible(*slider);
    }

    auto& apvts = processorRef.getApvts();
    outputAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::outputGainDb), outputSlider);
    attackAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::attack), attackSlider);
    decayAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::decay), decaySlider);
    sustainAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::sustain), sustainSlider);
    releaseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::release), releaseSlider);

    setSize(520, 220);
}

MyFirstSynthAudioProcessorEditor::~MyFirstSynthAudioProcessorEditor() = default;

void MyFirstSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawFittedText("MyFirstSynth (VSTi)", getLocalBounds().removeFromTop(32), juce::Justification::centred, 1);
}

void MyFirstSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10).withTrimmedTop(36);
    const int w = area.getWidth() / 5;
    outputSlider.setBounds(area.removeFromLeft(w).reduced(4));
    attackSlider.setBounds(area.removeFromLeft(w).reduced(4));
    decaySlider.setBounds(area.removeFromLeft(w).reduced(4));
    sustainSlider.setBounds(area.removeFromLeft(w).reduced(4));
    releaseSlider.setBounds(area.reduced(4));
}
