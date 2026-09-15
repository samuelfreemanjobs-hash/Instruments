#include "MotionLaneBar.h"
#include "View/Design/StudioColours.h"

namespace vmpc::view
{
static void drawLane(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& label, juce::Colour colour)
{
    g.setColour(studio::obsidian0());
    g.fillRect(area);
    g.setColour(juce::Colour(0xff222830));
    g.drawRect(area, 1);
    g.setColour(juce::Colours::white.withAlpha(0.45f));
    g.setFont(8.0f);
    g.drawText(label, area.reduced(4, 0), juce::Justification::centredLeft);

    juce::Path path;
    const float w = static_cast<float>(area.getWidth());
    const float h = static_cast<float>(area.getHeight());
    const float x0 = static_cast<float>(area.getX());
    const float y0 = static_cast<float>(area.getY());
    path.startNewSubPath(x0 + w * 0.05f, y0 + h * 0.55f);
    path.quadraticTo(x0 + w * 0.35f, y0 + h * 0.25f, x0 + w * 0.55f, y0 + h * 0.5f);
    path.quadraticTo(x0 + w * 0.75f, y0 + h * 0.35f, x0 + w * 0.95f, y0 + h * 0.45f);
    g.setColour(colour);
    g.strokePath(path, juce::PathStrokeType(1.5f));
}

void MotionLaneBar::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    const int h = area.getHeight() / 3;
    drawLane(g, area.removeFromTop(h), "PITCH", studio::iceOled());
    drawLane(g, area.removeFromTop(h), "CUTOFF", studio::iceDim());
    drawLane(g, area, "PAN", studio::amberAccent());
}
} // namespace vmpc::view
