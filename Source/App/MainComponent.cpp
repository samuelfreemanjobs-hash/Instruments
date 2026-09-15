#include "MainComponent.h"

namespace vmpc::app
{
MainComponent::MainComponent(controller::AppController& controller)
    : appController(controller)
    , electribePanel(controller)
    , mpcPanel(controller)
    , maschinePanel(controller)
    , sessionPanel(controller)
    , patternPanel(controller)
{
    setLookAndFeel(&lookAndFeel);

    titleLabel.setText("VMPC2000XL Hybrid", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredRight);
    titleLabel.setColour(juce::Label::textColourId, lookAndFeel.iceBlue);
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(modeToolbar);

    modeToolbar.onModeChanged([this](vmpc::model::AppMode mode) {
        appController.setAppMode(mode);
    });

    addChildComponent(mpcPanel);
    addChildComponent(maschinePanel);
    addChildComponent(sessionPanel);
    addChildComponent(patternPanel);
    addChildComponent(electribePanel);

    appController.addListener(this);
    showMode(appController.getAppMode());
    modeToolbar.setCurrentMode(appController.getAppMode());

    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    appController.removeListener(this);
    setLookAndFeel(nullptr);
}

void MainComponent::appModeChanged(vmpc::model::AppMode mode)
{
    modeToolbar.setCurrentMode(mode);
    showMode(mode);
}

void MainComponent::showMode(vmpc::model::AppMode mode)
{
    electribePanel.setVisible(false);
    mpcPanel.setVisible(false);
    maschinePanel.setVisible(false);
    sessionPanel.setVisible(false);
    patternPanel.setVisible(false);

    switch (mode)
    {
        case vmpc::model::AppMode::HybridMpc:
            activeModePanel = &mpcPanel;
            break;
        case vmpc::model::AppMode::Maschine:
            activeModePanel = &maschinePanel;
            break;
        case vmpc::model::AppMode::SessionClip:
            activeModePanel = &sessionPanel;
            break;
        case vmpc::model::AppMode::PatternSong:
            activeModePanel = &patternPanel;
            break;
        case vmpc::model::AppMode::Electribe:
        default:
            activeModePanel = &electribePanel;
            break;
    }

    if (activeModePanel != nullptr)
        activeModePanel->setVisible(true);

    resized();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(lookAndFeel.matteBlack);

    auto top = getLocalBounds().removeFromTop(56);
    drawStudioEmblem(g, top.removeFromLeft(56).reduced(8));

    g.setColour(lookAndFeel.walnutBrown);
    g.fillRect(getLocalBounds().removeFromBottom(6));
}

void MainComponent::drawStudioEmblem(juce::Graphics& g, juce::Rectangle<int> area) const
{
    g.setColour(lookAndFeel.walnutBrown.brighter(0.15f));
    g.fillEllipse(area.toFloat());
    g.setColour(lookAndFeel.neonAccent);
    g.drawEllipse(area.toFloat().reduced(2.0f), 1.5f);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText("VMPC", area, juce::Justification::centred);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(8);
    auto header = bounds.removeFromTop(40);
    modeToolbar.setBounds(header.removeFromLeft(header.getWidth() - 200));
    titleLabel.setBounds(header);

    bounds.removeFromTop(8);
    if (activeModePanel != nullptr && activeModePanel->isVisible())
        activeModePanel->setBounds(bounds);
}

void MainComponent::timerCallback()
{
    const int step = appController.getEngine().getPlayingStepForUi();
    const float peakL = appController.getEngine().getPeakL();
    const float peakR = appController.getEngine().getPeakR();

    switch (appController.getAppMode())
    {
        case vmpc::model::AppMode::Electribe:
            electribePanel.updateTransportUi(step);
            break;
        case vmpc::model::AppMode::HybridMpc:
            mpcPanel.updateTransportUi(step, peakL, peakR);
            break;
        default:
            break;
    }
}
} // namespace vmpc::app
