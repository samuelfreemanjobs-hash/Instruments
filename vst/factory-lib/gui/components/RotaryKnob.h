#pragma once

#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

enum class KnobSize { sm, md, lg, xl };

/** Rotary control: legend + body + value readout. Colours from DesignTokens roles only. */
class RotaryKnob : public juce::Component,
                   private juce::Slider::Listener
{
public:
    RotaryKnob (const juce::String& legendText = {}, KnobSize size = KnobSize::md);
    ~RotaryKnob() override;

    void setLegend (const juce::String& text);
    void setValueText (const juce::String& text);
    void setNormalisedValue (float v, juce::NotificationType n = juce::sendNotificationSync);
    float getNormalisedValue() const;
    void setDefaultNormalised (float v);
    void setBipolar (bool bipolar, float originNorm = 0.5f);
    void setEnabledState (bool shouldBeEnabled);

    std::function<void (float)> onValueChange;
    std::function<juce::String (float)> valueFormatter;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDoubleClick (const juce::MouseEvent&) override;

    juce::Slider& getSlider() noexcept { return slider; }

private:
    void sliderValueChanged (juce::Slider*) override;
    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override;

    KnobSize knobSize;
    juce::Label legend;
    juce::Slider slider;
    juce::Label valueLabel;
    float defaultNorm = 0.0f;
    bool editing = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};

} // namespace gui
} // namespace fds
