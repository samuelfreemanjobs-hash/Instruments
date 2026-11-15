#pragma once

#include "dl_ui/SkinAssetCache.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace dl_ui
{

class SkinnableLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    explicit SkinnableLookAndFeel (SkinAssetCache& assets);

    void setDefaultFilmstrip (const juce::String& resourceName, int numFrames);

    void drawRotarySlider (juce::Graphics&,
                            int x,
                            int y,
                            int width,
                            int height,
                            float sliderPosProportional,
                            float rotaryStartAngle,
                            float rotaryEndAngle,
                            juce::Slider&) override;

    void drawButtonBackground (juce::Graphics&,
                                juce::Button&,
                                const juce::Colour&,
                                bool,
                                bool) override;

    juce::Font getLabelFont (juce::Label&) override;

private:
    SkinAssetCache& assets_;
    juce::String rotaryFilmstrip_;
    int rotaryFrames_ = 0;
};

} // namespace dl_ui