#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/** 16×2-style status LCD (two text lines). */
class LcdPanelComponent : public juce::Component
{
public:
    LcdPanelComponent();

    void setLines (juce::String line1, juce::String line2);
    void paint (juce::Graphics& g) override;

private:
    juce::String line1_;
    juce::String line2_;
};
