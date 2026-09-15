#include "MainComponent.h"

namespace vmpc::app
{
MainComponent::MainComponent(controller::AppController& controller)
    : appController(controller)
{
    setLookAndFeel(&lookAndFeel);

    titleLabel.setText("VMPC2000XL Modern Hybrid", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, lookAndFeel.iceBlue);
    addAndMakeVisible(titleLabel);

    addAndMakeVisible(lcd);
    addAndMakeVisible(stepGrid);
    addAndMakeVisible(pianoRoll);
    addAndMakeVisible(channelStrip);

    stepGrid.onStepToggled([this](int step, bool active) {
        auto& pattern = appController.getEngine().getSequencer().getPattern();
        pattern.getStep(step).active = active;
        lcd.setStatusLine("STEP " + juce::String(step + 1) + (active ? " ON" : " OFF"));
    });

    stepGrid.setPattern(appController.getEngine().getSequencer().getPattern());
    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(lookAndFeel.matteBlack);

    auto top = getLocalBounds().removeFromTop(56);
    drawStudioEmblem(g, top.removeFromLeft(56).reduced(8));

    g.setColour(lookAndFeel.walnutBrown);
    g.fillRect(getLocalBounds().removeFromBottom(8));
}

void MainComponent::drawStudioEmblem(juce::Graphics& g, juce::Rectangle<int> area) const
{
    g.setColour(lookAndFeel.walnutBrown.brighter(0.15f));
    g.fillEllipse(area.toFloat());
    g.setColour(lookAndFeel.neonAccent);
    g.drawEllipse(area.toFloat().reduced(2.0f), 1.5f);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("VMPC", area, juce::Justification::centred);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(12);
    bounds.removeFromTop(48);
    titleLabel.setBounds(bounds.removeFromTop(24));

    auto topRow = bounds.removeFromTop(bounds.getHeight() / 2);
    lcd.setBounds(topRow.removeFromLeft(topRow.getWidth() * 2 / 3).reduced(4));
    stepGrid.setBounds(topRow.reduced(4));

    auto bottom = bounds;
    channelStrip.setBounds(bottom.removeFromRight(72).reduced(4));
    pianoRoll.setBounds(bottom.reduced(4));
}

void MainComponent::timerCallback()
{
    const int step = appController.getEngine().getSequencer().getPlayingStepForUi();
    stepGrid.setPlayingStep(step);
    channelStrip.meterUpdate(juce::jmax(appController.getEngine().getPeakL(),
                                        appController.getEngine().getPeakR()));
}
} // namespace vmpc::app
