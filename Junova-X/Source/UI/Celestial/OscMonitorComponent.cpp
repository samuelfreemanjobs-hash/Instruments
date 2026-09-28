#include "OscMonitorComponent.h"

#include "PluginProcessor.h"

namespace junovax::ui::celestial
{
OscMonitorComponent::OscMonitorComponent (JunovaXAudioProcessor& processor)
    : processor_ (processor)
{
    startTimerHz (30);
}

void OscMonitorComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (4.0f);
    g.setColour (juce::Colour (0xff050508));
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (Theme::glowBlue().withAlpha (0.35f));
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

    g.setFont (juce::FontOptions (11.0f));
    g.setColour (Theme::textMuted());
    g.drawText ("OSC MONITOR // REAL-TIME DSP", bounds.removeFromTop (16.0f).reduced (6.0f, 0.0f), juce::Justification::centredLeft);

    juce::Path pathL, pathR;
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();
    const float mid = bounds.getCentreY();

    for (int i = 0; i < static_cast<int> (samples_.size()); ++i)
    {
        const float x = bounds.getX() + (static_cast<float> (i) / static_cast<float> (samples_.size() - 1)) * w;
        const float yL = mid - samples_[static_cast<std::size_t> (i)] * h * 0.35f;
        const float yR = mid - samples_[static_cast<std::size_t> ((i + 40) % samples_.size())] * h * 0.28f;
        if (i == 0)
        {
            pathL.startNewSubPath (x, yL);
            pathR.startNewSubPath (x, yR);
        }
        else
        {
            pathL.lineTo (x, yL);
            pathR.lineTo (x, yR);
        }
    }

    g.setColour (Theme::glowRed().withAlpha (0.85f));
    g.strokePath (pathL, juce::PathStrokeType (1.8f));
    g.setColour (Theme::glowCyan().withAlpha (0.85f));
    g.strokePath (pathR, juce::PathStrokeType (1.8f));
}

void OscMonitorComponent::timerCallback()
{
    processor_.copyScopeSamples (samples_.data(), static_cast<int> (samples_.size()));
    repaint();
}
} // namespace junovax::ui::celestial
