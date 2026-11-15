#pragma once

#include <JuceHeader.h>

namespace wave9090::ui
{

/** Sam v1 layout grid (SYNTH MAIN tab only). */
struct Layout
{
    static constexpr int editorWidth = 1280;
    static constexpr int editorHeight = 720;
    static constexpr int woodCheekW = 14;
    static constexpr int headerHeight = 86;
    static constexpr int tabHeight = 26;
    static constexpr int keyboardHeight = 124;
    static constexpr int perfRowHeight = 36;
    static constexpr int margin = 10;
    static constexpr int bayGap = 8;
};

struct Regions
{
    static juce::Rectangle<int> faceplate (juce::Rectangle<int> full)
    {
        return full.reduced (Layout::woodCheekW, 0);
    }

    static juce::Rectangle<int> header (juce::Rectangle<int> full)
    {
        auto f = faceplate (full);
        return f.removeFromTop (Layout::headerHeight);
    }

    static juce::Rectangle<int> tabs (juce::Rectangle<int> header)
    {
        return header.removeFromBottom (Layout::tabHeight);
    }

    static juce::Rectangle<int> body (juce::Rectangle<int> full)
    {
        auto f = faceplate (full);
        f.removeFromTop (Layout::headerHeight);
        f.removeFromBottom (Layout::keyboardHeight + Layout::perfRowHeight);
        return f.reduced (Layout::margin, Layout::bayGap);
    }

    static juce::Rectangle<int> keyboard (juce::Rectangle<int> full)
    {
        auto f = faceplate (full);
        f.removeFromBottom (Layout::keyboardHeight);
        return f;
    }

    static juce::Rectangle<int> perfRow (juce::Rectangle<int> full)
    {
        auto f = faceplate (full);
        f.removeFromBottom (Layout::perfRowHeight);
        return f.reduced (Layout::margin, 4);
    }
};

} // namespace wave9090::ui