#include "SevenSegmentDisplay.h"

namespace vmpc::view
{
namespace
{
bool segmentOn(juce::juce_wchar c, int seg)
{
    static const juce::String digits[] { "1111110", "0110000", "1101101", "1111001", "0110011",
                                         "1011011", "1011111", "1110000", "1111111", "1111011" };
    if (c >= '0' && c <= '9')
        return digits[c - '0'][seg] == '1';
    if (c == '-')
        return seg == 6;
    if (c == '.')
        return false;
    return false;
}
} // namespace

void SevenSegmentDisplay::setText(const juce::String& text)
{
    displayText = text;
    repaint();
}

void SevenSegmentDisplay::drawDigit(juce::Graphics& g, juce::Rectangle<float> area, juce::juce_wchar c) const
{
    const auto on = juce::Colour(0xff3fffd6);
    const auto off = juce::Colour(0xff1a2830);
    const float w = area.getWidth();
    const float h = area.getHeight();
    const float t = juce::jmax(2.0f, w * 0.12f);

    auto seg = [&](int id, juce::Rectangle<float> r) {
        g.setColour(segmentOn(c, id) ? on : off.withAlpha(0.25f));
        g.fillRoundedRectangle(r, t * 0.35f);
    };

    seg(0, { area.getX() + t, area.getY(), w - 2 * t, t });
    seg(1, { area.getRight() - t, area.getY() + t, t, h * 0.5f - t });
    seg(2, { area.getRight() - t, area.getCentreY() + t * 0.5f, t, h * 0.5f - t });
    seg(3, { area.getX() + t, area.getBottom() - t, w - 2 * t, t });
    seg(4, { area.getX(), area.getCentreY() + t * 0.5f, t, h * 0.5f - t });
    seg(5, { area.getX(), area.getY() + t, t, h * 0.5f - t });
    seg(6, { area.getX() + t, area.getCentreY() - t * 0.5f, w - 2 * t, t });
}

void SevenSegmentDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff080c10));
    g.setColour(juce::Colour(0xff243038));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 4.0f, 1.0f);

    const int chars = displayText.length();
    if (chars <= 0)
        return;

    const float digitW = static_cast<float>(getWidth()) / static_cast<float>(chars);
    for (int i = 0; i < chars; ++i)
    {
        auto area = juce::Rectangle<float>(digitW * static_cast<float>(i), 4.0f, digitW, static_cast<float>(getHeight()) - 8.0f);
        drawDigit(g, area.reduced(2.0f), displayText[i]);
    }
}
} // namespace vmpc::view
