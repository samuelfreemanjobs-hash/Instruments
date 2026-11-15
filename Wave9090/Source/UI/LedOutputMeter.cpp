#include "UI/LedOutputMeter.h"
#include "UI/DesignTokens.h"

namespace wave9090::ui
{

void LedOutputMeter::setLevelDb (float peakDb) noexcept
{
    if (std::abs (peakDb_ - peakDb) > 0.05f)
    {
        peakDb_ = peakDb;
        repaint();
    }
}

void LedOutputMeter::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (tokens::surfaceGlass());
    g.fillRoundedRectangle (r, 3.0f);

    const int segments = 12;
    const float segW = (r.getWidth() - static_cast<float> (segments - 1)) / static_cast<float> (segments);
    const bool silent = peakDb_ <= -72.0f;
    const float norm = silent ? 0.0f : juce::jlimit (0.0f, 1.0f, juce::jmap (peakDb_, -48.0f, 0.0f, 0.0f, 1.0f));
    const juce::Colour offColour = tokens::surfaceGlass().brighter (0.12f);

    for (int i = 0; i < segments; ++i)
    {
        const float x = r.getX() + static_cast<float> (i) * (segW + 1.0f);
        const float level = static_cast<float> (i + 1) / static_cast<float> (segments);
        const bool on = ! silent && norm >= level - 0.06f;

        juce::Colour c = juce::Colour (0xff1f5531);
        if (! silent)
        {
            if (i >= 8)
                c = tokens::panicRed();
            else if (i >= 5)
                c = tokens::amberGlow();
        }

        g.setColour (on ? c.brighter (0.35f) : (silent ? offColour : c.darker (0.6f)));
        g.fillRoundedRectangle (x, r.getY() + 2.0f, segW, r.getHeight() - 4.0f, 1.5f);
        if (on)
        {
            g.setColour (c.withAlpha (0.45f));
            g.fillRoundedRectangle (x - 1.0f, r.getY(), segW + 2.0f, r.getHeight(), 2.0f);
        }
    }
}

} // namespace wave9090::ui