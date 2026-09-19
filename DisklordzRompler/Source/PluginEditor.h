#pragma once

#include "PluginProcessor.h"

#include <JuceHeader.h>

class RomplerLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    RomplerLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colour (0xff7861ff));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff0be5ff));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff2a3340));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff121b21));
        setColour (juce::Label::textColourId, juce::Colour (0xffd8dce8));
    }
};

class DisklordzRomplerEditor final : public juce::AudioProcessorEditor
{
public:
    explicit DisklordzRomplerEditor (DisklordzRomplerProcessor&);
    ~DisklordzRomplerEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    void addMacroKnob (const char* paramId, juce::String label, int x, int y);

    DisklordzRomplerProcessor& processor_;
    RomplerLookAndFeel laf_;

    juce::ComboBox presetBox_;
    juce::Label title_ { {}, "DISKLORDZ ROMPLER" };
    juce::Label subtitle_ { {}, "Sample-based instrument" };

    std::vector<std::unique_ptr<juce::Slider>> sliders_;
    std::vector<std::unique_ptr<juce::Label>> labels_;
    std::vector<std::unique_ptr<SliderAttachment>> attachments_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisklordzRomplerEditor)
};
