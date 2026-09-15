#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/Components/StepSequencerGrid.h"
#include "View/Components/StudioLcdDisplay.h"

namespace resonance::view
{
/** Electribe mode: 16-step row is the hero; parts + motion seq along the bottom. */
class ElectribeModePanel : public juce::Component
{
public:
    explicit ElectribeModePanel(controller::AppController& controller);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void refreshFromModel();
    void updateTransportUi(int playingStep);

private:
    void syncStepGridFromPart();
    void syncPartButtons();

    controller::AppController& appController;
    StudioLcdDisplay lcd;
    StepSequencerGrid stepRow;
    juce::OwnedArray<juce::TextButton> partButtons;
    juce::TextButton motionBtn { "MOTION" };
    juce::Slider partLevel;
    juce::Label partLabel;
    juce::Label hintLabel;
};
} // namespace resonance::view
