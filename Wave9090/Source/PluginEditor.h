#pragma once

#include "PluginProcessor.h"
#include "UI/LayoutConstants.h"
#include "UI/LabeledControl.h"
#include "UI/LedOutputMeter.h"
#include "UI/Wave9090LookAndFeel.h"
#include "UI/WavetableScope.h"

#include <JuceHeader.h>
#include <array>
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
        juce::String mockupSection;
        juce::String designSlot;
        juce::String knobDomain;
        juce::String tooltip;
    };

    void timerCallback() override;
    void buildControls();
    void layoutControls();
    void layoutKeyboardKeyWidth();
    void refreshPresetCombo();
    void applyOctaveShift (int delta);
    wave9090::ui::LabeledKnob* knobForId (const char* id) noexcept;
    static juce::String formatParamValue (const char* id, float norm);
    static const std::vector<ParamUiSpec>& allUiSpecs();
    void paintEnvCurve (juce::Graphics& g, juce::Rectangle<int> area, juce::Colour c,
                        float a, float d, float s, float r) const;

    Wave9090AudioProcessor& processor_;
    wave9090::ui::Wave9090LookAndFeel laf_;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow_;

    juce::Label brandTitle_;
    juce::Label brandSub_;
    juce::TextButton presetPrev_ { "<" };
    juce::TextButton presetNext_ { ">" };
    juce::ComboBox presetBox_;
    juce::Label tabSynthMain_;
    juce::Label tabFuture1_;
    juce::Label tabFuture2_;

    wave9090::ui::WavetableScope wtScope_;
    wave9090::ui::LedOutputMeter ledMeter_;

    juce::ToggleButton circuitButton_ { "DESTRUCT" };
    juce::MidiKeyboardComponent keyboard_ { processor_.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard };
    std::array<juce::TextButton, 5> octaveButtons_;

    std::vector<std::unique_ptr<wave9090::ui::LabeledKnob>> knobs_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
    std::unique_ptr<ButtonAttachment> circuitAttachment_;

    juce::Rectangle<int> bayOsc1_;
    juce::Rectangle<int> bayVcf1_;
    juce::Rectangle<int> bayEnvAmp_;
    juce::Rectangle<int> bayEnvFilt_;
    juce::Rectangle<int> bayMacro_;

    int octaveShift_ = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave9090AudioProcessorEditor)
};