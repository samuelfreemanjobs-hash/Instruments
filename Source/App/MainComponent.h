#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "View/LookAndFeel/VMpcLookAndFeel.h"
#include "View/Components/ModeToolbar.h"
#include "View/Modes/ElectribeModePanel.h"
#include "View/Modes/HybridMpcModePanel.h"
#include "View/Modes/MaschineModePanel.h"
#include "View/Modes/SessionModePanel.h"
#include "View/Modes/PatternModePanel.h"
#include "View/Components/PluginRackPanel.h"
#include "View/Components/MixerBank.h"

namespace vmpc::app
{
class MainComponent : public juce::Component,
                      private juce::Timer,
                      private controller::AppController::Listener
{
public:
    MainComponent(controller::AppController& controller);
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void appModeChanged(vmpc::model::AppMode mode) override;
    void showMode(vmpc::model::AppMode mode);
    void drawStudioEmblem(juce::Graphics& g, juce::Rectangle<int> area) const;

    controller::AppController& appController;
    view::VMpcLookAndFeel lookAndFeel;

    view::ModeToolbar modeToolbar;
    juce::Component* activeModePanel = nullptr;
    view::ElectribeModePanel electribePanel;
    view::HybridMpcModePanel mpcPanel;
    view::MaschineModePanel maschinePanel;
    view::SessionModePanel sessionPanel;
    view::PatternModePanel patternPanel;
    view::PluginRackPanel pluginRack;
    view::MixerBank mixerBank;

    juce::Label titleLabel;
};
} // namespace vmpc::app
