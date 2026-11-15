#pragma once

#include <JuceHeader.h>

namespace wave9090::ui
{

/** Skinnable knob row: legend + rotary + live value (APVTS attachment applied by editor). */
class LabeledKnob final : public juce::Component
{
public:
    explicit LabeledKnob (juce::String legendText);

    void resized() override;
    juce::Slider& getSlider() noexcept { return slider_; }
    juce::Label& getLegend() noexcept { return legend_; }

    void setValueText (const juce::String& text);
    std::function<juce::String (float normalised)> formatValue;

private:
    void sliderValueChanged();
    juce::Label legend_;
    juce::Slider slider_;
    juce::Label value_;
};

} // namespace wave9090::ui