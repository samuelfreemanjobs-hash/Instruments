#pragma once

#include "PluginProcessor.h"

#include <JuceHeader.h>

class PluggVALookAndFeel final : public juce::LookAndFeel_V4
{
public:
    PluggVALookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colour (0xffc45ce0));
        setColour (juce::Slider::trackColourId, juce::Colour (0xff2a2030));
        setColour (juce::Slider::backgroundColourId, juce::Colour (0xff140f18));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1a1420));
        setColour (juce::ComboBox::textColourId, juce::Colours::white);
        setColour (juce::Label::textColourId, juce::Colour (0xffc8b8d0));
    }
};

class PluggVAAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit PluggVAAudioProcessorEditor (PluggVAAudioProcessor&);
    ~PluggVAAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void addKnob (const char* paramId, juce::String label, int x, int y, int w, int h);

    PluggVAAudioProcessor& processor_;
    PluggVALookAndFeel laf_;

    juce::ComboBox presetBox_;
    juce::Label titleLabel_ { {}, "PLUGG VA" };
    juce::Label subtitleLabel_ { {}, "Virtual analog for trap · plugg · phonk · rage" };

    std::vector<std::unique_ptr<juce::Slider>> sliders_;
    std::vector<std::unique_ptr<juce::Label>> labels_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluggVAAudioProcessorEditor)
};
