#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
/** Four Q-Link sliders (Phase 2 UI; Phase 5 MIDI CC automation). */
class QLinkSliderStrip : public juce::Component
{
public:
    static constexpr int kNumLinks = 4;

    using ValueChanged = std::function<void(int linkIndex, float value01)>;

    QLinkSliderStrip();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setLinkLabel(int index, const juce::String& label);
    void setLinkValue(int index, float value01);
    float getLinkValue(int index) const;

    void onValueChanged(ValueChanged callback) { valueChanged = std::move(callback); }

private:
    std::array<juce::Slider, kNumLinks> sliders;
    std::array<juce::Label, kNumLinks> labels;
    ValueChanged valueChanged;
};
} // namespace vmpc::view
