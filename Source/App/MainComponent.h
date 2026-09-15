#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/LookAndFeel/VMpcLookAndFeel.h"
#include "View/Components/Mpc2000xlLcdDisplay.h"
#include "View/Components/StepSequencerGrid.h"
#include "View/Components/PianoRollGrid.h"
#include "View/Components/MixerChannelStrip.h"

namespace vmpc::app
{
class MainComponent : public juce::Component,
                      private juce::Timer
{
public:
    MainComponent(controller::AppController& controller);
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawStudioEmblem(juce::Graphics& g, juce::Rectangle<int> area) const;

    controller::AppController& appController;
    view::VMpcLookAndFeel lookAndFeel;

    view::Mpc2000xlLcdDisplay lcd;
    view::StepSequencerGrid stepGrid;
    view::PianoRollGrid pianoRoll;
    view::MixerChannelStrip channelStrip { "DRUM 1" };
    juce::Label titleLabel;
};
} // namespace vmpc::app
