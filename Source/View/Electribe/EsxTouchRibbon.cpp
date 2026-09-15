#include "EsxTouchRibbon.h"
#include "View/Design/StudioColours.h"

namespace vmpc::view
{
void EsxTouchRibbon::paint(juce::Graphics& g)
{
    g.fillAll(studio::obsidian0());
    g.setColour(juce::Colour(0xff222830));
    g.drawRect(getLocalBounds(), 1);
    const auto x = dot.x * static_cast<float>(getWidth());
    const auto y = dot.y * static_cast<float>(getHeight());
    g.setColour(studio::iceOled());
    g.fillEllipse(x - 5.0f, y - 5.0f, 10.0f, 10.0f);
}

void EsxTouchRibbon::mouseDrag(const juce::MouseEvent& e)
{
    dot.x = juce::jlimit(0.0f, 1.0f, static_cast<float>(e.x) / static_cast<float>(getWidth()));
    dot.y = juce::jlimit(0.0f, 1.0f, static_cast<float>(e.y) / static_cast<float>(getHeight()));
    repaint();
}
} // namespace vmpc::view
