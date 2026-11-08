#include "PluginEditor.h"

namespace
{
constexpr juce::uint32 kBg = 0xff0a0a0c;
constexpr juce::uint32 kAccent = 0xffcdff00;
} // namespace

ChordEngineAudioProcessorEditor::ChordEngineAudioProcessorEditor(ChordEngineAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processorRef(p)
{
    setSize(480, 160);
}

void ChordEngineAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(kBg));
    g.setColour(juce::Colour(kAccent));
    g.setFont(juce::FontOptions(18.0f));
    g.drawFittedText("CHORD ENGINE — Milestone 001 shell", getLocalBounds().reduced(12),
                     juce::Justification::centred, 2);
}

void ChordEngineAudioProcessorEditor::resized() {}
