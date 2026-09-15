#include "ElectribeModePanel.h"
#include "View/LookAndFeel/StudioLookAndFeel.h"

namespace resonance::view
{
ElectribeModePanel::ElectribeModePanel(controller::AppController& controller)
    : appController(controller)
{
    stepRow.setLayout(StepSequencerGrid::Layout::Row16Electribe);

    partLabel.setJustificationType(juce::Justification::centredLeft);
    partLabel.setColour(juce::Label::textColourId, juce::Colour(0xff6ec8e8));
    addAndMakeVisible(partLabel);
    addAndMakeVisible(lcd);
    addAndMakeVisible(stepRow);

    hintLabel.setText("Electribe mode — step sequencer is primary. Maschine / Session / Pattern in toolbar.",
                      juce::dontSendNotification);
    hintLabel.setFont(juce::FontOptions(11.0f));
    hintLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.45f));
    addAndMakeVisible(hintLabel);

    for (int i = 0; i < resonance::model::ElectribeSong::kNumParts; ++i)
    {
        auto* b = partButtons.add(new juce::TextButton(juce::String(i + 1)));
        b->setClickingTogglesState(true);
        b->setRadioGroupId(7000);
        b->onClick = [this, i]() {
            appController.getWorkspace().getElectribeSong().setSelectedPart(i);
            refreshFromModel();
        };
        addAndMakeVisible(b);
    }

    motionBtn.setClickingTogglesState(true);
    motionBtn.onClick = [this]() {
        auto& song = appController.getWorkspace().getElectribeSong();
        song.setMotionSeqEnabled(motionBtn.getToggleState());
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

    stepRow.onStepToggled([this](int step, bool active) {
        auto& song = appController.getWorkspace().getElectribeSong();
        auto& part = song.getPart(song.getSelectedPart());
        part.steps[static_cast<size_t>(step)].active = active;
        lcd.setStatusLine(part.name + " step " + juce::String(step + 1) + (active ? " ON" : " OFF"));
    });

    refreshFromModel();
}

void ElectribeModePanel::syncStepGridFromPart()
{
    const auto& song = appController.getWorkspace().getElectribeSong();
    const auto& part = song.getPart(song.getSelectedPart());
    resonance::model::SixteenStepPattern pattern;
    for (int i = 0; i < 16; ++i)
    {
        pattern.getStep(i).active = part.steps[static_cast<size_t>(i)].active;
        pattern.getStep(i).velocity = part.steps[static_cast<size_t>(i)].velocity;
    }
    stepRow.setPattern(pattern);
    partLevel.setValue(part.level, juce::dontSendNotification);
    partLabel.setText(part.name, juce::dontSendNotification);
}

void ElectribeModePanel::syncPartButtons()
{
    const int sel = appController.getWorkspace().getElectribeSong().getSelectedPart();
    for (int i = 0; i < partButtons.size(); ++i)
    {
        const auto& part = appController.getWorkspace().getElectribeSong().getPart(i);
        partButtons[i]->setToggleState(i == sel, juce::dontSendNotification);
        partButtons[i]->setColour(juce::TextButton::buttonColourId,
                                  part.muted ? juce::Colour(0xff2a2020) : juce::Colour(0xff1e2830));
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
    lcd.setBounds(bounds.removeFromTop(100).reduced(0, 4));
    stepRow.setBounds(bounds.removeFromTop(bounds.getHeight() / 2).reduced(0, 8));

    auto bottom = bounds;
    partLabel.setBounds(bottom.removeFromTop(20));
    auto partRow = bottom.removeFromTop(36);
    const int pw = partRow.getWidth() / juce::jmax(1, partButtons.size());
    for (auto* b : partButtons)
        b->setBounds(partRow.removeFromLeft(pw).reduced(2));

    auto controls = bottom;
    motionBtn.setBounds(controls.removeFromLeft(80).reduced(2));
    partLevel.setBounds(controls.removeFromRight(72).reduced(4));
}
} // namespace resonance::view
