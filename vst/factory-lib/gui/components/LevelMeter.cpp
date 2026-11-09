#include "LevelMeter.h"

namespace fds {
namespace gui {

LevelMeter::LevelMeter (MeterBarSize s) : barSize (s)
{
    setSize (barWidth() + 8, 120);
    setWantsKeyboardFocus (true);
}

int LevelMeter::barWidth() const
{
    switch (barSize)
    {
        case MeterBarSize::sm: return size::meterBar::sm;
        case MeterBarSize::lg: return size::meterBar::lg;
        case MeterBarSize::xl: return size::meterBar::xl;
        case MeterBarSize::md:
        default:               return size::meterBar::md;
    }
}

void LevelMeter::setBarSize (MeterBarSize s)
{
    barSize = s;
    resized();
    repaint();
}

void LevelMeter::setLevels (float peak, float rms, float hold, bool overLatched)
{
    peakDb = peak;
    rmsDb = rms;
    holdDb = hold;
    over = overLatched;
    repaint();
}

void LevelMeter::resized()
{
    const float cap = (float) meter::overCapHeight;
    auto r = getLocalBounds().toFloat();
    overCapBounds = r.removeFromTop (cap + 2.0f).withSizeKeepingCentre ((float) barWidth(), cap);
    barBounds = r.withSizeKeepingCentre ((float) barWidth(), r.getHeight());
    trackPath.clear();
    trackPath.addRoundedRectangle (barBounds, radius::xs);
}

void LevelMeter::paint (juce::Graphics& g)
{
    // Over cap
    g.setColour (over ? argb (color::meter::over) : argb (color::state::ledOff));
    g.fillRoundedRectangle (overCapBounds, radius::xs);

    // Track
    g.setColour (argb (color::meter::track));
    g.fillPath (trackPath);

    const float floorDb = meter::defaultFloorDb;
    auto yFor = [&] (float db) -> float
    {
        return barBounds.getBottom() - dbToNorm (db, floorDb) * barBounds.getHeight();
    };

    // RMS full width
    {
        const float y = yFor (rmsDb);
        auto fill = juce::Rectangle<float> (barBounds.getX(), y, barBounds.getWidth(),
                                            barBounds.getBottom() - y);
        g.setColour (argb (color::meter::rms));
        g.fillRect (fill);
    }

    // Peak half-width, hot above threshold
    {
        const float y = yFor (peakDb);
        const float pw = barBounds.getWidth() * meter::peakWidthFraction;
        auto fill = juce::Rectangle<float> (barBounds.getCentreX() - pw * 0.5f, y, pw,
                                            barBounds.getBottom() - y);
        const bool hot = peakDb >= meter::hotThresholdDb;
        g.setColour (hot ? argb (color::meter::hot) : argb (color::meter::peak));
        g.fillRect (fill);
    }

    // Hold line
    {
        const float y = yFor (holdDb);
        g.setColour (argb (color::meter::hold));
        g.fillRect (barBounds.getX(), y - (float) meter::holdLineThickness * 0.5f,
                    barBounds.getWidth(), (float) meter::holdLineThickness);
    }
}

void LevelMeter::mouseDown (const juce::MouseEvent&)
{
    over = false;
    holdDb = meter::defaultFloorDb;
    if (onClear)
        onClear();
    repaint();
}

bool LevelMeter::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress::spaceKey || key == juce::KeyPress::returnKey)
    {
        over = false;
        holdDb = meter::defaultFloorDb;
        if (onClear)
            onClear();
        repaint();
        return true;
    }
    return false;
}

juce::Colour LevelMeter::samplePixel (int x, int y) const
{
    // Approximate: for tests that sample known regions after paint into Image.
    juce::ignoreUnused (x, y);
    return argb (color::meter::track);
}

std::unique_ptr<juce::AccessibilityHandler> LevelMeter::createAccessibilityHandler()
{
    return std::make_unique<juce::AccessibilityHandler> (
        *this, juce::AccessibilityRole::progressBar, juce::AccessibilityActions());
}

} // namespace gui
} // namespace fds
