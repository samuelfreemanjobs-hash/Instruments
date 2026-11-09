#include "FactoryLookAndFeel.h"

#if ! defined (FACTORY_GUI_USE_SYSTEM_FONTS) || ! FACTORY_GUI_USE_SYSTEM_FONTS
 #include <FactoryGuiFonts.h>
#endif

namespace fds {
namespace gui {

namespace {
juce::Typeface::Ptr typefaceFromBinary (const char* name, const void* data, size_t size)
{
    juce::ignoreUnused (name);
    if (data == nullptr || size == 0)
        return {};
    return juce::Typeface::createSystemTypefaceFor (data, size);
}
} // namespace

FactoryLookAndFeel::FactoryLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, argb (color::bg::chassis));
    setColour (juce::Label::textColourId, argb (color::text::primary));
    setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::TextButton::buttonColourId, argb (color::control::body));
    setColour (juce::TextButton::textColourOffId, argb (color::text::primary));
    setColour (juce::TextButton::textColourOnId, argb (color::text::onBrand));
    setColour (juce::ToggleButton::textColourId, argb (color::text::primary));
    setColour (juce::ComboBox::backgroundColourId, argb (color::bg::well));
    setColour (juce::ComboBox::outlineColourId, argb (color::border::subtle));
    setColour (juce::ComboBox::textColourId, argb (color::text::primary));
    setColour (juce::PopupMenu::backgroundColourId, argb (color::bg::overlay));
    setColour (juce::PopupMenu::textColourId, argb (color::text::primary));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, argb (color::brand::subtle));
    setColour (juce::PopupMenu::highlightedTextColourId, argb (color::text::primary));
    setColour (juce::TooltipWindow::backgroundColourId, argb (color::bg::overlay));
    setColour (juce::TooltipWindow::outlineColourId, argb (color::border::overlay));
    setColour (juce::TooltipWindow::textColourId, argb (color::text::primary));
    setColour (juce::Slider::rotarySliderFillColourId, argb (color::brand::stroke));
    setColour (juce::Slider::rotarySliderOutlineColourId, argb (color::control::track));
    setColour (juce::Slider::thumbColourId, argb (color::control::body));
    setColour (juce::Slider::trackColourId, argb (color::bg::well));
    setColour (juce::CaretComponent::caretColourId, argb (color::brand::stroke));

    loadEmbeddedTypefaces();
}

void FactoryLookAndFeel::loadEmbeddedTypefaces()
{
#if defined (FACTORY_GUI_USE_SYSTEM_FONTS) && FACTORY_GUI_USE_SYSTEM_FONTS
    fontsLoaded = false;
    return;
#else
    auto load = [] (const char* resourceName) -> juce::Typeface::Ptr
    {
        int size = 0;
        const char* data = FactoryGuiFonts::getNamedResource (resourceName, size);
        return typefaceFromBinary (resourceName, data, (size_t) size);
    };

    // Resource names are filename with non-alnum → underscore (JUCE binary data).
    auto tryLoad = [&] (std::initializer_list<const char*> names) -> juce::Typeface::Ptr
    {
        for (auto* n : names)
        {
            if (auto t = load (n))
                return t;
        }
        return {};
    };

    sansRegular   = tryLoad ({ "Inter_Regular_ttf", "IBMPlexSans_Regular_ttf" });
    sansMedium    = tryLoad ({ "Inter_Medium_ttf", "IBMPlexSans_Medium_ttf" });
    sansSemiBold  = tryLoad ({ "Inter_SemiBold_ttf", "IBMPlexSans_SemiBold_ttf" });
    sansBold      = tryLoad ({ "Inter_Bold_ttf", "IBMPlexSans_Bold_ttf" });
    monoRegular   = tryLoad ({ "JetBrainsMono_Regular_ttf", "IBMPlexMono_Regular_ttf" });
    monoMedium    = tryLoad ({ "JetBrainsMono_Medium_ttf", "IBMPlexMono_Medium_ttf" });
    monoSemiBold  = tryLoad ({ "JetBrainsMono_SemiBold_ttf", "IBMPlexMono_SemiBold_ttf" });
    monoBold      = tryLoad ({ "JetBrainsMono_Bold_ttf", "IBMPlexMono_Bold_ttf" });
    fontsLoaded = (sansRegular != nullptr) || (monoRegular != nullptr);
#endif
}

juce::Typeface::Ptr FactoryLookAndFeel::findTypeface (const juce::String& family, int weight) const
{
    const bool mono = family.containsIgnoreCase ("Mono")
                      || family.containsIgnoreCase ("JetBrains")
                      || family.equalsIgnoreCase (typography::family::mono.name);

    auto pick = [&] (juce::Typeface::Ptr r, juce::Typeface::Ptr m,
                     juce::Typeface::Ptr s, juce::Typeface::Ptr b) -> juce::Typeface::Ptr
    {
        if (weight >= 700 && b) return b;
        if (weight >= 600 && s) return s;
        if (weight >= 500 && m) return m;
        if (r) return r;
        if (m) return m;
        if (s) return s;
        return b;
    };

    if (mono)
        return pick (monoRegular, monoMedium, monoSemiBold, monoBold);
    return pick (sansRegular, sansMedium, sansSemiBold, sansBold);
}

juce::Typeface::Ptr FactoryLookAndFeel::getTypefaceForFont (const juce::Font& font)
{
    if (fontsLoaded)
    {
        const int w = font.isBold() ? 700 : 400;
        if (auto t = findTypeface (font.getTypefaceName(), w))
            return t;
    }
    return LookAndFeel_V4::getTypefaceForFont (font);
}

void FactoryLookAndFeel::drawFocusRing (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour (argb (color::focus::ring));
    const float t = stroke::focus;
    const float o = stroke::focusOffset;
    g.drawRoundedRectangle (bounds.expanded (o).reduced (t * 0.5f), radius::xs + o, t);
}

void FactoryLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float /*rotaryStartAngle*/,
                                           float /*rotaryEndAngle*/, juce::Slider& slider)
{
    // Spec: 270° sweep from 7:30 to 4:30 (135° → 405° screen space).
    constexpr float startAngle = juce::MathConstants<float>::pi * 1.25f; // 225° from +x? use 135° from north
    // JUCE angles: 0 = 12 o'clock, clockwise. Spec: 135° → 405° screen = from SW to SE via bottom.
    // Screen degrees from +x CCW: 135° (7:30) to 405° (4:30). In JUCE (from 12 o'clock clockwise):
    // 7:30 = 225° clockwise from 12, 4:30 = 135° clockwise from 12 going the long way...
    // Use: start = 2.35619 (135° from +x in standard math) — match common plugin knobs:
    const float rotaryStart = juce::MathConstants<float>::pi * 1.25f; // 225° = 7:30
    const float rotaryEnd   = juce::MathConstants<float>::pi * 2.75f; // 495° → equiv 135° = 4:30 (270° sweep)
    juce::ignoreUnused (startAngle);

    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced ((float) size::knobArcInset);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();

    // Origin: unipolar = min (sliderPos 0); bipolar via properties "fdsBipolar" / "fdsOrigin"
    float originPos = 0.0f;
    if (slider.getProperties().contains ("fdsOrigin"))
        originPos = (float) slider.getProperties()["fdsOrigin"];
    else if ((bool) slider.getProperties().getWithDefault ("fdsBipolar", false))
        originPos = 0.5f;

    const float toAngle = rotaryStart + sliderPos * (rotaryEnd - rotaryStart);
    const float fromAngle = rotaryStart + originPos * (rotaryEnd - rotaryStart);
    const float arcFrom = juce::jmin (fromAngle, toAngle);
    const float arcTo = juce::jmax (fromAngle, toAngle);

    juce::Path track;
    track.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, rotaryStart, rotaryEnd, true);
    g.setColour (argb (color::control::track));
    g.strokePath (track, juce::PathStrokeType (stroke::arcMd, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    if (std::abs (arcTo - arcFrom) > 1.0e-4f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, arcFrom, arcTo, true);
        g.setColour (slider.isEnabled() ? argb (color::brand::stroke) : argb (color::text::disabled));
        g.strokePath (valueArc, juce::PathStrokeType (stroke::arcMd, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
    }

    const float bodyR = radius * 0.72f;
    juce::ColourGradient grad (argb (elevation::bodyGradientFrom), centre.x, centre.y - bodyR,
                               argb (elevation::bodyGradientTo), centre.x, centre.y + bodyR, false);
    g.setGradientFill (grad);
    g.fillEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f);
    g.setColour (argb (color::control::rim));
    g.drawEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f, stroke::rim);

    // Pointer 35% → 85% of body radius
    const float ang = toAngle - juce::MathConstants<float>::halfPi;
    const float inner = bodyR * 0.35f;
    const float outer = bodyR * 0.85f;
    g.setColour (slider.isEnabled() ? argb (color::control::pointer) : argb (color::text::disabled));
    g.drawLine (centre.x + std::cos (ang) * inner, centre.y + std::sin (ang) * inner,
                centre.x + std::cos (ang) * outer, centre.y + std::sin (ang) * outer, 2.0f);

    if (slider.hasKeyboardFocus (true) && ! slider.isMouseButtonDown())
        drawFocusRing (g, juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (1.0f));
}

void FactoryLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                                           float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                           juce::Slider::SliderStyle style, juce::Slider& slider)
{
    const bool vertical = style == juce::Slider::LinearVertical
                          || style == juce::Slider::LinearBarVertical;
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h);

    float originPos = 0.0f;
    if (slider.getProperties().contains ("fdsOrigin"))
        originPos = (float) slider.getProperties()["fdsOrigin"];
    else if ((bool) slider.getProperties().getWithDefault ("fdsBipolar", false))
        originPos = 0.5f;

    if (vertical)
    {
        const float cx = bounds.getCentreX();
        const float trackW = (float) size::slider::trackThickness;
        auto track = juce::Rectangle<float> (cx - trackW * 0.5f, bounds.getY(), trackW, bounds.getHeight())
                         .withSizeKeepingCentre (trackW, bounds.getHeight());
        g.setColour (argb (color::bg::well));
        g.fillRoundedRectangle (track, radius::xs);

        const float y0 = bounds.getBottom() - originPos * bounds.getHeight();
        const float y1 = sliderPos;
        g.setColour (argb (color::brand::stroke));
        g.fillRect (juce::Rectangle<float> (track.getX(), juce::jmin (y0, y1), trackW, std::abs (y1 - y0)));

        const float th = (float) size::slider::thumbThickness;
        const float tl = (float) size::slider::thumbLength;
        auto thumb = juce::Rectangle<float> (cx - tl * 0.5f, sliderPos - th * 0.5f, tl, th);
        juce::ColourGradient grad (argb (elevation::bodyGradientFrom), thumb.getX(), thumb.getY(),
                                   argb (elevation::bodyGradientTo), thumb.getX(), thumb.getBottom(), false);
        g.setGradientFill (grad);
        g.fillRoundedRectangle (thumb, radius::xs);
    }
    else
    {
        const float cy = bounds.getCentreY();
        const float trackH = (float) size::slider::trackThickness;
        auto track = juce::Rectangle<float> (bounds.getX(), cy - trackH * 0.5f, bounds.getWidth(), trackH);
        g.setColour (argb (color::bg::well));
        g.fillRoundedRectangle (track, radius::xs);

        const float x0 = bounds.getX() + originPos * bounds.getWidth();
        const float x1 = sliderPos;
        g.setColour (argb (color::brand::stroke));
        g.fillRect (juce::Rectangle<float> (juce::jmin (x0, x1), track.getY(), std::abs (x1 - x0), trackH));

        const float th = (float) size::slider::thumbThickness;
        const float tl = (float) size::slider::thumbLength;
        auto thumb = juce::Rectangle<float> (sliderPos - th * 0.5f, cy - tl * 0.5f, th, tl);
        juce::ColourGradient grad (argb (elevation::bodyGradientFrom), thumb.getX(), thumb.getY(),
                                   argb (elevation::bodyGradientTo), thumb.getX(), thumb.getBottom(), false);
        g.setGradientFill (grad);
        g.fillRoundedRectangle (thumb, radius::xs);
    }

    if (slider.hasKeyboardFocus (true) && ! slider.isMouseButtonDown())
        drawFocusRing (g, bounds);
}

void FactoryLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                           bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = button.getToggleState();
    const auto role = button.getProperties().getWithDefault (ledRoleProperty, "standard").toString();
    const bool caution = role.equalsIgnoreCase ("caution");

    juce::ColourGradient grad (argb (elevation::bodyGradientFrom), bounds.getX(), bounds.getY(),
                               argb (elevation::bodyGradientTo), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (bounds, radius::xs);
    if (! down)
    {
        // raised drop approximated as bottom edge darken
        g.setColour (argb (elevation::raisedDrop.color).withAlpha (elevation::raisedDrop.alpha * 0.35f));
        g.drawRoundedRectangle (bounds.translated (0, 1), radius::xs, 1.0f);
    }

    const float led = (float) size::led;
    auto ledBounds = juce::Rectangle<float> (bounds.getX() + 6.0f,
                                             bounds.getCentreY() - led * 0.5f, led, led);
    if (on)
    {
        const auto ledColour = caution ? argb (color::state::caution) : argb (color::active::default_);
        g.setColour (ledColour.withAlpha (0.35f));
        g.fillEllipse (ledBounds.expanded (3.0f));
        g.setColour (ledColour);
        g.fillEllipse (ledBounds);
    }
    else
    {
        g.setColour (argb (color::state::ledOff));
        g.fillEllipse (ledBounds);
    }

    g.setColour (button.isEnabled() ? argb (color::text::primary) : argb (color::text::disabled));
    auto font = makeFont (typography::style::legend);
    g.setFont (font);
    auto textArea = bounds.withTrimmedLeft (led + 14.0f).withTrimmedRight (6.0f);
    g.drawFittedText (applyCase (button.getButtonText(), typography::style::legend.textCase),
                      textArea.toNearestInt(), juce::Justification::centredLeft, 1);

    juce::ignoreUnused (highlighted);
    if (button.hasKeyboardFocus (true) && ! down)
        drawFocusRing (g, bounds);
}

void FactoryLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                               const juce::Colour&, bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);
    const auto variant = button.getProperties().getWithDefault (buttonVariantProperty, "secondary").toString();

    if (variant == "primary")
    {
        g.setColour (down ? argb (color::active::pressed) : (highlighted ? argb (color::active::hover)
                                                                          : argb (color::active::default_)));
        g.fillRoundedRectangle (bounds, radius::xs);
        g.setColour (argb (color::active::pressed));
        g.drawRoundedRectangle (bounds, radius::xs, 1.0f);
    }
    else if (variant == "brand")
    {
        g.setColour (down ? argb (color::brand::pressed) : argb (color::brand::fill));
        g.fillRoundedRectangle (bounds, radius::xs);
    }
    else
    {
        juce::ColourGradient grad (argb (elevation::bodyGradientFrom), bounds.getX(), bounds.getY(),
                                   argb (elevation::bodyGradientTo), bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill (grad);
        g.fillRoundedRectangle (bounds, radius::xs);
        g.setColour (argb (color::control::rim));
        g.drawRoundedRectangle (bounds, radius::xs, stroke::rim);
    }

    if (button.hasKeyboardFocus (true) && ! down)
        drawFocusRing (g, bounds);
}

void FactoryLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                         bool, bool)
{
    const auto variant = button.getProperties().getWithDefault (buttonVariantProperty, "secondary").toString();
    juce::Colour textCol = argb (color::text::primary);
    if (variant == "primary")
        textCol = argb (color::text::onActive);
    else if (variant == "brand")
        textCol = argb (color::text::onBrand);
    if (! button.isEnabled())
        textCol = argb (color::text::disabled);

    g.setColour (textCol);
    g.setFont (makeFont (typography::style::legend));
    g.drawFittedText (applyCase (button.getButtonText(), typography::style::legend.textCase),
                      button.getLocalBounds(), juce::Justification::centred, 1);
}

void FactoryLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    auto r = juce::Rectangle<float> (0, 0, (float) width, (float) height);
    g.setColour (argb (color::bg::overlay));
    g.fillRoundedRectangle (r, radius::sm);
    g.setColour (argb (color::border::overlay));
    g.drawRoundedRectangle (r.reduced (0.5f), radius::sm, 1.0f);
}

void FactoryLookAndFeel::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                            bool isSeparator, bool isActive, bool isHighlighted,
                                            bool isTicked, bool /*hasSubMenu*/,
                                            const juce::String& text, const juce::String& /*shortcutKeyText*/,
                                            const juce::Drawable*, const juce::Colour*)
{
    if (isSeparator)
    {
        g.setColour (argb (color::border::subtle));
        g.fillRect (area.reduced (8, 0).withHeight (1).withY (area.getCentreY()));
        return;
    }

    auto r = area.toFloat();
    if (isHighlighted && isActive)
    {
        g.setColour (argb (color::brand::subtle));
        g.fillRoundedRectangle (r.reduced (2.0f), radius::xs);
    }

    if (isTicked)
    {
        g.setColour (argb (color::active::default_));
        g.fillEllipse (r.getX() + 6.0f, r.getCentreY() - 3.0f, 6.0f, 6.0f);
    }

    g.setColour (isActive ? argb (color::text::primary) : argb (color::text::disabled));
    g.setFont (makeFont (typography::style::body));
    g.drawText (text, area.withTrimmedLeft (18).withTrimmedRight (8), juce::Justification::centredLeft, true);
}

void FactoryLookAndFeel::drawTooltip (juce::Graphics& g, const juce::String& text, int width, int height)
{
    auto r = juce::Rectangle<float> (0, 0, (float) width, (float) height);
    g.setColour (argb (color::bg::overlay));
    g.fillRoundedRectangle (r, radius::sm);
    g.setColour (argb (color::border::overlay));
    g.drawRoundedRectangle (r.reduced (0.5f), radius::sm, 1.0f);
    g.setColour (argb (color::text::primary));
    g.setFont (makeFont (typography::style::body));
    g.drawFittedText (text, r.reduced (10.0f, 8.0f).toNearestInt(), juce::Justification::centredLeft, 4);
}

void FactoryLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                       int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float> (0, 0, (float) width, (float) height);
    g.setColour (argb (color::bg::well));
    g.fillRoundedRectangle (r, radius::xs);
    g.setColour (argb (color::border::subtle));
    g.drawRoundedRectangle (r.reduced (0.5f), radius::xs, 1.0f);
    if (box.hasKeyboardFocus (true))
        drawFocusRing (g, r);
}

void FactoryLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    g.fillAll (label.findColour (juce::Label::backgroundColourId));
    if (! label.isBeingEdited())
    {
        g.setColour (label.findColour (juce::Label::textColourId));
        g.setFont (label.getFont());
        g.drawFittedText (label.getText(), label.getLocalBounds(), label.getJustificationType(),
                          juce::jmax (1, (int) (label.getHeight() / label.getFont().getHeight())),
                          label.getMinimumHorizontalScale());
    }
}

} // namespace gui
} // namespace fds
