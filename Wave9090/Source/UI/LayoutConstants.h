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
    /** Footer: octave column + on-screen keyboard (mockup proportions). */
    static constexpr int footerHeight = 122;
    static constexpr int feltStripHeight = 6;
    static constexpr int octaveColumnWidth = 198;
    static constexpr int glideSlotWidth = 118;
    /** White keys shown (~2.5 octaves); use octave shift for C range. */
    static constexpr int visibleWhiteKeys = 25;
    /** Octave button `0` → lowest visible MIDI note (36 = C2, FL / 808 bass range). */
    static constexpr int octaveZeroLowestMidi = 36;
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

    static juce::Rectangle<int> footer (juce::Rectangle<int> full)
    {
        auto f = faceplate (full);
        return f.removeFromBottom (Layout::footerHeight);
    }

    static juce::Rectangle<int> body (juce::Rectangle<int> full)
    {
        auto f = faceplate (full);
        f.removeFromTop (Layout::headerHeight);
        f.removeFromBottom (Layout::footerHeight);
        return f.reduced (Layout::margin, Layout::bayGap);
    }

    /** Octave shift buttons (left column of footer). */
    static juce::Rectangle<int> octaveColumn (juce::Rectangle<int> full)
    {
        auto foot = footer (full);
        return foot.removeFromLeft (Layout::octaveColumnWidth).reduced (4, 6);
    }

    /** Keyboard keybed inside footer (after octave column + glide slot). */
    static juce::Rectangle<int> keyboardKeybed (juce::Rectangle<int> full)
    {
        auto foot = footer (full);
        foot.removeFromLeft (Layout::octaveColumnWidth);
        foot.removeFromLeft (Layout::glideSlotWidth);
        return foot.reduced (6, 6);
    }
};

} // namespace wave9090::ui