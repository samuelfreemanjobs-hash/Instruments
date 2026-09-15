#include "HybridMpcModePanel.h"

namespace vmpc::view
{
HybridMpcModePanel::HybridMpcModePanel(controller::AppController& controller)
    : appController(controller)
{
    addAndMakeVisible(shell);
    auto& host = shell.getContentHost();
    host.addAndMakeVisible(lcd);
    host.addAndMakeVisible(bpmDisplay);
    host.addAndMakeVisible(stepGrid);
    host.addAndMakeVisible(pianoRoll);
    host.addAndMakeVisible(mixerBank);
    host.addAndMakeVisible(qLink);

    const double bpm = appController.getProject().getTree().getProperty("bpm", 120.0);
    bpmDisplay.setText(juce::String(static_cast<int>(bpm)));

    stepGrid.onStepToggled([this](int step, bool active) {
        auto& pattern = appController.getEngine().getSequencer().getPattern();
        pattern.getStep(step).active = active;
        lcd.setStatusLine("STEP " + juce::String(step + 1) + (active ? " ON" : " OFF"));
    });

    stepGrid.onStepAccentToggled([this](int step, bool accent) {
        appController.getEngine().getSequencer().getPattern().getStep(step).accent = accent;
    });

    stepGrid.setPattern(appController.getEngine().getSequencer().getPattern());

    qLink.onValueChanged([this](int link, float value) {
        appController.setQLinkValue(link, value);
    });
}

void HybridMpcModePanel::resized()
{
    shell.setBounds(getLocalBounds());
    auto bounds = shell.getContentHost().getLocalBounds().reduced(8);

    auto header = bounds.removeFromTop(28);
    bpmDisplay.setBounds(header.removeFromRight(72));
    header.removeFromRight(8);

    auto topRow = bounds.removeFromTop(bounds.getHeight() * 55 / 100);
    lcd.setBounds(topRow.removeFromLeft(topRow.getWidth() * 62 / 100).reduced(2));
    stepGrid.setBounds(topRow.reduced(2));

    auto bottom = bounds;
    qLink.setBounds(bottom.removeFromRight(140).reduced(2));
    mixerBank.setBounds(bottom.removeFromRight(220).reduced(2));
    pianoRoll.setBounds(bottom.reduced(2));
}

void HybridMpcModePanel::updateTransportUi(int playingStep, float peakL, float peakR)
{
    stepGrid.setPlayingStep(playingStep);
    for (int i = 0; i < MixerBank::kChannels; ++i)
        mixerBank.meterUpdate(i, juce::jmax(peakL, peakR) * (1.0f - 0.15f * static_cast<float>(i)));
}
} // namespace vmpc::view
