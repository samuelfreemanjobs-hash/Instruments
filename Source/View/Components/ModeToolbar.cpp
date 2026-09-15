#include "ModeToolbar.h"

namespace resonance::view
{
ModeToolbar::ModeToolbar()
{
    buildButtons();
}

void ModeToolbar::buildButtons()
{
    const std::array modes { resonance::model::AppMode::Electribe,
                             resonance::model::AppMode::HybridMpc,
                             resonance::model::AppMode::Maschine,
                             resonance::model::AppMode::SessionClip,
                             resonance::model::AppMode::PatternSong };

    for (auto m : modes)
    {
        auto* btn = modeButtons.add(new juce::TextButton(resonance::model::appModeToString(m)));
        btn->setClickingTogglesState(true);
        btn->setRadioGroupId(9001);
        btn->onClick = [this, m]() {
            setCurrentMode(m);
            if (modeChanged)
                modeChanged(m);
        };
        addAndMakeVisible(btn);
    }
    setCurrentMode(resonance::model::AppMode::Electribe);
}

void ModeToolbar::setCurrentMode(resonance::model::AppMode mode)
{
    current = mode;
    for (auto* btn : modeButtons)
        btn->setToggleState(btn->getButtonText() == resonance::model::appModeToString(mode), juce::dontSendNotification);
    repaint();
}

void ModeToolbar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0e0e12));
}

void ModeToolbar::resized()
{
    auto area = getLocalBounds().reduced(4);
    const int w = area.getWidth() / juce::jmax(1, modeButtons.size());
    for (auto* btn : modeButtons)
        btn->setBounds(area.removeFromLeft(w).reduced(2));
}
} // namespace resonance::view
