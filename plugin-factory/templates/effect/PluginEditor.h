#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

class {{PLUGIN_CAMEL}}AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit {{PLUGIN_CAMEL}}AudioProcessorEditor({{PLUGIN_CAMEL}}AudioProcessor&);
    ~{{PLUGIN_CAMEL}}AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    {{PLUGIN_CAMEL}}AudioProcessor& processorRef;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    juce::Slider driveSlider;
    juce::Slider lfoRateSlider;
    juce::Slider lfoDepthSlider;
    juce::Slider mixSlider;
    juce::Slider outputSlider;

    juce::Label cutoffLabel;
    juce::Label resonanceLabel;
    juce::Label driveLabel;
    juce::Label lfoRateLabel;
    juce::Label lfoDepthLabel;
    juce::Label mixLabel;
    juce::Label outputLabel;

    std::unique_ptr<SliderAttachment> cutoffAttachment;
    std::unique_ptr<SliderAttachment> resonanceAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> lfoRateAttachment;
    std::unique_ptr<SliderAttachment> lfoDepthAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR({{PLUGIN_CAMEL}}AudioProcessorEditor)
};
