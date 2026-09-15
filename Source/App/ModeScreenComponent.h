#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "Model/AppMode.h"
#include "View/LookAndFeel/VMpcLookAndFeel.h"
#include "View/Modes/ElectribeModePanel.h"
#include "View/Modes/HybridMpcModePanel.h"
#include "View/Modes/MaschineModePanel.h"
#include "View/Modes/SessionModePanel.h"
#include "View/Modes/PatternModePanel.h"

namespace vmpc::app
{
/** Root content for one mode — one window per mode on multi-monitor setups. */
class ModeScreenComponent : public juce::Component,
                            private juce::Timer
{
public:
    ModeScreenComponent(vmpc::model::AppMode mode, controller::AppController& controller);
    ~ModeScreenComponent() override;

    vmpc::model::AppMode getMode() const noexcept { return appMode; }

    void paint(juce::Graphics& g) override;
    void resized() override;
    void focusGained(juce::Component::FocusChangeType cause) override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    void timerCallback() override;
    void activateModeForAudio();

    vmpc::model::AppMode appMode;
    controller::AppController& appController;
    view::VMpcLookAndFeel lookAndFeel;

    std::unique_ptr<juce::Component> modePanel;
    juce::Label header;
};
} // namespace vmpc::app
