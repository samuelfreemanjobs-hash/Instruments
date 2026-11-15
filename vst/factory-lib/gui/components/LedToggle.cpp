#include "LedToggle.h"
#include "../FactoryLookAndFeel.h"

namespace fds {
namespace gui {

LedToggle::LedToggle (const juce::String& legendText, LedRole role, ToggleSize sz)
    : toggleSize (sz)
{
    button.setButtonText (legendText);
    button.getProperties().set (FactoryLookAndFeel::ledRoleProperty,
                                role == LedRole::caution ? "caution" : "standard");
    button.onClick = [this]
    {
        if (onToggle)
            onToggle (button.getToggleState());
    };
    addAndMakeVisible (button);
    const int h = (toggleSize == ToggleSize::lg) ? size::toggle::lg : size::toggle::md;
    setSize (96, h);
}

void LedToggle::setLegend (const juce::String& text) { button.setButtonText (text); }

void LedToggle::setToggleState (bool on, juce::NotificationType n)
{
    button.setToggleState (on, n);
}

bool LedToggle::getToggleState() const { return button.getToggleState(); }

void LedToggle::setLedRole (LedRole role)
{
    button.getProperties().set (FactoryLookAndFeel::ledRoleProperty,
                                role == LedRole::caution ? "caution" : "standard");
    button.repaint();
}

void LedToggle::resized() { button.setBounds (getLocalBounds()); }

std::unique_ptr<juce::AccessibilityHandler> LedToggle::createAccessibilityHandler()
{
    return std::make_unique<juce::AccessibilityHandler> (
        *this, juce::AccessibilityRole::toggleButton, juce::AccessibilityActions());
}

} // namespace gui
} // namespace fds
