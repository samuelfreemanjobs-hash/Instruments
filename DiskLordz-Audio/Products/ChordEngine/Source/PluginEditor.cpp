#include "PluginEditor.h"

namespace
{
constexpr juce::uint32 kBg = 0xff0a0a0c;
constexpr juce::uint32 kPrimary = 0xff7b2cbf;
constexpr juce::uint32 kAccent = 0xffcdff00;

juce::String keyItem(int pc)
{
    return disklordz::theory::noteNameFromPc(pc);
}
} // namespace

ChordEngineAudioProcessorEditor::ChordEngineAudioProcessorEditor(ChordEngineAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processorRef(p)
{
    setSize(760, 420);

    for (int pc = 0; pc < 12; ++pc)
        keyBox.addItem(keyItem(pc), pc + 1);
    addAndMakeVisible(keyBox);

    scaleBox.addItemList(juce::StringArray{ "Natural minor", "Harmonic minor", "Major", "Phrygian" }, 1);
    addAndMakeVisible(scaleBox);

    const auto& presets = disklordz::chordengine::starterProgressions();
    for (int i = 0; i < disklordz::chordengine::kNumStarterProgressions; ++i)
        presetBox.addItem(presets[static_cast<size_t>(i)].displayName, i + 1);
    addAndMakeVisible(presetBox);

    spreadSlider.setRange(0.0, 1.0, 0.01);
    spreadSlider.setTextValueSuffix(" spread");
    addAndMakeVisible(spreadSlider);

    inversionSlider.setRange(0, 3, 1);
    inversionSlider.setTextValueSuffix(" inv");
    addAndMakeVisible(inversionSlider);

    hostMidiToggle.setButtonText("MIDI to piano roll");
    hostMidiToggle.setTooltip("When on, chords are also sent as MIDI output (stack with FL sampler).");
    addAndMakeVisible(hostMidiToggle);

    auditionToggle.setButtonText("Loop audition");
    addAndMakeVisible(auditionToggle);

    auditionButton.onClick = [this] { processorRef.triggerAudition(); };
    addAndMakeVisible(auditionButton);

    helpLabel.setText(
        "FL Studio: add to Channel rack → pick a progression → hit Play (or Audition). "
        "Built-in sound plays automatically; enable MIDI for Piano roll capture.",
        juce::dontSendNotification);
    helpLabel.setFont(juce::Font(13.0f));
    helpLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(helpLabel);

    for (size_t i = 0; i < chordLabels.size(); ++i)
    {
        chordLabels[i].setJustificationType(juce::Justification::centred);
        chordLabels[i].setFont(juce::Font(16.0f, juce::Font::bold));
        addAndMakeVisible(chordLabels[i]);

        stepButtons[i].setButtonText(juce::String(static_cast<int>(i) + 1));
        stepButtons[i].onClick = [this, i] {
            processorRef.getApvts().getParameter("audition")->setValueNotifyingHost(1.0f);
        };
        addAndMakeVisible(stepButtons[i]);
    }

    auto& apvts = processorRef.getApvts();
    rootAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "root", keyBox);
    scaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "scale", scaleBox);
    presetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "preset", presetBox);
    spreadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "spread", spreadSlider);
    inversionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "inversion", inversionSlider);
    hostMidiAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "sendHostMidi", hostMidiToggle);
    auditionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "audition", auditionToggle);

    refreshChordLabels();
    startTimerHz(15);
}

ChordEngineAudioProcessorEditor::~ChordEngineAudioProcessorEditor() = default;

void ChordEngineAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(kBg));

    g.setColour(juce::Colour(kPrimary));
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("CHORD ENGINE", 16, 8, 300, 28, juce::Justification::centredLeft);

    const int active = processorRef.getPlayheadStep();
    for (int i = 0; i < ChordEngineAudioProcessor::kProgressionLength; ++i)
    {
        auto bounds = chordLabels[static_cast<size_t>(i)].getBounds().expanded(4);
        g.setColour(i == active ? juce::Colour(kAccent) : juce::Colour(kPrimary).withAlpha(0.6f));
        g.drawRoundedRectangle(bounds.toFloat(), 6.0f, i == active ? 2.5f : 1.0f);
    }
}

void ChordEngineAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16);
    auto top = area.removeFromTop(36);
    keyBox.setBounds(top.removeFromLeft(70));
    top.removeFromLeft(8);
    scaleBox.setBounds(top.removeFromLeft(150));
    top.removeFromLeft(8);
    presetBox.setBounds(top.removeFromLeft(280));

    area.removeFromTop(8);
    helpLabel.setBounds(area.removeFromTop(40));

    auto controls = area.removeFromBottom(90);
    spreadSlider.setBounds(controls.removeFromLeft(160).reduced(4));
    inversionSlider.setBounds(controls.removeFromLeft(120).reduced(4));
    hostMidiToggle.setBounds(controls.removeFromLeft(170).reduced(4));
    auditionToggle.setBounds(controls.removeFromLeft(130).reduced(4));
    auditionButton.setBounds(controls.removeFromLeft(110).reduced(4));

    auto grid = area.reduced(0, 8);
    const int w = grid.getWidth() / ChordEngineAudioProcessor::kProgressionLength;
    for (int i = 0; i < ChordEngineAudioProcessor::kProgressionLength; ++i)
    {
        auto col = grid.removeFromLeft(w).reduced(6);
        chordLabels[static_cast<size_t>(i)].setBounds(col.removeFromTop(col.getHeight() - 34));
        stepButtons[static_cast<size_t>(i)].setBounds(col);
    }
}

void ChordEngineAudioProcessorEditor::timerCallback()
{
    refreshChordLabels();
    repaint();
}

void ChordEngineAudioProcessorEditor::refreshChordLabels()
{
    const auto labels = processorRef.getChordSymbolLabels();
    for (size_t i = 0; i < labels.size(); ++i)
        chordLabels[i].setText(labels[i], juce::dontSendNotification);
}