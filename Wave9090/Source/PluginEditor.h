#pragma once

#include "PluginProcessor.h"
#include "UI/LayoutConstants.h"
#include "UI/LabeledControl.h"
#include "UI/Wave9090LookAndFeel.h"

#include <JuceHeader.h>
#include <memory>
#include <vector>

class Wave9090AudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit Wave9090AudioProcessorEditor (Wave9090AudioProcessor&);
    ~Wave9090AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void renderEditorPng (const juce::File& outFile) const;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    struct ParamUiSpec
    {
        const char* paramId;
        juce::String label;
        juce::String designSlot;
        juce::String tooltip;
    };

    void timerCallback() override;
    void buildControls();
    void layoutControls();
    wave9090::ui::LabeledKnob* knobForId (const char* id) noexcept;
    static juce::String formatParamValue (const char* id, float norm);
    static const std::vector<ParamUiSpec>& allUiSpecs();

    Wave9090AudioProcessor& processor_;
    wave9090::ui::Wave9090LookAndFeel laf_;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow_;

    juce::Label titleLabel_;
    juce::ComboBox presetBox_;
    juce::ToggleButton circuitButton_ { "Destructive" };
    juce::MidiKeyboardComponent keyboard_ { processor_.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard };
    juce::Label meterLabel_;
    juce::ProgressBar meterBar_;
    double meterNorm_ = 0.0;

    std::vector<std::unique_ptr<wave9090::ui::LabeledKnob>> knobs_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
    std::unique_ptr<ButtonAttachment> circuitAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave9090AudioProcessorEditor)
};