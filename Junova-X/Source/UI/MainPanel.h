#pragma once

#include "JunovaLookAndFeel.h"
#include "Parameters/ParameterIds.h"
#include "UiLayout.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

namespace junovax::ui
{
class MainPanel final : public juce::Component
{
public:
    explicit MainPanel (JunovaXAudioProcessor& processor);
    ~MainPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void addKnob (const char* paramId, juce::String label, int x, int y);

    JunovaXAudioProcessor& processor_;
    JunovaLookAndFeel laf_;

    juce::Label title_ { {}, "JUNOVA-X" };
    juce::Label subtitle_ { {}, "Main — wire custom GUI assets in UI/UiLayout.h" };

    juce::ComboBox chorusBox_;
    juce::ToggleButton hpfButton_ { "HPF" };

    std::vector<std::unique_ptr<juce::Slider>> sliders_;
    std::vector<std::unique_ptr<juce::Label>> labels_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
    std::unique_ptr<ComboAttachment> chorusAttachment_;
    std::unique_ptr<ButtonAttachment> hpfAttachment_;
};
} // namespace junovax::ui
