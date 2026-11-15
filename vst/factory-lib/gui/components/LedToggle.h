#pragma once

#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

enum class LedRole { standard, caution };
enum class ToggleSize { md, lg };

class LedToggle : public juce::Component
{
public:
    LedToggle (const juce::String& legendText = {}, LedRole role = LedRole::standard,
               ToggleSize size = ToggleSize::md);

    void setLegend (const juce::String& text);
    void setToggleState (bool on, juce::NotificationType n = juce::sendNotificationSync);
    bool getToggleState() const;
    void setLedRole (LedRole role);

    std::function<void (bool)> onToggle;

    void resized() override;
    void paint (juce::Graphics&) override {}

    juce::ToggleButton& getButton() noexcept { return button; }

private:
    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override;

    ToggleSize toggleSize;
    juce::ToggleButton button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LedToggle)
};

} // namespace gui
} // namespace fds
