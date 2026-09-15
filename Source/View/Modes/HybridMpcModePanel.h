#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/Components/Mpc2000xlLcdDisplay.h"
#include "View/Components/StepSequencerGrid.h"
#include "View/Components/PianoRollGrid.h"
#include "View/Components/MixerChannelStrip.h"

namespace vmpc::view
{
class HybridMpcModePanel : public juce::Component
{
public:
    explicit HybridMpcModePanel(controller::AppController& controller);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void updateTransportUi(int playingStep, float peakL, float peakR);

private:
    controller::AppController& appController;
    Mpc2000xlLcdDisplay lcd;
    StepSequencerGrid stepGrid;
    PianoRollGrid pianoRoll;
    MixerChannelStrip channelStrip { "DRUM 1" };
};
} // namespace vmpc::view
