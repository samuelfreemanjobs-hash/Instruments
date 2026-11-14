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

constexpr float kVelOff = 0.05f;
} // namespace

//==============================================================================
StepPianoRollComponent::StepPianoRollComponent(HuckabyAudioProcessor& processorIn, int trackIndexIn)
    : processor(processorIn)
    , trackIndex(trackIndexIn)
{
    setOpaque(true);
    reloadFromProcessor();
}

void StepPianoRollComponent::reloadFromProcessor()
{
    edit = processor.getTrackPatternSnapshot(trackIndex);
    repaint();
}

void StepPianoRollComponent::resized()
{
    repaint();
}

float StepPianoRollComponent::timeToX(int step, int numSteps, juce::Rectangle<float> plot) const noexcept
{
    if (numSteps <= 0)
        return plot.getX();

    const float w = plot.getWidth() / static_cast<float>(numSteps);
    return plot.getX() + static_cast<float>(step) * w;
}

int StepPianoRollComponent::xToStep(float x, int numSteps, juce::Rectangle<float> plot) const noexcept
{
    if (numSteps <= 0)
        return 0;

    const float w = plot.getWidth() / static_cast<float>(numSteps);
    const float rel = (x - plot.getX()) / juce::jmax(1.0f, w);
    return juce::jlimit(0, numSteps - 1, static_cast<int>(std::floor(rel)));
}

float StepPianoRollComponent::yToVelocity(float y, juce::Rectangle<float> plot) const noexcept
{
    const float t = juce::jlimit(0.0f, 1.0f, (y - plot.getY()) / juce::jmax(1.0f, plot.getHeight()));
    return juce::jlimit(kVelOff, 1.0f, 1.0f - t);
}

void StepPianoRollComponent::applyEditAt(int step, bool on, float velocity, bool allowErase)
{
    if (!juce::isPositiveAndBelow(step, kMaxSteps))
        return;

    if (!on && allowErase)
    {
        edit.bits &= ~(1ull << step);
        edit.velocities[static_cast<size_t>(step)] = kVelOff;
        return;
    }

    edit.bits |= (1ull << step);
    edit.velocities[static_cast<size_t>(step)] = juce::jlimit(kVelOff, 1.0f, velocity);
}

void StepPianoRollComponent::commitEdit()
{
    processor.setTrackPatternFromUi(trackIndex, edit);
}

void StepPianoRollComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    g.fillAll(juce::Colour(0xff121212));

    const int numSteps = juce::jlimit(1, kMaxSteps, processor.getApvts().getRawParameterValue(trackId(trackIndex, "len")) != nullptr
                                                ? juce::roundToInt(processor.getApvts().getRawParameterValue(trackId(trackIndex, "len"))->load())
                                                : 16);

    auto plot = bounds.removeFromBottom(bounds.getHeight() - 18.0f);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.drawRoundedRectangle(plot, 4.0f, 1.0f);

    g.setFont(juce::FontOptions(12.0f));
    g.setColour(juce::Colours::grey);
    const juce::String caption = "Pattern / velocity  |  " + juce::String(numSteps) + " steps";
    g.drawText(caption,
               bounds.toNearestInt(),
               juce::Justification::centredLeft);

    for (int s = 0; s <= numSteps; ++s)
    {
        const float x = timeToX(s, numSteps, plot);
        g.setColour(((s % 4) == 0) ? juce::Colour(0xff3a3a3a) : juce::Colour(0xff222222));
        g.drawVerticalLine(juce::roundToInt(x), plot.getY(), plot.getBottom());
    }

    const int ph = processor.getTransportStepForTrack(trackIndex);
    if (ph >= 0 && juce::isPositiveAndBelow(ph, numSteps))
    {
        const float cell = plot.getWidth() / static_cast<float>(numSteps);
        const float px = timeToX(ph, numSteps, plot) + cell * 0.5f;
        g.setColour(juce::Colour(0xffff4444).withAlpha(0.55f));
        g.drawVerticalLine(juce::roundToInt(px), juce::roundToInt(plot.getY()), juce::roundToInt(plot.getBottom()));
    }

    for (int s = 0; s < numSteps; ++s)
    {
        const float x0 = timeToX(s, numSteps, plot);
        const float x1 = timeToX(s + 1, numSteps, plot);
        const float cellW = juce::jmax(2.0f, x1 - x0 - 2.0f);
        const float cx = x0 + 1.0f;

        const float vel = edit.velocities[static_cast<size_t>(s)];
        const bool on = ((edit.bits >> s) & 1ull) != 0;

        if (!on && vel <= kVelOff + 0.001f)
            continue;

        const float h = juce::jmax(2.0f, (vel - kVelOff) / (1.0f - kVelOff) * plot.getHeight() * 0.92f);
        const juce::Rectangle<float> bar(cx, plot.getBottom() - h, cellW, h);

        g.setColour(on ? juce::Colour(0xff4fd1c5) : juce::Colour(0xff555555));
        g.fillRoundedRectangle(bar, 2.0f);
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.drawRoundedRectangle(bar, 2.0f, 1.0f);
    }
}

void StepPianoRollComponent::mouseDown(const juce::MouseEvent& e)
{
    auto plot = getLocalBounds().toFloat().reduced(4.0f).removeFromBottom(getHeight() - 18 - 8);

    const int numSteps = juce::jlimit(1, kMaxSteps, processor.getApvts().getRawParameterValue(trackId(trackIndex, "len")) != nullptr
                                                ? juce::roundToInt(processor.getApvts().getRawParameterValue(trackId(trackIndex, "len"))->load())
                                                : 16);

    const int step = xToStep(static_cast<float>(e.x), numSteps, plot);
    lastStep = step;

    const float vel = yToVelocity(static_cast<float>(e.y), plot);

    if (e.mods.isAltDown())
    {
        applyEditAt(step, false, kVelOff, true);
        commitEdit();
        repaint();
        return;
    }

    const bool currentlyOn = ((edit.bits >> step) & 1ull) != 0;
    const bool turnOff = currentlyOn && e.mods.isShiftDown();

    if (turnOff)
    {
        applyEditAt(step, false, kVelOff, true);
    }
    else
    {
        applyEditAt(step, true, vel, false);
    }

    commitEdit();
    repaint();
}

void StepPianoRollComponent::mouseDrag(const juce::MouseEvent& e)
{
    auto plot = getLocalBounds().toFloat().reduced(4.0f).removeFromBottom(getHeight() - 18 - 8);

    const int numSteps = juce::jlimit(1, kMaxSteps, processor.getApvts().getRawParameterValue(trackId(trackIndex, "len")) != nullptr
                                                ? juce::roundToInt(processor.getApvts().getRawParameterValue(trackId(trackIndex, "len"))->load())
                                                : 16);

    const int step = xToStep(static_cast<float>(e.x), numSteps, plot);
    const float vel = yToVelocity(static_cast<float>(e.y), plot);

    if (step != lastStep)
    {
        applyEditAt(step, true, vel, false);
        lastStep = step;
    }
    else
    {
        applyEditAt(step, true, vel, false);
    }

    commitEdit();
    repaint();
}

void StepPianoRollComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    auto plot = getLocalBounds().toFloat().reduced(4.0f).removeFromBottom(getHeight() - 18 - 8);

    const int numSteps = juce::jlimit(1, kMaxSteps, processor.getApvts().getRawParameterValue(trackId(trackIndex, "len")) != nullptr
                                                ? juce::roundToInt(processor.getApvts().getRawParameterValue(trackId(trackIndex, "len"))->load())
                                                : 16);

    const int step = xToStep(static_cast<float>(e.x), numSteps, plot);

    if (!((edit.bits >> step) & 1ull))
        return;

    const float delta = wheel.deltaY > 0 ? 0.05f : -0.05f;
    const float nv = juce::jlimit(kVelOff, 1.0f, edit.velocities[static_cast<size_t>(step)] + delta);
    applyEditAt(step, true, nv, false);
    commitEdit();
    repaint();
}

//==============================================================================
PatternGridComponent::PatternGridComponent(HuckabyAudioProcessor& processorIn, int trackIndexIn)
    : processor(processorIn)
    , trackIndex(trackIndexIn)
    , apvts(processorIn.getApvts())
    , stepRoll(processorIn, trackIndexIn)
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

    patCcLabel.setText("Pat CC", juce::dontSendNotification);
    addAndMakeVisible(patCcLabel);
    addAndMakeVisible(patCcSlider);
    patCcSlider.setRange(0.0, 127.0, 1.0);
    patCcSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    patCcSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 22);
    patCcAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, trackId(trackIndex, "pat_cc"), patCcSlider);

    patChLabel.setText("Pat ch", juce::dontSendNotification);
    addAndMakeVisible(patChLabel);
    addAndMakeVisible(patChSlider);
    patChSlider.setRange(0.0, 16.0, 1.0);
    patChSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    patChSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 22);
    patChAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, trackId(trackIndex, "pat_ch"), patChSlider);

    midiFolderLabel.setText("", juce::dontSendNotification);
    midiFolderLabel.setJustificationType(juce::Justification::centredLeft);
    midiFolderLabel.setFont(juce::FontOptions(13.0f));
    addAndMakeVisible(midiFolderLabel);

    addAndMakeVisible(chooseMidiFolderButton);
    addAndMakeVisible(midiPrevButton);
    addAndMakeVisible(midiNextButton);
    addAndMakeVisible(midiReloadButton);
    addAndMakeVisible(exportMidiButton);

    rollLabel.setText("Pattern (click/drag blocks, Shift+click = off, Alt+click = erase, wheel = velocity)", juce::dontSendNotification);
    rollLabel.setFont(juce::FontOptions(13.0f));
    addAndMakeVisible(rollLabel);
    addAndMakeVisible(stepRoll);

    chooseMidiFolderButton.onClick = [this]() {
        auto chooser = std::make_shared<juce::FileChooser>("Select MIDI pattern folder",
                                                           processor.getTrackMidiPatternFolder(trackIndex),
                                                           "*");

        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                             [this, chooser](const juce::FileChooser& fc) mutable {
                                 const auto f = fc.getResult();
                                 if (!f.isDirectory())
                                     return;
                                 processor.setTrackMidiPatternFolder(trackIndex, f);
                                 syncFromProcessor();
                             });
    };

    midiPrevButton.onClick = [this]() {
        processor.selectPreviousMidiPattern(trackIndex);
        syncFromProcessor();
    };

    midiNextButton.onClick = [this]() {
        processor.selectNextMidiPattern(trackIndex);
        syncFromProcessor();
    };

    midiReloadButton.onClick = [this]() {
        processor.reloadCurrentMidiPattern(trackIndex);
        syncFromProcessor();
    };

    exportMidiButton.onClick = [this]() {
        auto chooser = std::make_shared<juce::FileChooser>("Export pattern as .mid", juce::File {}, "*.mid");

        chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                             [this, chooser](const juce::FileChooser& fc) mutable {
                                 auto f = fc.getResult();
                                 if (f == juce::File {})
                                     return;

                                 if (!f.getFileName().endsWithIgnoreCase(".mid"))
                                     f = f.withFileExtension("mid");

                                 processor.exportCurrentPatternToMidi(trackIndex, f);
                             });
    };

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
    syncFromProcessor();
    startTimerHz(30);
}

void PatternGridComponent::syncFromProcessor()
{
    stepRoll.reloadFromProcessor();
    updateMidiPatternStatusLabel();
}

void PatternGridComponent::updateMidiPatternStatusLabel()
{
    const auto folder = processor.getTrackMidiPatternFolder(trackIndex);
    const int n = processor.getTrackMidiPatternCount(trackIndex);
    const int idx = processor.getTrackMidiPatternIndex(trackIndex);

    juce::String line;
    if (folder.exists())
        line << folder.getFileName() << " — ";
    else
        line << "(no folder) — ";

    if (n <= 0)
        line << "no .mid";
    else
        line << "pat " << (idx + 1) << "/" << n;

    midiFolderLabel.setText(line, juce::dontSendNotification);
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

    updateMidiPatternStatusLabel();
    stepRoll.reloadFromProcessor();
    resized();
}

void PatternGridComponent::timerCallback()
{
    const int len = apvts.getRawParameterValue(trackId(trackIndex, "len")) != nullptr
                        ? juce::roundToInt(apvts.getRawParameterValue(trackId(trackIndex, "len"))->load())
                        : 16;

    const int clamped = juce::jlimit(1, kMaxSteps, len);
    if (clamped != cachedLengthSteps)
        rebuildFromLengthChange();

    stepRoll.repaint();
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

    auto rowMidiMap = area.removeFromTop(34);
    {
        auto a = rowMidiMap.removeFromLeft(360);
        patCcLabel.setBounds(a.removeFromLeft(90));
        patCcSlider.setBounds(a);

        rowMidiMap.removeFromLeft(16);

        auto b = rowMidiMap.removeFromLeft(360);
        patChLabel.setBounds(b.removeFromLeft(90));
        patChSlider.setBounds(b);
    }

    area.removeFromTop(8);

    auto rowMidiNav = area.removeFromTop(34);
    {
        auto lab = rowMidiNav.removeFromLeft(juce::jmin(420, rowMidiNav.getWidth() - 320));
        midiFolderLabel.setBounds(lab);
        rowMidiNav.removeFromLeft(8);
        chooseMidiFolderButton.setBounds(rowMidiNav.removeFromLeft(100));
        midiPrevButton.setBounds(rowMidiNav.removeFromLeft(56));
        midiNextButton.setBounds(rowMidiNav.removeFromLeft(56));
        midiReloadButton.setBounds(rowMidiNav.removeFromLeft(72));
        exportMidiButton.setBounds(rowMidiNav.removeFromLeft(88));
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

    area.removeFromTop(8);

    rollLabel.setBounds(area.removeFromTop(22));
    stepRoll.setBounds(area);
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
    setSize(980, 760);

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

void HuckabyAudioProcessorEditor::syncTrackPatternUi(int trackIndex)
{
    if (!juce::isPositiveAndBelow(trackIndex, kNumTracks))
        return;

    if (grids[static_cast<size_t>(trackIndex)] != nullptr)
        grids[static_cast<size_t>(trackIndex)]->syncFromProcessor();
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
