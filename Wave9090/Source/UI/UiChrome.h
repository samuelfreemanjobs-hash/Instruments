#pragma once

#include "UI/DesignTokens.h"

#include <JuceHeader.h>

namespace wave9090::ui
{

inline void paintWalnutCheeks (juce::Graphics& g, juce::Rectangle<int> bounds, int cheekW)
{
    auto left = bounds.removeFromLeft (cheekW);
    auto right = bounds.removeFromRight (cheekW);
    juce::ColourGradient lg (tokens::woodHi(), 0, 0, tokens::woodDark(), static_cast<float> (left.getWidth()), 0, false);
    juce::ColourGradient rg (tokens::woodDark(), 0, 0, tokens::woodHi(), static_cast<float> (right.getWidth()), 0, false);
    g.setGradientFill (lg);
    g.fillRect (left);
    g.setGradientFill (rg);
    g.fillRect (right);
    for (auto* r : { &left, &right })
    {
        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.drawVerticalLine (r->getRight() - 1, static_cast<float> (r->getY()), static_cast<float> (r->getBottom()));
    }
}

inline void paintRivets (juce::Graphics& g, juce::Rectangle<float> box)
{
    const float d = 5.0f;
    const float inset = 5.0f;
    const juce::Point<float> pts[4] = {
        { box.getX() + inset, box.getY() + inset },
        { box.getRight() - inset - d, box.getY() + inset },
        { box.getX() + inset, box.getBottom() - inset - d },
        { box.getRight() - inset - d, box.getBottom() - inset - d },
    };
    for (auto p : pts)
    {
        g.setColour (tokens::metalScrew());
        g.fillEllipse (p.x, p.y, d, d);
        g.setColour (juce::Colours::white.withAlpha (0.12f));
        g.drawEllipse (p.x + 0.5f, p.y + 0.5f, d - 1.0f, d - 1.0f, 0.6f);
    }
}

inline void paintModuleBay (juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title, juce::Colour accent)
{
    auto box = area.toFloat().reduced (1.0f);
    g.setColour (tokens::surfaceInset());
    g.fillRoundedRectangle (box, 6.0f);
    g.setColour (juce::Colours::white.withAlpha (0.07f));
    g.drawRoundedRectangle (box, 6.0f, 1.0f);
    g.setColour (juce::Colours::white.withAlpha (0.12f));
    g.drawHorizontalLine (static_cast<int> (box.getY() + 1.0f), box.getX() + 8.0f, box.getRight() - 8.0f);

    paintRivets (g, box);

    g.setColour (accent.withAlpha (0.85f));
    g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)));
    g.drawText (title.toUpperCase(), area.withHeight (22).reduced (10, 0), juce::Justification::centredLeft);
}

} // namespace wave9090::ui