#include "LcdPanelComponent.h"

LcdPanelComponent::LcdPanelComponent() = default;

void LcdPanelComponent::setLines (juce::String line1, juce::String line2)
{
    line1_ = std::move (line1);
    line2_ = std::move (line2);
    repaint();
}

void LcdPanelComponent::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (juce::Colour (0xff0a1628));
    g.fillRoundedRectangle (r, 4.0f);
    g.setColour (juce::Colour (0xff3dd68c));
    g.drawRoundedRectangle (r.reduced (0.5f), 4.0f, 1.0f);

    g.setFont (juce::FontOptions (14.0f).withStyle ("Monospaced"));
    auto text = r.reduced (8.0f, 6.0f);
    const auto top = text.removeFromTop (text.getHeight() * 0.5f);
    g.drawText (line1_, top, juce::Justification::centredLeft);
    g.drawText (line2_, text, juce::Justification::centredLeft);
}
