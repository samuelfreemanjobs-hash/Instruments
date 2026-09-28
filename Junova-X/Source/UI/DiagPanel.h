#pragma once

#include "JunovaLookAndFeel.h"
#include "Parameters/ParameterIds.h"
#include "UiLayout.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

namespace junovax::ui
{
class DiagPanel final : public juce::Component
{
public:
    explicit DiagPanel (JunovaXAudioProcessor& processor);
    ~DiagPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    JunovaXAudioProcessor& processor_;
    JunovaLookAndFeel laf_;

    juce::Label title_ { {}, "Diagnostics" };
    juce::ToggleButton testToneButton_ { "Test tone" };
    juce::Slider freqSlider_;
    juce::TextButton panicButton_ { "Panic (all notes off)" };

    std::unique_ptr<ButtonAttachment> toneAttachment_;
    std::unique_ptr<SliderAttachment> freqAttachment_;
};
} // namespace junovax::ui
