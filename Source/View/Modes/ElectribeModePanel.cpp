#include "ElectribeModePanel.h"

namespace vmpc::view
{
ElectribeModePanel::ElectribeModePanel(controller::AppController& controller)
    : appController(controller)
{
    stepRow.setLayout(StepSequencerGrid::Layout::Row16Electribe);

    partLabel.setJustificationType(juce::Justification::centredLeft);
    partLabel.setColour(juce::Label::textColourId, juce::Colour(0xff6ec8e8));
    addAndMakeVisible(partLabel);
    addAndMakeVisible(lcd);
    addAndMakeVisible(bpmDisplay);
    addAndMakeVisible(stepDisplay);
    addAndMakeVisible(stepRow);

    hintLabel.setText("Step row = primary. Right-click step = accent. Part # = select, M = mute.",
                      juce::dontSendNotification);
    hintLabel.setFont(juce::FontOptions(11.0f));
    hintLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.45f));
    addAndMakeVisible(hintLabel);

    for (int i = 0; i < vmpc::model::ElectribeSong::kNumParts; ++i)
    {
        auto* b = partButtons.add(new juce::TextButton(juce::String(i + 1)));
        b->setClickingTogglesState(true);
        b->setRadioGroupId(7000);
        b->onClick = [this, i]() {
            appController.getWorkspace().getElectribeSong().setSelectedPart(i);
            refreshFromModel();
        };
        addAndMakeVisible(b);

        auto* m = partMuteButtons.add(new juce::TextButton("M"));
        m->onClick = [this, i]() {
            auto& part = appController.getWorkspace().getElectribeSong().getPart(i);
            part.muted = !part.muted;
            refreshFromModel();
        };
        addAndMakeVisible(m);
    }

    motionBtn.setClickingTogglesState(true);
    motionBtn.onClick = [this]() {
        appController.getWorkspace().getElectribeSong().setMotionSeqEnabled(motionBtn.getToggleState());
    };
    addAndMakeVisible(motionBtn);

    partLevel.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    partLevel.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    partLevel.setRange(0.0, 1.0, 0.01);
    partLevel.onValueChange = [this]() {
        auto& song = appController.getWorkspace().getElectribeSong();
        song.getPart(song.getSelectedPart()).level = static_cast<float>(partLevel.getValue());
    };
    addAndMakeVisible(partLevel);

    swingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    swingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 18);
    swingSlider.setRange(50, 75, 1);
    swingSlider.setValue(appController.getProject().getTree().getProperty("swing", 54));
    swingSlider.onValueChange = [this]() {
        const int swing = static_cast<int>(swingSlider.getValue());
        appController.getProject().getTree().setProperty("swing", swing, nullptr);
        appController.syncProjectToSequencer();
    };
    addAndMakeVisible(swingSlider);

    stepRow.onStepToggled([this](int step, bool active) {
        auto& song = appController.getWorkspace().getElectribeSong();
        auto& part = song.getPart(song.getSelectedPart());
        part.steps[static_cast<size_t>(step)].active = active;
        lcd.setStatusLine(part.name + " step " + juce::String(step + 1) + (active ? " ON" : " OFF"));
    });

    stepRow.onStepAccentToggled([this](int step, bool accent) {
        auto& song = appController.getWorkspace().getElectribeSong();
        song.getPart(song.getSelectedPart()).steps[static_cast<size_t>(step)].accent = accent;
    });

    refreshFromModel();
}

void ElectribeModePanel::syncStepGridFromPart()
{
    const auto& song = appController.getWorkspace().getElectribeSong();
    const auto& part = song.getPart(song.getSelectedPart());
    vmpc::model::SixteenStepPattern pattern;
    for (int i = 0; i < 16; ++i)
    {
        pattern.getStep(i).active = part.steps[static_cast<size_t>(i)].active;
        pattern.getStep(i).velocity = part.steps[static_cast<size_t>(i)].velocity;
        pattern.getStep(i).accent = part.steps[static_cast<size_t>(i)].accent;
    }
    stepRow.setPattern(pattern);
    partLevel.setValue(part.level, juce::dontSendNotification);
    partLabel.setText(part.name, juce::dontSendNotification);

    const int bpm = static_cast<int>(appController.getProject().getTree().getProperty("bpm", 120.0));
    bpmDisplay.setText(juce::String(bpm));
}

void ElectribeModePanel::syncPartButtons()
{
    const int sel = appController.getWorkspace().getElectribeSong().getSelectedPart();
    for (int i = 0; i < partButtons.size(); ++i)
    {
        const auto& part = appController.getWorkspace().getElectribeSong().getPart(i);
        partButtons[i]->setToggleState(i == sel, juce::dontSendNotification);
        partMuteButtons[i]->setColour(juce::TextButton::buttonColourId,
                                      part.muted ? juce::Colour(0xff802020) : juce::Colour(0xff2a3038));
    }
    motionBtn.setToggleState(appController.getWorkspace().getElectribeSong().isMotionSeqEnabled(),
                             juce::dontSendNotification);
}

void ElectribeModePanel::refreshFromModel()
{
    syncStepGridFromPart();
    syncPartButtons();
    lcd.setStatusLine("ELECTRIBE / " + appController.getWorkspace().getElectribeSong().getPart(
                          appController.getWorkspace().getElectribeSong().getSelectedPart())
                          .name);
}

void ElectribeModePanel::updateTransportUi(int playingStep)
{
    stepRow.setPlayingStep(playingStep);
    stepDisplay.setText(juce::String(playingStep + 1).paddedLeft('0', 2));
}

void ElectribeModePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101014));
    g.setColour(juce::Colour(0xff3fffd6).withAlpha(0.15f));
    g.drawRoundedRectangle(getLocalBounds().reduced(6).toFloat(), 6.0f, 1.0f);
}

void ElectribeModePanel::resized()
{
    auto bounds = getLocalBounds().reduced(12);
    hintLabel.setBounds(bounds.removeFromTop(18));

    auto header = bounds.removeFromTop(100);
    bpmDisplay.setBounds(header.removeFromRight(56).reduced(0, 8));
    stepDisplay.setBounds(header.removeFromRight(40).reduced(0, 8));
    lcd.setBounds(header.reduced(0, 4));

    stepRow.setBounds(bounds.removeFromTop(bounds.getHeight() * 58 / 100).reduced(0, 8));

    auto bottom = bounds;
    partLabel.setBounds(bottom.removeFromTop(20));
    swingSlider.setBounds(bottom.removeFromTop(24).reduced(0, 2));

    auto partRow = bottom.removeFromTop(40);
    const int cols = vmpc::model::ElectribeSong::kNumParts;
    const int colW = partRow.getWidth() / cols;
    for (int i = 0; i < cols; ++i)
    {
        auto col = partRow.removeFromLeft(colW).reduced(2);
        partButtons[i]->setBounds(col.removeFromTop(col.getHeight() * 2 / 3));
        partMuteButtons[i]->setBounds(col);
    }

    auto controls = bottom;
    motionBtn.setBounds(controls.removeFromLeft(80).reduced(2));
    partLevel.setBounds(controls.removeFromRight(72).reduced(4));
}
} // namespace vmpc::view
