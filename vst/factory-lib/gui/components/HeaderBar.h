#pragma once

#include "LedToggle.h"
#include "PresetBrowser.h"
#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

class HeaderBar : public juce::Component
{
public:
    HeaderBar (const juce::String& productNameText,
               const juce::String& versionText = "0.1.0");

    void setProductName (const juce::String& name);
    void setVersion (const juce::String& version);
    void setGlyphPath (juce::Path glyph);
    /** Outline (stroked brand.stroke), fill (filled brand.stroke), accent (stroked premium.badge). */
    void setGlyphPaths (juce::Path outline, juce::Path fill, juce::Path accent);

    LedToggle& getBypassToggle() noexcept { return bypass; }
    PresetBrowser& getPresetBrowser() noexcept { return presets; }
    juce::Component& getGlyphSlot() noexcept { return glyphSlot; }
    juce::Label& getProductNameLabel() noexcept { return productName; }
    const juce::Label& getProductNameLabel() const noexcept { return productName; }
    /** Product-name label bounds in HeaderBar local coords — must lie inside the header. */
    juce::Rectangle<int> getProductNameBounds() const { return productName.getBounds(); }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class GlyphSlot : public juce::Component
    {
    public:
        juce::Path path;      // legacy single fill (brand.stroke)
        juce::Path outline;   // stroked brand.stroke
        juce::Path fill;      // filled brand.stroke
        juce::Path accent;    // stroked premium.badge
        void paint (juce::Graphics& g) override
        {
            const bool multi = ! outline.isEmpty() || ! fill.isEmpty() || ! accent.isEmpty();
            if (! multi && path.isEmpty())
            {
                g.setColour (argb (color::brand::fill));
                g.fillRoundedRectangle (getLocalBounds().toFloat(), radius::xs);
                return;
            }
            if (multi)
            {
                g.setColour (argb (color::brand::stroke));
                if (! fill.isEmpty())
                    g.fillPath (fill);
                if (! outline.isEmpty())
                    g.strokePath (outline, juce::PathStrokeType (1.5f));
                g.setColour (argb (color::premium::badge));
                if (! accent.isEmpty())
                    g.strokePath (accent, juce::PathStrokeType (1.5f));
                return;
            }
            g.setColour (argb (color::brand::stroke));
            g.fillPath (path);
        }
    };

    GlyphSlot glyphSlot;
    juce::Label productName;
    juce::Label brandLabel;
    PresetBrowser presets;
    juce::Label versionLabel;
    LedToggle bypass { "Bypass", LedRole::caution, ToggleSize::md };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderBar)
};

} // namespace gui
} // namespace fds
