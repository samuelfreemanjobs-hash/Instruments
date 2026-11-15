#include "VoiceLedCluster.h"

namespace junovax::ui::celestial
{
VoiceLedCluster::VoiceLedCluster (JunovaXAudioProcessor& processor)
    : processor_ (processor)
{
    juce::ignoreUnused (processor_);
}

void VoiceLedCluster::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced (4.0f);
    g.setColour (Theme::textMuted());
    g.setFont (Theme::labelFont());
    g.drawText ("6-VOICE CLUSTER", b.removeFromTop (14.0f), juce::Justification::centredLeft);

    const float led = 10.0f;
    const float gap = 6.0f;
    float x = b.getX();
    const float y = b.getCentreY() - led * 0.5f;

    for (int i = 0; i < 6; ++i)
    {
        const bool on = (activeMask_ & (1u << i)) != 0;
        const bool crimson = i >= 3;
        const auto col = on ? (crimson ? Theme::stellarCrimson() : Theme::electricCyan())
                            : Theme::ledInactive();
        g.setColour (col.withAlpha (on ? 1.0f : 0.55f));
        g.fillEllipse (x, y, led, led);
        if (on)
        {
            g.setColour (col.withAlpha (0.35f));
            g.fillEllipse (x - 2.0f, y - 2.0f, led + 4.0f, led + 4.0f);
        }
        x += led + gap;
    }
}

void VoiceLedCluster::resized()
{
}
} // namespace junovax::ui::celestial