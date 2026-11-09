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

    LedToggle& getBypassToggle() noexcept { return bypass; }
    PresetBrowser& getPresetBrowser() noexcept { return presets; }
    juce::Component& getGlyphSlot() noexcept { return glyphSlot; }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class GlyphSlot : public juce::Component
    {
    public:
        juce::Path path;
        void paint (juce::Graphics& g) override
        {
            if (path.isEmpty())
            {
                g.setColour (argb (color::brand::fill));
                g.fillRoundedRectangle (getLocalBounds().toFloat(), radius::xs);
            }
            else
            {
                g.setColour (argb (color::brand::stroke));
                g.fillPath (path);
            }
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
