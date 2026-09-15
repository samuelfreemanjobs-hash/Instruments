#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/Electribe/EsxTransportHeader.h"
#include "View/Electribe/ValveForceStrip.h"
#include "View/Electribe/EsxPartsMatrix.h"
#include "View/Electribe/MotionLaneBar.h"
#include "View/Electribe/EsxTouchRibbon.h"
#include "View/Components/StepSequencerGrid.h"

namespace vmpc::view
{
/** Full Electribe ESX hybrid DAW surface (software UI inside VMPC2000XL). */
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

    controller::AppController& appController;
    EsxTransportHeader transport;
    juce::Label oledBanner;
    ValveForceStrip valveForce;
    EsxPartsMatrix partsMatrix;
    juce::Slider cutoff;
    juce::Slider resonance;
    juce::Slider egInt;
    juce::Slider drive;
    MotionLaneBar motionLanes;
    StepSequencerGrid stepRow;
    EsxTouchRibbon touchRibbon;
};
} // namespace vmpc::view
