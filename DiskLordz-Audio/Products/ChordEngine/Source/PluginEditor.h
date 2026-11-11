#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class ChordEngineAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit ChordEngineAudioProcessorEditor(ChordEngineAudioProcessor&);
    ~ChordEngineAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ChordEngineAudioProcessor& processorRef;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordEngineAudioProcessorEditor)
};
