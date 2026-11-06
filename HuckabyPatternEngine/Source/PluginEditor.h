#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

namespace huckaby
{

class PatternGridComponent final : public juce::Component,
                                   private juce::Timer
{
public:
    PatternGridComponent(HuckabyAudioProcessor& processorIn, int trackIndexIn);

    void resized() override;
    void timerCallback() override;

    void syncFromProcessor();

private:
    HuckabyAudioProcessor& processor;
    const int trackIndex;

    juce::AudioProcessorValueTreeState& apvts;

    int cachedLengthSteps { -1 };

    juce::Label lenLabel;
    juce::Slider lenSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lenAttach;

    juce::Label modeLabel;
    juce::ComboBox modeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttach;

    juce::Label staticBankLabel;
    juce::Slider staticBankSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> staticBankAttach;

    juce::Label moduloLabel;
    juce::Slider moduloSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> moduloAttach;

    juce::Label patCcLabel;
    juce::Slider patCcSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> patCcAttach;

    juce::Label patChLabel;
    juce::Slider patChSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> patChAttach;

    juce::Label midiFolderLabel;
    juce::TextButton chooseMidiFolderButton { "MIDI folder" };
    juce::TextButton midiPrevButton { "◀ Pat" };
    juce::TextButton midiNextButton { "Pat ▶" };
    juce::TextButton midiReloadButton { "Reload" };
    juce::TextButton exportMidiButton { "Export .mid" };

    juce::OwnedArray<juce::ToggleButton> stepButtons;

    juce::Label slotLabel;
    juce::Slider slotSlider;
    juce::TextButton loadButton { "Load WAV into selected bank slot" };

    int getSelectedSlotIndex() const noexcept;

    void rebuildFromLengthChange();
    void rebuildStepButtons();
    void applyBitsToButtons(uint64_t bits);
    uint64_t readBitsFromButtons() const;

    void updateMidiPatternStatusLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternGridComponent)
};

class HuckabyAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit HuckabyAudioProcessorEditor(HuckabyAudioProcessor&);

    void paint(juce::Graphics&) override;
    void resized() override;

    void syncTrackPatternUi(int trackIndex);

private:
    HuckabyAudioProcessor& processor;

    // Widgets must be declared before their APVTS attachments — C++ initializes members
    // in declaration order; attachments reference sliders/toggles in their ctor.
    juce::Label masterGainLabel;
    juce::Slider masterGainSlider;

    juce::Label bpmLabel;
    juce::Slider bpmSlider;

    juce::ToggleButton internalPlayToggle { "Play" };
    juce::ToggleButton internalTransportToggle { "Internal transport" };

    juce::TextButton resetButton { "Reset counters" };

    juce::AudioProcessorValueTreeState::SliderAttachment masterGainAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment internalBpmAttach;

    juce::AudioProcessorValueTreeState::ButtonAttachment internalPlayAttach;
    juce::AudioProcessorValueTreeState::ButtonAttachment useInternalTransportAttach;

    juce::TabbedComponent tabs;

    std::array<std::unique_ptr<PatternGridComponent>, kNumTracks> grids;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuckabyAudioProcessorEditor)
};

} // namespace huckaby
