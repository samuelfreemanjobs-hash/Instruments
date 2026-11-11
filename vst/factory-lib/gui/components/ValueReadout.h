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

    juce::String getValueText() const { return valueLabel.getText(); }
    State getState() const noexcept { return state; }

    /** True when the current value string fits the value label without ellipsis. */
    bool valueTextFitsWithoutEllipsis() const
    {
        const auto text = valueLabel.getText();
        if (text.isEmpty())
            return true;
        const auto font = valueLabel.getFont();
        // JUCE Label ellipsizes when string width exceeds bounds at minimumHorizontalScale.
        const float minScale = valueLabel.getMinimumHorizontalScale();
        const float avail = (float) valueLabel.getWidth() / juce::jmax (0.01f, minScale);
        return stringWidth (font, text) <= avail + 0.5f;
    }

    juce::Rectangle<int> getValueLabelBounds() const { return valueLabel.getBounds(); }

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
