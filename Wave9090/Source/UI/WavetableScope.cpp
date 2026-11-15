#include "UI/WavetableScope.h"
#include "UI/DesignTokens.h"

namespace wave9090::ui
{

WavetableScope::WavetableScope()
{
    startTimerHz (20);
}

void WavetableScope::ensureBank() const
{
    if (bank_ == nullptr)
    {
        bank_ = std::make_unique<dsp::WavetableBank>();
        bank_->rebuild();
    }
}

void WavetableScope::setWavetablePosition (float pos01) noexcept
{
    position_ = juce::jlimit (0.0f, 1.0f, pos01);
}

void WavetableScope::timerCallback()
{
    scanPhase_ += 0.04f;
    repaint();
}

void WavetableScope::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (2.0f);
    juce::ColourGradient glass (tokens::surfaceGlass().brighter (0.08f), r.getCentreX(), r.getY(),
                                tokens::surfaceGlass().darker (0.4f), r.getCentreX(), r.getBottom(), false);
    g.setGradientFill (glass);
    g.fillRoundedRectangle (r, 4.0f);
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.drawRoundedRectangle (r, 4.0f, 1.0f);

    // scanlines
    g.setColour (juce::Colours::white.withAlpha (0.04f));
    for (float y = r.getY(); y < r.getBottom(); y += 3.0f)
        g.drawHorizontalLine (static_cast<int> (y), r.getX(), r.getRight());

    ensureBank();
    juce::Path wave;
    const int n = juce::jmax (32, getWidth());
    const float posWithScan = juce::jlimit (0.0f, 1.0f, position_ + std::sin (scanPhase_) * 0.06f);
    for (int i = 0; i < n; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (n - 1);
        const float s = bank_->sampleMorph (phase, posWithScan, 220.0 / 48000.0);
        const float x = r.getX() + (static_cast<float> (i) / static_cast<float> (n - 1)) * r.getWidth();
        const float y = r.getCentreY() - s * r.getHeight() * 0.38f;
        if (i == 0)
            wave.startNewSubPath (x, y);
        else
            wave.lineTo (x, y);
    }
    g.setColour (tokens::cyanGlow().withAlpha (0.92f));
    g.strokePath (wave, juce::PathStrokeType (2.0f));

  const float dotX = r.getX() + posWithScan * r.getWidth();
    g.setColour (tokens::amberGlow());
    g.fillEllipse (dotX - 3.0f, r.getY() + 6.0f, 6.0f, 6.0f);

    g.setColour (tokens::textDim());
    g.setFont (juce::Font (juce::FontOptions (9.0f)));
    g.drawText ("POS " + juce::String (static_cast<int> (position_ * 100.0f)) + "%",
                r.removeFromBottom (14.0f), juce::Justification::centredRight);
}

} // namespace wave9090::ui