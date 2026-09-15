#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"

namespace resonance::view
{
class PatternModePanel : public juce::Component
{
public:
    explicit PatternModePanel(controller::AppController& controller);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    controller::AppController& appController;
    juce::OwnedArray<juce::TextButton> patternButtons;
    juce::ToggleButton songModeBtn { "Song" };
};
} // namespace resonance::view
