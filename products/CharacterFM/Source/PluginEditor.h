#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class CharacterFMAudioProcessor;

class CharacterFMEditor final : public juce::AudioProcessorEditor
{
public:
    explicit CharacterFMEditor (CharacterFMAudioProcessor&);
    ~CharacterFMEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CharacterFMAudioProcessor& processor_;

    juce::Slider masterSlider_;
    juce::Slider algorithmSlider_;
    juce::Label titleLabel_;
    juce::Label algorithmLabel_;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> masterAttachment_;
    std::unique_ptr<SliderAttachment> algorithmAttachment_;
};
