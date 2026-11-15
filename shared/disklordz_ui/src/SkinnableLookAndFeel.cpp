#include "dl_ui/SkinnableLookAndFeel.h"

namespace dl_ui
{

SkinnableLookAndFeel::SkinnableLookAndFeel (SkinAssetCache& assets)
    : assets_ (assets)
{
    setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xff12141a));
    setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x33000000));
}

void SkinnableLookAndFeel::setDefaultFilmstrip (const juce::String& resourceName, int numFrames)
{
    rotaryFilmstrip_ = resourceName;
    rotaryFrames_ = numFrames;
}

void SkinnableLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                              int x,
                                              int y,
                                              int width,
                                              int height,
                                              float sliderPos,
                                              float,
                                              float,
                                              juce::Slider& slider)
{
    if (! rotaryFilmstrip_.isEmpty() && rotaryFrames_ > 0)
    {
        const int frame = juce::jlimit (0, rotaryFrames_ - 1,
                                        static_cast<int> (std::round (sliderPos * static_cast<float> (rotaryFrames_ - 1))));
        const auto img = assets_.getFilmstripFrame (rotaryFilmstrip_, frame, rotaryFrames_);
        if (img.isValid())
        {
            g.drawImage (img, x, y, width, height, 0, 0, img.getWidth(), img.getHeight());
            return;
        }
    }

    LookAndFeel_V4::drawRotarySlider (g, x, y, width, height, sliderPos, 0.0f, juce::MathConstants<float>::twoPi, slider);
}

void SkinnableLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                                  juce::Button& button,
                                                  const juce::Colour&,
                                                  bool,
                                                  bool)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (button.getToggleState() ? juce::Colour (0xff2d6cdf) : juce::Colour (0xff2a2d36));
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
}

juce::Font SkinnableLookAndFeel::getLabelFont (juce::Label&)
{
    return juce::Font (juce::FontOptions().withHeight (13.0f));
}

} // namespace dl_ui