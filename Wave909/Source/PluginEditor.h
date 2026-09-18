#pragma once

#include "PluginProcessor.h"

#include <JuceHeader.h>

class Wave909LookAndFeel final : public juce::LookAndFeel_V4
{
public:
    Wave909LookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colour (0xffe8c547));
        setColour (juce::Slider::trackColourId, juce::Colour (0xff2a2a32));
        setColour (juce::Slider::backgroundColourId, juce::Colour (0xff121218));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1a1a22));
        setColour (juce::ComboBox::textColourId, juce::Colours::white);
        setColour (juce::Label::textColourId, juce::Colour (0xffb8b8c8));
    }
};

class Wave909AudioProcessorEditor final : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    explicit Wave909AudioProcessorEditor (Wave909AudioProcessor&);
    ~Wave909AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void addKnob (const char* paramId, juce::String label, int x, int y, int w, int h);
    void timerCallback() override;

    Wave909AudioProcessor& processor_;
    Wave909LookAndFeel laf_;
    int lastProgramSeen_ = -1;

    juce::ComboBox presetBox_;
    juce::Label titleLabel_ { {}, "WAVE-909" };
    juce::Label subtitleLabel_ { {}, "Analog/Digital Hybrid Engine" };
    juce::Label ampSection_ { {}, "AMP" };
    juce::Label filtSection_ { {}, "FILTER ENV" };

    std::vector<std::unique_ptr<juce::Slider>> sliders_;
    std::vector<std::unique_ptr<juce::Label>> labels_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
    std::unique_ptr<ButtonAttachment> circuitAttachment_;
    juce::ToggleButton circuitButton_ { "Destructive Circuit" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave909AudioProcessorEditor)
};
