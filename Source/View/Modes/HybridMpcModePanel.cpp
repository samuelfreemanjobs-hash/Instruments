#include "HybridMpcModePanel.h"

namespace vmpc::view
{
HybridMpcModePanel::HybridMpcModePanel(controller::AppController& controller)
    : appController(controller)
{
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
}

void HybridMpcModePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121214));
}

void HybridMpcModePanel::resized()
{
    auto bounds = getLocalBounds().reduced(8);
    auto topRow = bounds.removeFromTop(bounds.getHeight() / 2);
    lcd.setBounds(topRow.removeFromLeft(topRow.getWidth() * 2 / 3).reduced(4));
    stepGrid.setBounds(topRow.reduced(4));

    auto bottom = bounds;
    channelStrip.setBounds(bottom.removeFromRight(72).reduced(4));
    pianoRoll.setBounds(bottom.reduced(4));
}

void HybridMpcModePanel::updateTransportUi(int playingStep, float peakL, float peakR)
{
    stepGrid.setPlayingStep(playingStep);
    channelStrip.meterUpdate(juce::jmax(peakL, peakR));
}
} // namespace vmpc::view
