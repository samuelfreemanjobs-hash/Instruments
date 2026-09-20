#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

/** SP-style 0–9 keypad with ENTER and CANCEL. */
class KeypadComponent : public juce::Component
{
public:
    KeypadComponent();

    std::function<void (int digit)> onDigit;
    std::function<void()> onEnter;
    std::function<void()> onCancel;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    std::array<juce::TextButton, 10> digitButtons_;
    juce::TextButton enterButton_ { "ENTER" };
    juce::TextButton cancelButton_ { "CANCEL" };
};
