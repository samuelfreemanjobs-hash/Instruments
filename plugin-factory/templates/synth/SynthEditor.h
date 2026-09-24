#pragma once

#include "SynthProcessor.h"
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

    juce::Slider outputSlider;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR({{PLUGIN_CAMEL}}AudioProcessorEditor)
};
