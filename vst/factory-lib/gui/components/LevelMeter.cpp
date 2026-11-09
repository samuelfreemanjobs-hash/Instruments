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

float LevelMeter::yForDb (float db) const noexcept
{
    return barBounds.getBottom() - dbToNorm (db, meter::defaultFloorDb) * barBounds.getHeight();
}

juce::Rectangle<float> LevelMeter::getPeakColumnBounds() const noexcept
{
    const float pw = barBounds.getWidth() * meter::peakWidthFraction;
    return { barBounds.getCentreX() - pw * 0.5f, barBounds.getY(), pw, barBounds.getHeight() };
}

void LevelMeter::paint (juce::Graphics& g)
{
    // Over cap
    g.setColour (over ? argb (color::meter::over) : argb (color::state::ledOff));
    g.fillRoundedRectangle (overCapBounds, radius::xs);

    // Track
    g.setColour (argb (color::meter::track));
    g.fillPath (trackPath);

    auto yFor = [this] (float db) { return yForDb (db); };

    // RMS full width
    {
        const float y = yFor (rmsDb);
        auto fill = juce::Rectangle<float> (barBounds.getX(), y, barBounds.getWidth(),
                                            barBounds.getBottom() - y);
        g.setColour (argb (color::meter::rms));
        g.fillRect (fill);
    }

    // Peak half-width: meter.peak from floor up to hotThreshold; meter.hot only above it.
    {
        const float pw = barBounds.getWidth() * meter::peakWidthFraction;
        const float px = barBounds.getCentreX() - pw * 0.5f;
        const float yBottom = barBounds.getBottom();
        const float yPeak = yFor (peakDb);
        const float yThresh = yFor (meter::hotThresholdDb);

        if (peakDb > meter::hotThresholdDb)
        {
            // Cool peak body: bottom → hot threshold
            g.setColour (argb (color::meter::peak));
            g.fillRect (px, yThresh, pw, yBottom - yThresh);
            // Hot tip: hot threshold → peak
            g.setColour (argb (color::meter::hot));
            g.fillRect (px, yPeak, pw, yThresh - yPeak);
        }
        else
        {
            g.setColour (argb (color::meter::peak));
            g.fillRect (px, yPeak, pw, yBottom - yPeak);
        }
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

std::unique_ptr<juce::AccessibilityHandler> LevelMeter::createAccessibilityHandler()
{
    return std::make_unique<juce::AccessibilityHandler> (
        *this, juce::AccessibilityRole::progressBar, juce::AccessibilityActions());
}

} // namespace gui
} // namespace fds
