#include "Mpc2000xlLcdDisplay.h"
#include "View/LookAndFeel/VMpcLookAndFeel.h"

namespace vmpc::view
{
Mpc2000xlLcdDisplay::Mpc2000xlLcdDisplay()
{
    setOpaque(true);
}

void Mpc2000xlLcdDisplay::setStatusLine(const juce::String& line)
{
    statusLine = line;
    repaint();
}

void Mpc2000xlLcdDisplay::setWaveformPreview(const juce::AudioBuffer<float>& buffer)
{
    const juce::ScopedLock lock(previewLock);
    preview.makeCopyOf(buffer);
    repaint();
}

void Mpc2000xlLcdDisplay::paint(juce::Graphics& g)
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

void Mpc2000xlLcdDisplay::resized() {}
} // namespace vmpc::view
