#pragma once

#include "CelestialTheme.h"

#include <JuceHeader.h>

namespace junovax::ui::celestial
{
class PresetLcdStrip final : public juce::Component
{
public:
    PresetLcdStrip();

    void setPresetText (const juce::String& text);
    juce::String getPresetText() const;

    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::TextButton& prev() noexcept { return prev_; }
    juce::TextButton& next() noexcept { return next_; }

private:
    juce::String presetText_ { "INIT" };
    juce::TextButton prev_ { "<" };
    juce::TextButton next_ { ">" };
};
} // namespace junovax::ui::celestial