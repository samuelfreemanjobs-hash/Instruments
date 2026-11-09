#pragma once

#include "GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

/**
 * LookAndFeel_V4 implementation bound exclusively to DesignTokens.h roles.
 * Fonts come from FactoryGuiFonts binary data unless FACTORY_GUI_USE_SYSTEM_FONTS=1.
 */
class FactoryLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FactoryLookAndFeel();
    ~FactoryLookAndFeel() override = default;

    juce::Typeface::Ptr getTypefaceForFont (const juce::Font& font) override;

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;

    void drawButtonText (juce::Graphics&, juce::TextButton&,
                         bool shouldDrawButtonAsHighlighted,
                         bool shouldDrawButtonAsDown) override;

    void drawPopupMenuBackground (juce::Graphics&, int width, int height) override;
    void drawPopupMenuItem (juce::Graphics&, const juce::Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted,
                            bool isTicked, bool hasSubMenu,
                            const juce::String& text, const juce::String& shortcutKeyText,
                            const juce::Drawable* icon, const juce::Colour* textColour) override;

    void drawTooltip (juce::Graphics&, const juce::String& text, int width, int height) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;

    void drawLabel (juce::Graphics&, juce::Label&) override;

    /** Draw keyboard-focus ring (2 px focus.ring, offset 2). Call from components. */
    static void drawFocusRing (juce::Graphics& g, juce::Rectangle<float> bounds);

    /** Property on ToggleButton: "fdsLedRole" = "caution" | "standard" (default). */
    static constexpr const char* ledRoleProperty = "fdsLedRole";
    /** Property on TextButton: "fdsButtonVariant" = "secondary"|"primary"|"brand". */
    static constexpr const char* buttonVariantProperty = "fdsButtonVariant";

private:
    void loadEmbeddedTypefaces();
    juce::Typeface::Ptr findTypeface (const juce::String& family, int weight) const;

    juce::Typeface::Ptr sansRegular, sansMedium, sansSemiBold, sansBold;
    juce::Typeface::Ptr monoRegular, monoMedium, monoSemiBold, monoBold;
    bool fontsLoaded = false;
};

} // namespace gui
} // namespace fds
