#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"

namespace resonance::view
{
class SessionModePanel : public juce::Component
{
public:
    explicit SessionModePanel(controller::AppController& controller);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    controller::AppController& appController;
};
} // namespace resonance::view
