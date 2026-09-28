#pragma once

#include <JuceHeader.h>

namespace junovax::ui
{
class JunovaLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    JunovaLookAndFeel()
    {
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xff12121a));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff6c5ce7));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff2d2d3a));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xfff0f0ff));
        setColour (juce::Label::textColourId, juce::Colour (0xffd8d8e8));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1e1e28));
        setColour (juce::TextButton::buttonColourId, juce::Colour (0xff3d3d52));
    }
};
} // namespace junovax::ui
