#pragma once

#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

/** Tick column for paired level meters (labels + edge ticks). */
class MeterScale : public juce::Component
{
public:
    MeterScale();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Rectangle<float> column;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterScale)
};

} // namespace gui
} // namespace fds
