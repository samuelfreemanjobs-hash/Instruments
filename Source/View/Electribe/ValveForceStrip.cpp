#include "ValveForceStrip.h"
#include "View/Design/StudioColours.h"

namespace vmpc::view
{
ValveForceStrip::ValveForceStrip()
{
    for (auto* s : { &driveL, &driveR })
    {
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s->setRange(0.0, 1.0, 0.01);
        s->setValue(0.45);
        addAndMakeVisible(s);
    }
    startTimerHz(15);
}

void ValveForceStrip::timerCallback()
{
    vuL = -0.5f + juce::Random::getSystemRandom().nextFloat() * 0.35f;
    vuR = -0.45f + juce::Random::getSystemRandom().nextFloat() * 0.35f;
    repaint();
}

void ValveForceStrip::drawTube(juce::Graphics& g, juce::Rectangle<float> area) const
{
    g.setColour(juce::Colour(0xff1a1816));
    g.fillRoundedRectangle(area, 6.0f);
    g.setColour(juce::Colour(0xff4a4038));
    g.drawRoundedRectangle(area, 6.0f, 2.0f);
    g.setColour(studio::amberAccent().withAlpha(0.55f));
    g.fillEllipse(area.reduced(area.getWidth() * 0.2f, area.getHeight() * 0.35f));
}

void ValveForceStrip::drawVu(juce::Graphics& g, juce::Rectangle<float> area, float angle) const
{
    g.setColour(juce::Colour(0xff1a1408));
    g.fillRoundedRectangle(area, 3.0f);
    g.setColour(studio::amberAccent());
    const auto pivot = juce::Point<float>(area.getCentreX(), area.getBottom() - 4.0f);
    juce::Path needle;
    needle.startNewSubPath(pivot);
    needle.lineTo(pivot.getX() + std::sin(angle) * area.getHeight() * 0.8f,
                  pivot.getY() - std::cos(angle) * area.getHeight() * 0.8f);
    g.strokePath(needle, juce::PathStrokeType(2.0f));
}

void ValveForceStrip::paint(juce::Graphics& g)
{
    g.fillAll(studio::obsidian2());
    g.setColour(studio::iceOled());
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText("VALVE FORCE — 12AX7", getLocalBounds().removeFromTop(16).reduced(8, 0), juce::Justification::left);

    auto area = getLocalBounds().withTrimmedTop(18).reduced(8);
    const int tubeW = 48;
    drawVu(g, area.removeFromLeft(tubeW).toFloat().reduced(4), vuL);
    drawTube(g, area.removeFromLeft(tubeW).toFloat().reduced(2));
    drawTube(g, area.removeFromLeft(tubeW).toFloat().reduced(2));
    area.removeFromLeft(8);
    drawVu(g, area.removeFromRight(tubeW).toFloat().reduced(4), vuR);
}

void ValveForceStrip::resized()
{
    auto knobs = getLocalBounds().withTrimmedTop(18).withTrimmedLeft(getWidth() / 2 - 50).withTrimmedRight(getWidth() / 2 - 50);
    driveL.setBounds(knobs.removeFromLeft(50).reduced(4));
    driveR.setBounds(knobs.removeFromLeft(50).reduced(4));
}
} // namespace vmpc::view
