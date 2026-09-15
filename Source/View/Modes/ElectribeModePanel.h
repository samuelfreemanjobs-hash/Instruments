#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/Components/StepSequencerGrid.h"
#include "View/Components/Mpc2000xlLcdDisplay.h"
#include "View/Graphics/SevenSegmentDisplay.h"

namespace vmpc::view
{
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
    Mpc2000xlLcdDisplay lcd;
    SevenSegmentDisplay bpmDisplay;
    SevenSegmentDisplay stepDisplay;
    StepSequencerGrid stepRow;
    juce::OwnedArray<juce::TextButton> partButtons;
    juce::OwnedArray<juce::TextButton> partMuteButtons;
    juce::TextButton motionBtn { "MOTION" };
    juce::Slider partLevel;
    juce::Slider swingSlider;
    juce::Label partLabel;
    juce::Label hintLabel;
};
} // namespace vmpc::view
