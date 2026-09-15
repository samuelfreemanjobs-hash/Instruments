#pragma once

#include <JuceHeader.h>
#include "Model/AppMode.h"

namespace vmpc::app
{
inline std::array<vmpc::model::AppMode, 5> allModes()
{
    return { vmpc::model::AppMode::Electribe,
             vmpc::model::AppMode::HybridMpc,
             vmpc::model::AppMode::Maschine,
             vmpc::model::AppMode::SessionClip,
             vmpc::model::AppMode::PatternSong };
}

/** Assign each mode window to a display (round-robin) using ~90% of user area. */
inline juce::Rectangle<int> boundsForModeScreen(int modeIndex)
{
    const auto displays = juce::Desktop::getInstance().getDisplays();
    if (displays.displays.isEmpty())
        return { 80 + modeIndex * 40, 80 + modeIndex * 40, 960, 640 };

    const int displayIndex = modeIndex % displays.displays.size();
    auto area = displays.displays.getReference(displayIndex).userArea;
    area = area.reduced(static_cast<int>(area.getWidth() * 0.05f),
                        static_cast<int>(area.getHeight() * 0.05f));

    const int modesOnThisDisplay = (allModes().size() + displays.displays.size() - 1) / displays.displays.size();
    const int slot = modeIndex / displays.displays.size();
    if (modesOnThisDisplay <= 1 || slot >= modesOnThisDisplay)
        return area;

    const int cols = modesOnThisDisplay <= 2 ? modesOnThisDisplay : 2;
    const int rows = (modesOnThisDisplay + cols - 1) / cols;
    const int col = slot % cols;
    const int row = slot / cols;
    const int w = area.getWidth() / cols;
    const int h = area.getHeight() / rows;
    return { area.getX() + col * w, area.getY() + row * h, w, h };
}
} // namespace vmpc::app
