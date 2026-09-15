#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/Components/HardwareShellFrame.h"
#include "View/Components/Mpc2000xlLcdDisplay.h"
#include "View/Components/StepSequencerGrid.h"
#include "View/Components/PianoRollGrid.h"
#include "View/Components/MixerBank.h"
#include "View/Components/QLinkSliderStrip.h"
#include "View/Graphics/SevenSegmentDisplay.h"

namespace vmpc::view
{
class HybridMpcModePanel : public juce::Component
{
public:
    explicit HybridMpcModePanel(controller::AppController& controller);

    void resized() override;
    void updateTransportUi(int playingStep, float peakL, float peakR);

private:
    controller::AppController& appController;
    HardwareShellFrame shell;
    Mpc2000xlLcdDisplay lcd;
    SevenSegmentDisplay bpmDisplay;
    StepSequencerGrid stepGrid;
    PianoRollGrid pianoRoll;
    MixerBank mixerBank;
    QLinkSliderStrip qLink;
};
} // namespace vmpc::view
