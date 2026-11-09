#pragma once

#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

class ValueReadout : public juce::Component,
                     public juce::SettableTooltipClient
{
public:
    enum class State { normal, floor, over };

    ValueReadout();

    void setValueText (const juce::String& value, const juce::String& unit = {});
    void setFromDb (float db);
    void setState (State s);
    void setLarge (bool large);

    std::function<void (const juce::String&)> onTextCommit;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;

private:
    juce::Label valueLabel;
    juce::Label unitLabel;
    State state = State::normal;
    bool large = true;
    juce::Path wellPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueReadout)
};

} // namespace gui
} // namespace fds
