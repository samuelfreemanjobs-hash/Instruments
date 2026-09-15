#include "MixerChannelStrip.h"

namespace vmpc::view
{
MixerChannelStrip::MixerChannelStrip(const juce::String& channelName)
    : name(channelName)
{
    fader.setSliderStyle(juce::Slider::LinearVertical);
    fader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    fader.setRange(0.0, 1.0, 0.001);
    fader.setValue(0.75);
    addAndMakeVisible(fader);
    addAndMakeVisible(muteBtn);
    addAndMakeVisible(soloBtn);
}

void MixerChannelStrip::meterUpdate(float peak)
{
    meterPeak = juce::jlimit(0.0f, 1.0f, peak);
    repaint();
}

void MixerChannelStrip::resized()
{
    auto area = getLocalBounds().reduced(4);
    muteBtn.setBounds(area.removeFromTop(22).removeFromLeft(22));
    soloBtn.setBounds(area.removeFromTop(22).removeFromLeft(22));
    area.removeFromTop(28);
    fader.setBounds(area.removeFromLeft(28));
}

void MixerChannelStrip::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff16161a));

    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(11.0f);
    g.drawText(name, getLocalBounds().removeFromTop(18), juce::Justification::centred);

    auto meter = getLocalBounds().removeFromRight(10).reduced(0, 24);
    const int segments = 12;
    const int segH = meter.getHeight() / segments;
    for (int i = 0; i < segments; ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const bool lit = meterPeak >= t;
        auto seg = meter.removeFromBottom(segH).reduced(0, 1);
        g.setColour(lit ? juce::Colour(0xff3fffd6) : juce::Colour(0xff2a2a30));
        g.fillRect(seg);
    }

    // Parametric EQ curve placeholder
    juce::Path eq;
    const auto eqArea = juce::Rectangle<float>(static_cast<float>(getWidth()) - 52.0f, 28.0f, 40.0f, 36.0f);
    eq.startNewSubPath(eqArea.getX(), eqArea.getCentreY());
    eq.quadraticTo(eqArea.getCentreX(), eqArea.getY(), eqArea.getRight(), eqArea.getCentreY() + 4.0f);
    g.setColour(juce::Colour(0xff6ec8e8).withAlpha(0.8f));
    g.strokePath(eq, juce::PathStrokeType(1.5f));
}
} // namespace vmpc::view
