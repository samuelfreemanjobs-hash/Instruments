#pragma once

#include <JuceHeader.h>

namespace junovax::ui
{
/** Swappable Sam / obsidian skin for Junova-X — attach via setLookAndFeel. */
class JunovaLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    enum class Accent
    {
        cyan,
        crimson,
        chrome
    };

    static constexpr const char* kAccentProperty = "junovaAccent";

    JunovaLookAndFeel();

    static void tagSlider (juce::Slider& slider, Accent accent) noexcept;
    static Accent accentFor (const juce::Slider& slider) noexcept;

    void drawLinearSlider (juce::Graphics&,
                           int x,
                           int y,
                           int width,
                           int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           juce::Slider::SliderStyle,
                           juce::Slider&) override;

    void drawRotarySlider (juce::Graphics&,
                           int x,
                           int y,
                           int width,
                           int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool isMouseOverButton, bool isButtonDown) override;

    void drawButtonBackground (juce::Graphics&,
                               juce::Button&,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;

    juce::Font getLabelFont (juce::Label&) override;

private:
    static juce::Colour colourForAccent (Accent accent) noexcept;
};
} // namespace junovax::ui