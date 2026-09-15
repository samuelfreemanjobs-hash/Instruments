#include "StudioLcdDisplay.h"
#include "View/LookAndFeel/StudioLookAndFeel.h"

namespace resonance::view
{
StudioLcdDisplay::StudioLcdDisplay()
{
    setOpaque(true);
}

void StudioLcdDisplay::setStatusLine(const juce::String& line)
{
    statusLine = line;
    repaint();
}

void StudioLcdDisplay::setWaveformPreview(const juce::AudioBuffer<float>& buffer)
{
    const juce::ScopedLock lock(previewLock);
    preview.makeCopyOf(buffer);
    repaint();
}

void StudioLcdDisplay::paint(juce::Graphics& g)
{
    const auto lcdBg = juce::Colour(0xff0a1620);
    const auto grid = juce::Colour(0xff1a3040);
    const auto trace = juce::Colour(0xff6ec8e8);

    g.fillAll(lcdBg);
    g.setColour(grid);
    for (int x = 0; x < getWidth(); x += 12)
        g.drawVerticalLine(x, 0.0f, static_cast<float>(getHeight()));

    juce::AudioBuffer<float> local;
    {
        const juce::ScopedLock lock(previewLock);
        local.makeCopyOf(preview);
    }

    if (local.getNumSamples() > 0)
    {
        g.setColour(trace);
        juce::Path wave;
        const auto area = getLocalBounds().reduced(8).withTrimmedBottom(22);
        const int n = local.getNumSamples();
        for (int i = 0; i < n; ++i)
        {
            const float x = area.getX() + area.getWidth() * (static_cast<float>(i) / static_cast<float>(n - 1));
            const float y = area.getCentreY() - local.getSample(0, i) * area.getHeight() * 0.45f;
            if (i == 0)
                wave.startNewSubPath(x, y);
            else
                wave.lineTo(x, y);
        }
        g.strokePath(wave, juce::PathStrokeType(1.2f));
    }

    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 13.0f, juce::Font::plain));
    g.drawText(statusLine, getLocalBounds().removeFromBottom(20).reduced(8, 0), juce::Justification::centredLeft);
}

void StudioLcdDisplay::resized() {}
} // namespace resonance::view
