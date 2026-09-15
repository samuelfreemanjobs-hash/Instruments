#include "PatternModePanel.h"

namespace vmpc::view
{
PatternModePanel::PatternModePanel(controller::AppController& controller)
    : appController(controller)
{
    songModeBtn.onClick = [this]() {
        appController.getWorkspace().getPatternPlaylist().setSongMode(songModeBtn.getToggleState());
    };
    addAndMakeVisible(songModeBtn);

    for (int i = 0; i < 8; ++i)
    {
        auto* b = patternButtons.add(new juce::TextButton("P" + juce::String(i + 1)));
        b->onClick = [this, i]() {
            appController.getWorkspace().getPatternPlaylist().setSelectedPattern(i);
            repaint();
        };
        addAndMakeVisible(b);
    }
}

void PatternModePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff18141c));
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.drawText("FL Studio — step patterns + playlist arrangement", getLocalBounds().removeFromTop(28),
               juce::Justification::centred);

    const auto& pl = appController.getWorkspace().getPatternPlaylist();
    const int sel = pl.getSelectedPattern();
    auto playlistArea = getLocalBounds().withTrimmedTop(80).withTrimmedBottom(48).reduced(20);

    for (size_t i = 0; i < pl.getPlaylist().size(); ++i)
    {
        const auto& block = pl.getPlaylist()[i];
        const float x = playlistArea.getX() + static_cast<float>(block.startBeat * 12.0);
        const float w = static_cast<float>(block.lengthBeats * 12.0);
        g.setColour(juce::Colour::fromHSV(static_cast<float>(block.patternIndex) * 0.11f, 0.55f, 0.75f, 1.0f));
        g.fillRoundedRectangle(x, static_cast<float>(playlistArea.getY()), w, static_cast<float>(playlistArea.getHeight()), 4.0f);
    }

    g.setColour(juce::Colour(0xffffc107));
    g.drawText("Selected pattern: " + pl.getPattern(sel).name, getLocalBounds().removeFromBottom(24),
               juce::Justification::centred);
}

void PatternModePanel::resized()
{
    auto top = getLocalBounds().removeFromTop(48).reduced(12);
    songModeBtn.setBounds(top.removeFromRight(80));
    const int w = top.getWidth() / juce::jmax(1, patternButtons.size());
    for (auto* b : patternButtons)
        b->setBounds(top.removeFromLeft(w).reduced(2));
}
} // namespace vmpc::view
