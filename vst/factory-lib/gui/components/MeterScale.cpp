#include "MeterScale.h"

namespace fds {
namespace gui {

MeterScale::MeterScale()
{
    setSize (36, 120);
    setInterceptsMouseClicks (false, false);
}

void MeterScale::resized()
{
    column = getLocalBounds().toFloat().reduced (0.0f, (float) meter::overCapHeight + 2.0f);
}

void MeterScale::paint (juce::Graphics& g)
{
    g.setFont (makeFont (typography::style::scale));
    for (int i = 0; i < meter::ticksDbCount; ++i)
    {
        const float db = meter::ticksDb[i];
        const float y = column.getBottom() - dbToNorm (db) * column.getHeight();
        g.setColour (argb (color::meter::tick));
        g.fillRect (column.getX(), y - 0.5f, 5.0f, 1.0f);
        g.fillRect (column.getRight() - 5.0f, y - 0.5f, 5.0f, 1.0f);
    }

    g.setColour (argb (color::meter::tickLabel));
    for (int i = 0; i < meter::labelledTicksDbCount; ++i)
    {
        const float db = meter::labelledTicksDb[i];
        const float y = column.getBottom() - dbToNorm (db) * column.getHeight();
        juce::String label;
        if (std::abs (db) < 0.001f)
            label = "0";
        else
            label = minusSign() + juce::String ((int) std::abs (db));
        g.drawText (label, column.withY (y - 6.0f).withHeight (12.0f).toNearestInt(),
                    juce::Justification::centred, false);
    }
}

} // namespace gui
} // namespace fds
