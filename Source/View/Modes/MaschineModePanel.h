#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"

namespace vmpc::view
{
class MaschineModePanel : public juce::Component
{
public:
    explicit MaschineModePanel(controller::AppController& controller);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    int hitPad(juce::Point<int> pos) const;

    controller::AppController& appController;
    juce::OwnedArray<juce::TextButton> sceneButtons;
    juce::TextButton groupBtn { "GROUP A" };
};
} // namespace vmpc::view
