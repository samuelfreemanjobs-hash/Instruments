#pragma once

#include "UI/SkinAssets.h"

#include <JuceHeader.h>

namespace wave9090::ui
{

/** Default placeholder skin — swap BinaryData-backed images without touching DSP. */
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

    juce::Font getLabelFont (juce::Label&) override;

    const juce::Image& getPanelBackground() const noexcept { return panelBg_; }
    const juce::Image& getKnobDial() const noexcept { return knobDial_; }

private:
    juce::Image panelBg_;
    juce::Image knobDial_;
    juce::Colour accent_ { 0xffc8a43c };
    juce::Colour panelFill_ { 0xff14141c };
    juce::Colour textPrimary_ { 0xffe8e8f0 };
    juce::Colour textMuted_ { 0xff9898a8 };
};

} // namespace wave9090::ui