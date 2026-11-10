#include "PluginEditorFormatters.h"
#include <cmath>

namespace TrapLordEditorFormatters
{
namespace
{
juce::String formatSeconds (double seconds)
{
    if (seconds < 1.0)
        return juce::String (seconds * 1000.0, seconds < 0.1 ? 1 : 0) + " ms";
    return juce::String (seconds, seconds < 10.0 ? 2 : 1) + " s";
}

juce::String formatPercent (double normalized)
{
    return juce::String (normalized * 100.0, normalized < 0.1 || normalized > 0.95 ? 0 : 1) + "%";
}
} // namespace

void configureTimeSeconds (juce::Slider& slider)
{
    slider.textFromValueFunction = [] (double v) { return formatSeconds (v); };
    slider.setNumDecimalPlacesToDisplay (0);
}

void configurePercent (juce::Slider& slider)
{
    slider.textFromValueFunction = [] (double v) { return formatPercent (v); };
    slider.setNumDecimalPlacesToDisplay (0);
}

void configureHz (juce::Slider& slider)
{
    slider.textFromValueFunction = [] (double v)
    {
        if (v >= 1000.0)
            return juce::String (v * 0.001, 2) + " kHz";
        return juce::String (v, 0) + " Hz";
    };
    slider.setNumDecimalPlacesToDisplay (0);
}

void configureGainDb (juce::Slider& slider)
{
    slider.textFromValueFunction = [] (double linear)
    {
        const double db = 20.0 * std::log10 (juce::jmax (linear, 1.0e-6));
        return juce::String (db, 1) + " dB";
    };
    slider.setNumDecimalPlacesToDisplay (0);
}

void configureDrive (juce::Slider& slider)
{
    slider.textFromValueFunction = [] (double v) { return juce::String (v, v < 10.0 ? 2 : 1) + "x"; };
    slider.setNumDecimalPlacesToDisplay (0);
}

void configureCents (juce::Slider& slider)
{
    slider.textFromValueFunction = [] (double v) { return juce::String (v, 1) + " ct"; };
    slider.setNumDecimalPlacesToDisplay (0);
}

void configureArpDivision (juce::Slider& slider)
{
    slider.textFromValueFunction = [] (double v) { return "1/" + juce::String (int (std::round (v))); };
    slider.setNumDecimalPlacesToDisplay (0);
}
} // namespace TrapLordEditorFormatters
