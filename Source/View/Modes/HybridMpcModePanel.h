#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/Components/StudioLcdDisplay.h"
#include "View/Components/StepSequencerGrid.h"
#include "View/Components/PianoRollGrid.h"
#include "View/Components/MixerChannelStrip.h"

namespace resonance::view
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
    StudioLcdDisplay lcd;
    StepSequencerGrid stepGrid;
    PianoRollGrid pianoRoll;
    MixerChannelStrip channelStrip { "DRUM 1" };
};
} // namespace resonance::view
