#include "PluginEditor.h"
#include "ParameterIds.h"

namespace huckaby
{

namespace
{
juce::String trackId(int trackIndex, const char* suffix)
{
    return pid::trackField(trackIndex, suffix);
}
} // namespace

PatternGridComponent::PatternGridComponent(HuckabyAudioProcessor& processorIn, int trackIndexIn)
    : processor(processorIn)
    , trackIndex(trackIndexIn)
    , apvts(processorIn.getApvts())
{
    lenLabel.setText("Steps", juce::dontSendNotification);
    addAndMakeVisible(lenLabel);
    addAndMakeVisible(lenSlider);
    lenSlider.setRange(1.0, 64.0, 1.0);
    lenSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lenSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    lenAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, trackId(trackIndex, "len"), lenSlider);

    modeLabel.setText("Bank mode", juce::dontSendNotification);
    addAndMakeVisible(modeLabel);
    addAndMakeVisible(modeBox);
    modeBox.addItem("Static", 1);
    modeBox.addItem("Modulo cycle", 2);
    modeBox.addItem("Random", 3);
    modeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, trackId(trackIndex, "mode"), modeBox);

    staticBankLabel.setText("Static slot", juce::dontSendNotification);
    addAndMakeVisible(staticBankLabel);
    addAndMakeVisible(staticBankSlider);
    staticBankSlider.setRange(0.0, double(kMaxBankSize - 1), 1.0);
    staticBankSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    staticBankSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    staticBankAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, trackId(trackIndex, "bank_static"), staticBankSlider);

    moduloLabel.setText("Modulo", juce::dontSendNotification);
    addAndMakeVisible(moduloLabel);
    addAndMakeVisible(moduloSlider);
    moduloSlider.setRange(1.0, 64.0, 1.0);
    moduloSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    moduloSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    moduloAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, trackId(trackIndex, "mod_div"), moduloSlider);

    slotLabel.setText("Load slot", juce::dontSendNotification);
    addAndMakeVisible(slotLabel);
    addAndMakeVisible(slotSlider);
    slotSlider.setRange(0.0, double(kMaxBankSize - 1), 1.0);
    slotSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    slotSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    slotSlider.setValue(0.0);

    addAndMakeVisible(loadButton);
    loadButton.onClick = [this]() {
        auto chooser = std::make_shared<juce::FileChooser>("Select a WAV file", juce::File {}, "*.wav;*.wave");

        chooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc) mutable {
                const auto f = fc.getResult();
                if (!f.existsAsFile())
                    return;

                processor.requestLoadSample(trackIndex, getSelectedSlotIndex(), f);
            });
    };

    rebuildFromLengthChange();
    startTimerHz(4);
}

int PatternGridComponent::getSelectedSlotIndex() const noexcept
{
    return juce::jlimit(0, kMaxBankSize - 1, juce::roundToInt(slotSlider.getValue()));
}

void PatternGridComponent::rebuildFromLengthChange()
{
    if (auto* raw = apvts.getRawParameterValue(trackId(trackIndex, "len")))
        cachedLengthSteps = juce::jlimit(1, kMaxSteps, juce::roundToInt(raw->load()));
    else
        cachedLengthSteps = 16;

    rebuildStepButtons();
}

void PatternGridComponent::rebuildStepButtons()
{
    stepButtons.clear();

    const int steps = juce::jlimit(1, kMaxSteps, cachedLengthSteps);

    for (int s = 0; s < steps; ++s)
    {
        auto* b = stepButtons.add(new juce::ToggleButton(juce::String(s + 1)));
        addAndMakeVisible(b);
        b->onClick = [this]() {
            processor.setPatternBits(trackIndex, readBitsFromButtons());
        };
    }

    applyBitsToButtons(processor.getPatternBits(trackIndex));
    resized();
}

void PatternGridComponent::applyBitsToButtons(uint64_t bits)
{
    for (int i = 0; i < stepButtons.size(); ++i)
        if (auto* b = stepButtons[i])
            b->setToggleState((bits & (1ull << i)) != 0, juce::dontSendNotification);
}

uint64_t PatternGridComponent::readBitsFromButtons() const
{
    uint64_t bits = 0;
    for (int i = 0; i < stepButtons.size(); ++i)
        if (auto* b = stepButtons[i])
            if (b->getToggleState())
                bits |= (1ull << i);
    return bits;
}

void PatternGridComponent::timerCallback()
{
    const int len = apvts.getRawParameterValue(trackId(trackIndex, "len")) != nullptr
                        ? juce::roundToInt(apvts.getRawParameterValue(trackId(trackIndex, "len"))->load())
                        : 16;

    const int clamped = juce::jlimit(1, kMaxSteps, len);
    if (clamped != cachedLengthSteps)
        rebuildFromLengthChange();
}

void PatternGridComponent::resized()
{
    auto area = getLocalBounds().reduced(8);

    auto row1 = area.removeFromTop(34);
    {
        auto a = row1.removeFromLeft(360);
        lenLabel.setBounds(a.removeFromLeft(70));
        lenSlider.setBounds(a);

        row1.removeFromLeft(16);

        auto b = row1.removeFromLeft(360);
        modeLabel.setBounds(b.removeFromLeft(90));
        modeBox.setBounds(b);
    }

    area.removeFromTop(8);

    auto row2 = area.removeFromTop(34);
    {
        auto a = row2.removeFromLeft(360);
        staticBankLabel.setBounds(a.removeFromLeft(90));
        staticBankSlider.setBounds(a);

        row2.removeFromLeft(16);

        auto b = row2.removeFromLeft(360);
        moduloLabel.setBounds(b.removeFromLeft(90));
        moduloSlider.setBounds(b);
    }

    area.removeFromTop(8);

    auto row3 = area.removeFromTop(34);
    {
        auto a = row3.removeFromLeft(360);
        slotLabel.setBounds(a.removeFromLeft(90));
        slotSlider.setBounds(a);

        row3.removeFromLeft(16);
        loadButton.setBounds(row3.removeFromLeft(420));
    }

    area.removeFromTop(10);

    const int cols = juce::jmin(16, juce::jmax(1, stepButtons.size()));
    const int rows = (stepButtons.size() + cols - 1) / cols;

    const int w = area.getWidth() / juce::jmax(1, cols);
    const int h = juce::jlimit(22, 44, area.getHeight() / juce::jmax(1, rows));

    int x = area.getX();
    int y = area.getY();

    for (int i = 0; i < stepButtons.size(); ++i)
    {
        if (i != 0 && (i % cols) == 0)
        {
            x = area.getX();
            y += h + 6;
        }

        if (auto* b = stepButtons[i])
            b->setBounds(x + 2, y + 2, w - 4, h - 4);

        x += w;
    }
}

HuckabyAudioProcessorEditor::HuckabyAudioProcessorEditor(HuckabyAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processor(p)
    , masterGainAttach(p.getApvts(), "master_gain_db", masterGainSlider)
    , internalBpmAttach(p.getApvts(), "internal_bpm", bpmSlider)
    , internalPlayAttach(p.getApvts(), "internal_play", internalPlayToggle)
    , useInternalTransportAttach(p.getApvts(), "internal_transport", internalTransportToggle)
    , tabs(juce::TabbedButtonBar::TabsAtTop)
{
    setSize(980, 620);

    masterGainLabel.setText("Gain", juce::dontSendNotification);
    masterGainLabel.attachToComponent(&masterGainSlider, true);
    addAndMakeVisible(masterGainSlider);
    masterGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 22);

    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmLabel.attachToComponent(&bpmSlider, true);
    addAndMakeVisible(bpmSlider);
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 22);

    addAndMakeVisible(internalPlayToggle);
    addAndMakeVisible(internalTransportToggle);

    addAndMakeVisible(resetButton);
    resetButton.onClick = [this]() { processor.resetTransportCounters(); };

    for (int t = 0; t < kNumTracks; ++t)
    {
        grids[static_cast<size_t>(t)] = std::make_unique<PatternGridComponent>(processor, t);
        tabs.addTab("Track " + juce::String(t + 1), juce::Colours::darkgrey, grids[static_cast<size_t>(t)].get(), false);
    }

    addAndMakeVisible(tabs);
}

void HuckabyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
}

void HuckabyAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(12);

    auto top = area.removeFromTop(44);
    masterGainSlider.setBounds(top.removeFromLeft(260).reduced(44, 0));
    top.removeFromLeft(16);
    bpmSlider.setBounds(top.removeFromLeft(260).reduced(44, 0));
    top.removeFromLeft(16);
    internalTransportToggle.setBounds(top.removeFromLeft(190));
    internalPlayToggle.setBounds(top.removeFromLeft(120));
    resetButton.setBounds(top.removeFromLeft(140));

    tabs.setBounds(area);
}

} // namespace huckaby
