#pragma once

#include "UI/DesignTokens.h"
#include "UI/SkinAssets.h"

#include <JuceHeader.h>

namespace wave9090::ui
{

class Wave9090LookAndFeel final : public juce::LookAndFeel_V4
{
public:
    Wave9090LookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                       int buttonW, int buttonH, juce::ComboBox&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getLabelFont (juce::Label&) override;

    static tokens::KnobDomain domainFromSlider (const juce::Slider& slider) noexcept;

private:
    juce::Image knobDial_;
};

} // namespace wave9090::ui