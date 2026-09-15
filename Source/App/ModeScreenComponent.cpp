#include "ModeScreenComponent.h"

namespace vmpc::app
{
ModeScreenComponent::ModeScreenComponent(vmpc::model::AppMode mode, controller::AppController& controller)
    : appMode(mode)
    , appController(controller)
{
    setWantsKeyboardFocus(true);
    setLookAndFeel(&lookAndFeel);

    header.setText("VMPC2000XL — " + vmpc::model::appModeToString(mode), juce::dontSendNotification);
    header.setJustificationType(juce::Justification::centredLeft);
    header.setColour(juce::Label::textColourId, lookAndFeel.iceBlue);
    addAndMakeVisible(header);

    switch (mode)
    {
        case vmpc::model::AppMode::Electribe:
            modePanel = std::make_unique<view::ElectribeModePanel>(controller);
            break;
        case vmpc::model::AppMode::HybridMpc:
            modePanel = std::make_unique<view::HybridMpcModePanel>(controller);
            break;
        case vmpc::model::AppMode::Maschine:
            modePanel = std::make_unique<view::MaschineModePanel>(controller);
            break;
        case vmpc::model::AppMode::SessionClip:
            modePanel = std::make_unique<view::SessionModePanel>(controller);
            break;
        case vmpc::model::AppMode::PatternSong:
            modePanel = std::make_unique<view::PatternModePanel>(controller);
            break;
    }

    if (modePanel != nullptr)
        addAndMakeVisible(modePanel.get());

    startTimerHz(30);
}

ModeScreenComponent::~ModeScreenComponent()
{
    setLookAndFeel(nullptr);
}

void ModeScreenComponent::activateModeForAudio()
{
    appController.setAppMode(appMode);
}

void ModeScreenComponent::focusGained(juce::Component::FocusChangeType cause)
{
    juce::Component::focusGained(cause);
    activateModeForAudio();
}

void ModeScreenComponent::mouseDown(const juce::MouseEvent& e)
{
    juce::Component::mouseDown(e);
    grabKeyboardFocus();
    activateModeForAudio();
}

void ModeScreenComponent::paint(juce::Graphics& g)
{
    g.fillAll(lookAndFeel.matteBlack);
    g.setColour(lookAndFeel.walnutBrown);
    g.fillRect(getLocalBounds().removeFromBottom(4));
}

void ModeScreenComponent::resized()
{
    auto bounds = getLocalBounds();
    header.setBounds(bounds.removeFromTop(28).reduced(10, 4));
    if (modePanel != nullptr)
        modePanel->setBounds(bounds.reduced(4));
}

void ModeScreenComponent::timerCallback()
{
    const int step = appController.getEngine().getPlayingStepForUi();
    const float peakL = appController.getEngine().getPeakL();
    const float peakR = appController.getEngine().getPeakR();

    if (appMode == vmpc::model::AppMode::Electribe)
    {
        if (auto* p = dynamic_cast<view::ElectribeModePanel*>(modePanel.get()))
            p->updateTransportUi(step);
    }
    else if (appMode == vmpc::model::AppMode::HybridMpc)
    {
        if (auto* p = dynamic_cast<view::HybridMpcModePanel*>(modePanel.get()))
            p->updateTransportUi(step, peakL, peakR);
    }
}
} // namespace vmpc::app
