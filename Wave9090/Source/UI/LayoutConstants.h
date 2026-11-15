#pragma once

#include <JuceHeader.h>

/** Fixed layout contract for the WAVE-9090 placeholder skin — replace art via BinaryData only. */
namespace wave9090::ui
{

struct Layout
{
    static constexpr int editorWidth = 960;
    static constexpr int editorHeight = 640;
    static constexpr int headerHeight = 56;
    static constexpr int keyboardHeight = 108;
    static constexpr int footerMeterHeight = 48;
    static constexpr int margin = 12;
    static constexpr int sectionGap = 8;
    static constexpr int knobSize = 76;
    static constexpr int knobLabelHeight = 18;
};

inline juce::Rectangle<int> headerArea (juce::Rectangle<int> full)
{
    return full.removeFromTop (Layout::headerHeight);
}

inline juce::Rectangle<int> keyboardArea (juce::Rectangle<int> full)
{
    return full.removeFromBottom (Layout::keyboardHeight);
}

inline juce::Rectangle<int> footerArea (juce::Rectangle<int> full)
{
    return full.removeFromBottom (Layout::footerMeterHeight);
}

inline juce::Rectangle<int> contentArea (juce::Rectangle<int> full)
{
    full.removeFromTop (Layout::headerHeight + Layout::sectionGap);
    full.removeFromBottom (Layout::keyboardHeight + Layout::footerMeterHeight + Layout::sectionGap);
    return full.reduced (Layout::margin, 0);
}

} // namespace wave9090::ui