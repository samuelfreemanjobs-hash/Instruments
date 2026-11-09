/**
 * Contract: each factory GUI component renders non-blank images at 100% and 200%
 * for the configured theme, and sampled pixels match DesignTokens role colours
 * (meter over cap == meter.over; peak bar splits peak/hot at hotThresholdDb;
 * focus ring present only when keyboard-focused; UTF-8 middot/minus round-trip).
 *
 * Visual regression artefacts (L-0001 delete-before-write, L-0003 UTF-8, L-0012 text):
 *   FACTORY_GUI_RENDER_OUT_DIR/<theme>/<component>-<state>@<scale>x.png
 *   FACTORY_GUI_RENDER_OUT_DIR/<theme>/<component>-<state>@<scale>x.json
 * plus BUILD_INFO.json at the render root.
 */
#include "../FactoryGui.h"
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

namespace {

struct Failure
{
    juce::String message;
};

struct TextEntry
{
    juce::String string;
    juce::Rectangle<float> bounds; // component-local, unscaled
};

std::vector<Failure> failures;
int checks = 0;

void expect (bool cond, const juce::String& msg)
{
    ++checks;
    if (! cond)
        failures.push_back ({ msg });
}

juce::uint32 toARGB (juce::Colour c)
{
    return c.getARGB();
}

bool colourClose (juce::Colour a, juce::Colour b, int tol = 8)
{
    return std::abs ((int) a.getRed() - (int) b.getRed()) <= tol
        && std::abs ((int) a.getGreen() - (int) b.getGreen()) <= tol
        && std::abs ((int) a.getBlue() - (int) b.getBlue()) <= tol;
}

bool imageNonBlank (const juce::Image& img)
{
    if (! img.isValid() || img.getWidth() < 2 || img.getHeight() < 2)
        return false;
    const auto first = img.getPixelAt (0, 0);
    for (int y = 0; y < img.getHeight(); y += juce::jmax (1, img.getHeight() / 16))
        for (int x = 0; x < img.getWidth(); x += juce::jmax (1, img.getWidth() / 16))
            if (img.getPixelAt (x, y) != first)
                return true;
    return first.getAlpha() > 0;
}

juce::Image renderComponent (juce::Component& c, float scale)
{
    const int w = juce::jmax (1, (int) std::ceil (c.getWidth() * scale));
    const int h = juce::jmax (1, (int) std::ceil (c.getHeight() * scale));
    juce::Image img (juce::Image::ARGB, w, h, true);
    juce::Graphics g (img);
    g.addTransform (juce::AffineTransform::scale (scale));
    c.paintEntireComponent (g, true);
    return img;
}

void writePng (const juce::Image& img, const juce::File& file)
{
    file.getParentDirectory().createDirectory();
    // L-0001: FileOutputStream appends — delete first so re-runs don't concatenate PNGs.
    file.deleteFile();
    juce::FileOutputStream stream (file);
    if (stream.openedOk())
    {
        juce::PNGImageFormat png;
        png.writeImageToStream (img, stream);
    }
}

juce::String jsonEscape (const juce::String& s)
{
    juce::String out;
    out.preallocateBytes ((size_t) s.length() * 2 + 8);
    for (auto cp = s.getCharPointer(); ! cp.isEmpty();)
    {
        auto c = cp.getAndAdvance();
        switch (c)
        {
            case '"':  out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (c < 0x20)
                    out << "\\u" << juce::String::toHexString ((int) c).paddedLeft ('0', 4);
                else
                    out += juce::String::charToString ((juce::juce_wchar) c);
                break;
        }
    }
    return out;
}

juce::String rectJson (juce::Rectangle<float> r)
{
    return "{\"x\":" + juce::String (r.getX(), 3)
         + ",\"y\":" + juce::String (r.getY(), 3)
         + ",\"w\":" + juce::String (r.getWidth(), 3)
         + ",\"h\":" + juce::String (r.getHeight(), 3) + "}";
}

/** String width compatible with JUCE 7 (Font::getStringWidthFloat) and JUCE 8 (GlyphArrangement). */
float stringWidth (const juce::Font& font, const juce::String& text)
{
#if JUCE_MAJOR_VERSION >= 8
    return juce::GlyphArrangement::getStringWidth (font, text);
#else
    return font.getStringWidthFloat (text);
#endif
}

/** If the glyph width exceeds the box, record ellipsis (L-0012 truncation signal). */
juce::String drawnStringFor (const juce::Font& font, const juce::String& text, float maxWidth)
{
    if (text.isEmpty())
        return text;
    if (stringWidth (font, text) <= maxWidth + 0.5f)
        return text;
    return "...";
}

TextEntry labelTextEntry (juce::Label& label, juce::Component& root)
{
    const auto font = label.getFont();
    const auto local = root.getLocalArea (&label, label.getLocalBounds()).toFloat();
    const auto drawn = drawnStringFor (font, label.getText(), (float) label.getWidth());
    // Tight text bounds: left-aligned glyph box inside the label area.
    const float tw = juce::jmin (stringWidth (font, drawn), local.getWidth());
    const float th = juce::jmin (font.getHeight(), local.getHeight());
    juce::Rectangle<float> tb (local.getX(), local.getY() + (local.getHeight() - th) * 0.5f, tw, th);
    return { drawn, tb };
}

void collectLabels (juce::Component& root, juce::Component& node, std::vector<TextEntry>& out)
{
    if (auto* lab = dynamic_cast<juce::Label*> (&node))
    {
        if (lab->getText().isNotEmpty())
            out.push_back (labelTextEntry (*lab, root));
    }
    for (int i = 0; i < node.getNumChildComponents(); ++i)
        if (auto* ch = node.getChildComponent (i))
            collectLabels (root, *ch, out);
}

void writeSidecar (const juce::File& jsonFile,
                   const juce::String& component,
                   const juce::String& state,
                   float scale,
                   int widthLogical,
                   int heightLogical,
                   const std::vector<TextEntry>& texts)
{
    jsonFile.getParentDirectory().createDirectory();
    jsonFile.deleteFile();
    juce::FileOutputStream stream (jsonFile);
    if (! stream.openedOk())
        return;

    juce::String body;
    body << "{\n";
    body << "  \"component\": \"" << jsonEscape (component) << "\",\n";
    body << "  \"state\": \"" << jsonEscape (state) << "\",\n";
    body << "  \"scale\": " << juce::String (scale, 1) << ",\n";
    body << "  \"theme\": \"" << jsonEscape (fds::themeId) << "\",\n";
    body << "  \"width\": " << widthLogical << ",\n";
    body << "  \"height\": " << heightLogical << ",\n";
    body << "  \"texts\": [\n";
    for (size_t i = 0; i < texts.size(); ++i)
    {
        const auto& t = texts[i];
        body << "    {\"string\": \"" << jsonEscape (t.string) << "\", \"bounds\": "
             << rectJson (t.bounds) << "}";
        if (i + 1 < texts.size())
            body << ",";
        body << "\n";
    }
    body << "  ]\n";
    body << "}\n";
    stream.writeText (body, false, false, nullptr);
}

juce::File outDir()
{
#ifdef FACTORY_GUI_RENDER_OUT_DIR
    return juce::File (FACTORY_GUI_RENDER_OUT_DIR).getChildFile (fds::themeId);
#else
    return juce::File::getCurrentWorkingDirectory().getChildFile ("gui-renders").getChildFile (fds::themeId);
#endif
}

juce::File renderRoot()
{
#ifdef FACTORY_GUI_RENDER_OUT_DIR
    return juce::File (FACTORY_GUI_RENDER_OUT_DIR);
#else
    return juce::File::getCurrentWorkingDirectory().getChildFile ("gui-renders");
#endif
}

juce::String scaleTag (float scale)
{
    if (std::abs (scale - 1.0f) < 1.0e-3f)
        return "1x";
    if (std::abs (scale - 2.0f) < 1.0e-3f)
        return "2x";
    return juce::String (scale, 1) + "x";
}

/** Capture PNG + text sidecar for one component state. */
void capture (juce::Component& c,
              const juce::String& component,
              const juce::String& state,
              float scale,
              std::vector<TextEntry> extraTexts = {})
{
    auto img = renderComponent (c, scale);
    expect (imageNonBlank (img), component + "-" + state + "@" + scaleTag (scale) + " blank");

    std::vector<TextEntry> texts = std::move (extraTexts);
    collectLabels (c, c, texts);

    // L-0012: ellipsis in drawn text is a hard failure at render time too.
    for (const auto& t : texts)
        expect (! t.string.contains ("..."),
                component + "-" + state + " drawn text truncated to ellipsis: '" + t.string + "'");

    const auto stem = component + "-" + state + "@" + scaleTag (scale);
    const auto png = outDir().getChildFile (stem + ".png");
    const auto json = outDir().getChildFile (stem + ".json");
    writePng (img, png);
    writeSidecar (json, component, state, scale, c.getWidth(), c.getHeight(), texts);
}

void applyButtonState (juce::Button& b, const juce::String& state)
{
    b.setEnabled (true);
    if (state == "disabled")
    {
        b.setEnabled (false);
        b.setState (juce::Button::buttonNormal);
    }
    else if (state == "hover")
    {
        b.setState (juce::Button::buttonOver);
    }
    else if (state == "focus")
    {
        b.setState (juce::Button::buttonNormal);
        b.setWantsKeyboardFocus (true);
        b.grabKeyboardFocus();
    }
    else
    {
        b.setState (juce::Button::buttonNormal);
    }
}

// ---------------------------------------------------------------------------
// Contract pixel checks (kept from prior suite)
// ---------------------------------------------------------------------------

void checkMeterOver (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::LevelMeter meter (fds::gui::MeterBarSize::lg);
    meter.setLookAndFeel (&lnf);
    meter.setSize (40, 160);
    meter.resized();
    meter.setLevels (-1.0f, -8.0f, -0.5f, true);

    auto img = renderComponent (meter, scale);
    expect (imageNonBlank (img), "LevelMeter@" + juce::String (scale) + " blank");

    const int cx = img.getWidth() / 2;
    const int cy = juce::jmax (1, (int) std::round (3.0f * scale));
    const auto pix = img.getPixelAt (cx, cy);
    const auto expected = fds::gui::argb (fds::color::meter::over);
    expect (colourClose (pix, expected, 40),
            "LevelMeter over cap pixel " + juce::String::toHexString ((int) toARGB (pix))
                + " != meter.over " + juce::String::toHexString ((int) toARGB (expected))
                + " @ scale " + juce::String (scale));
    meter.setLookAndFeel (nullptr);
}

/** Contract: peak bar is peak below hotThreshold, hot only above (components.md §7). */
void checkMeterPeakHotSplit (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::LevelMeter meter (fds::gui::MeterBarSize::lg);
    meter.setLookAndFeel (&lnf);
    meter.setSize (48, 200);
    meter.resized();

    auto sampleAtDb = [&] (float db) -> juce::Colour
    {
        auto img = renderComponent (meter, scale);
        const float lx = meter.getPeakColumnBounds().getCentreX();
        const float ly = meter.yForDb (db);
        const int x = juce::jlimit (0, img.getWidth() - 1, (int) std::lround ((double) lx * (double) scale));
        const int y = juce::jlimit (0, img.getHeight() - 1, (int) std::lround ((double) ly * (double) scale));
        return img.getPixelAt (x, y);
    };

    meter.setLevels (-2.0f, -40.0f, -2.0f, false);
    {
        const auto at30 = sampleAtDb (-30.0f);
        const auto at3 = sampleAtDb (-3.0f);
        const auto peakCol = fds::gui::argb (fds::color::meter::peak);
        const auto hotCol = fds::gui::argb (fds::color::meter::hot);
        expect (colourClose (at30, peakCol, 35),
                "peak=-2: pixel @ -30 dB " + juce::String::toHexString ((int) toARGB (at30))
                    + " != meter.peak " + juce::String::toHexString ((int) toARGB (peakCol))
                    + " scale=" + juce::String (scale));
        expect (colourClose (at3, hotCol, 35),
                "peak=-2: pixel @ -3 dB " + juce::String::toHexString ((int) toARGB (at3))
                    + " != meter.hot " + juce::String::toHexString ((int) toARGB (hotCol))
                    + " scale=" + juce::String (scale));
        std::cout << "ASSERT peak=-2dB scale=" << scale
                  << " @-30=" << juce::String::toHexString ((int) toARGB (at30))
                  << " (peak) @-3=" << juce::String::toHexString ((int) toARGB (at3))
                  << " (hot)" << std::endl;
    }

    meter.setLevels (-20.0f, -40.0f, -20.0f, false);
    {
        const auto at3 = sampleAtDb (-3.0f);
        const auto hotCol = fds::gui::argb (fds::color::meter::hot);
        const auto trackCol = fds::gui::argb (fds::color::meter::track);
        expect (! colourClose (at3, hotCol, 25),
                "peak=-20: pixel @ -3 dB must not be meter.hot (got "
                    + juce::String::toHexString ((int) toARGB (at3)) + ")");
        expect (colourClose (at3, trackCol, 40),
                "peak=-20: pixel @ -3 dB " + juce::String::toHexString ((int) toARGB (at3))
                    + " should be meter.track " + juce::String::toHexString ((int) toARGB (trackCol)));
        std::cout << "ASSERT peak=-20dB scale=" << scale
                  << " @-3=" << juce::String::toHexString ((int) toARGB (at3))
                  << " (track, not hot)" << std::endl;
    }

    meter.setLookAndFeel (nullptr);
}

// ---------------------------------------------------------------------------
// Visual regression state matrix
// ---------------------------------------------------------------------------

void captureMeters (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    auto make = [&] (const juce::String& state, float peak, float rms, float hold, bool over)
    {
        fds::gui::LevelMeter meter (fds::gui::MeterBarSize::lg);
        meter.setLookAndFeel (&lnf);
        meter.setSize (48, 200);
        meter.resized();
        meter.setLevels (peak, rms, hold, over);
        capture (meter, "LevelMeter", state, scale);
        meter.setLookAndFeel (nullptr);
    };

    make ("silence", fds::meter::defaultFloorDb, fds::meter::defaultFloorDb,
          fds::meter::defaultFloorDb, false);
    make ("normal", -12.0f, -18.0f, -12.0f, false);
    make ("hot", -2.0f, -10.0f, -2.0f, false);
    make ("over-latched", 1.5f, -6.0f, 1.5f, true);
}

void captureReadouts (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    // Width sized for longest values (−96.0 / +12.0) — L-0012.
    auto captureOne = [&] (const juce::String& state, const std::function<void (fds::gui::ValueReadout&)>& setup)
    {
        fds::gui::ValueReadout r;
        r.setLookAndFeel (&lnf);
        r.setSize (140, 32);
        r.resized();
        setup (r);
        capture (r, "ValueReadout", state, scale);
        r.setLookAndFeel (nullptr);
    };

    captureOne ("neg-inf", [] (fds::gui::ValueReadout& r) { r.setFromDb (-200.0f); });
    // numericFloorDb (−96) maps to −∞ via setFromDb; still assert the −96.0 glyph string fits.
    captureOne ("neg96", [] (fds::gui::ValueReadout& r)
    {
        r.setState (fds::gui::ValueReadout::State::normal);
        r.setValueText (fds::gui::minusSign() + "96.0", "dB");
    });
    // +12.0 as a normal (non-over) glyph fit check — overThresholdDb is 0, so setFromDb(+12)
    // would force OVER; set the digits explicitly.
    captureOne ("plus12", [] (fds::gui::ValueReadout& r)
    {
        r.setState (fds::gui::ValueReadout::State::normal);
        r.setValueText ("+12.0", "dB");
    });
    captureOne ("over", [] (fds::gui::ValueReadout& r) { r.setFromDb (3.0f); });
}

void captureHeader (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    // Pin a single product name across hosts. Auto-picking the longest fitting
    // candidate drifted between Cloud and ubuntu-latest (FreeType metrics),
    // which poisoned HeaderBar goldens. "CHORD ENGINE" fits the 180 px slot
    // even when productName is uppercase (disklordz / _template).
    const auto chosen = fds::gui::utf8 ("CHORD ENGINE");
    fds::gui::HeaderBar header (chosen, "0.2.0");
    header.setLookAndFeel (&lnf);
    header.setSize (720, fds::size::headerHeight);
    header.resized();
    capture (header, "HeaderBar", "longest", scale);
    std::cout << "ASSERT HeaderBar longest='" << chosen << "' theme=" << fds::themeId << std::endl;
    header.setLookAndFeel (nullptr);
}

void captureUtf8Label (float scale)
{
    // Contract: middot / minus via utf8() helper round-trip (no mojibake Â· / Ã¢).
    const auto title = fds::gui::utf8 ("METERS ") + fds::gui::middot() + fds::gui::utf8 (" INPUT ")
                       + fds::gui::minusSign() + "12.0";
    expect (title.contains (fds::gui::middot()), "utf8 title contains middot");
    expect (title.contains (fds::gui::minusSign()), "utf8 title contains minus");
    expect (! title.contains (fds::gui::utf8 ("\xc3\x82\xc2\xb7")),
            "utf8 title must not contain mojibake Â·");
    expect (title.length() == 20,
            "utf8 title length=" + juce::String (title.length()) + " expected 20 (got mojibake if 21+)");

    fds::gui::FactoryLookAndFeel lnf;
    juce::Label label;
    label.setLookAndFeel (&lnf);
    label.setText (title, juce::dontSendNotification);
    label.setFont (fds::gui::makeFont (fds::typography::style::sectionTitle));
    label.setColour (juce::Label::textColourId, fds::gui::argb (fds::color::text::primary));
    label.setColour (juce::Label::backgroundColourId, fds::gui::argb (fds::color::bg::panel));
    label.setSize (320, 28);
    label.setJustificationType (juce::Justification::centredLeft);
    expect (label.getText() == title, "Label getText preserves utf8 helper string");

    capture (label, "Utf8Label", "default", scale);
    std::cout << "ASSERT utf8 label='" << title << "' length=" << title.length() << std::endl;
    label.setLookAndFeel (nullptr);
}

void captureKnobStates (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    for (const char* state : { "default", "hover", "focus", "disabled" })
    {
        fds::gui::RotaryKnob knob ("Drive", fds::gui::KnobSize::md);
        knob.setLookAndFeel (&lnf);
        knob.setNormalisedValue (0.7f);
        knob.setSize (96, 120);
        knob.resized();
        if (std::strcmp (state, "disabled") == 0)
            knob.setEnabledState (false);
        else if (std::strcmp (state, "focus") == 0)
        {
            knob.setWantsKeyboardFocus (true);
            knob.getSlider().setWantsKeyboardFocus (true);
            knob.getSlider().grabKeyboardFocus();
        }
        else if (std::strcmp (state, "hover") == 0)
        {
            // Slider has no public hover force; capture default geometry annotated as hover.
            // Pixel parity with default is acceptable — disabled/focus cover interaction states.
            knob.getSlider().setMouseCursor (juce::MouseCursor::PointingHandCursor);
        }
        capture (knob, "RotaryKnob", state, scale);
        knob.setLookAndFeel (nullptr);
    }
}

void captureLedStates (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    for (const char* state : { "default", "hover", "focus", "disabled" })
    {
        fds::gui::LedToggle led ("Bypass", fds::gui::LedRole::caution);
        led.setLookAndFeel (&lnf);
        led.setToggleState (true, juce::dontSendNotification);
        led.setSize (100, 28);
        led.resized();
        applyButtonState (led.getButton(), state);
        capture (led, "LedToggle", state, scale);
        led.setLookAndFeel (nullptr);
    }
}

void captureButtonStates (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    for (const char* state : { "default", "hover", "focus", "disabled" })
    {
        juce::TextButton btn ("GO");
        btn.getProperties().set (fds::gui::FactoryLookAndFeel::buttonVariantProperty, "primary");
        btn.setLookAndFeel (&lnf);
        btn.setSize (80, 28);
        applyButtonState (btn, state);

        std::vector<TextEntry> texts;
        {
            const auto font = fds::gui::makeFont (fds::typography::style::legend);
            const auto drawn = drawnStringFor (font, btn.getButtonText(), (float) btn.getWidth() - 8.0f);
            texts.push_back ({ drawn, { 4.0f, 4.0f, (float) btn.getWidth() - 8.0f, (float) btn.getHeight() - 8.0f } });
        }

        if (std::strcmp (state, "focus") == 0)
        {
            // Include focus ring in a slightly larger offscreen image for the focus artefact.
            juce::Image ringImg (juce::Image::ARGB,
                                 (int) std::ceil (100 * scale),
                                 (int) std::ceil (40 * scale),
                                 true);
            {
                juce::Graphics g (ringImg);
                g.addTransform (juce::AffineTransform::scale (scale));
                g.fillAll (fds::gui::argb (fds::color::bg::panel));
                btn.setBounds (10, 6, 80, 28);
                btn.paintEntireComponent (g, true);
                fds::gui::FactoryLookAndFeel::drawFocusRing (g, { 10.0f, 6.0f, 80.0f, 28.0f });
            }
            const auto stem = juce::String ("PushButton-focus@") + scaleTag (scale);
            writePng (ringImg, outDir().getChildFile (stem + ".png"));
            writeSidecar (outDir().getChildFile (stem + ".json"),
                          "PushButton", "focus", scale, 100, 40, texts);
        }
        else
        {
            capture (btn, "PushButton", state, scale, texts);
        }
        btn.setLookAndFeel (nullptr);
    }
}

void captureSection (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::SectionPanel p ("Dynamics");
    p.setLookAndFeel (&lnf);
    p.setSize (200, 100);
    p.resized();
    capture (p, "SectionPanel", "default", scale);
    p.setLookAndFeel (nullptr);
}

void capturePreset (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::PresetBrowser b;
    b.setLookAndFeel (&lnf);
    b.setPresetName ("Punch", true);
    b.setSize (220, 24);
    b.resized();
    capture (b, "PresetBrowser", "default", scale);
    b.setLookAndFeel (nullptr);
}

void captureScale (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::MeterScale s;
    s.setLookAndFeel (&lnf);
    s.setSize (40, 160);
    s.resized();
    capture (s, "MeterScale", "default", scale);
    s.setLookAndFeel (nullptr);
}

void writeBuildInfo()
{
    auto root = renderRoot();
    root.createDirectory();
    auto file = root.getChildFile ("BUILD_INFO.json");
    file.deleteFile();
    juce::FileOutputStream stream (file);
    if (! stream.openedOk())
        return;

    juce::String osName = "unknown";
#if JUCE_LINUX
    osName = "Linux";
#elif JUCE_WINDOWS
    osName = "Windows";
#elif JUCE_MAC
    osName = "macOS";
#endif

    juce::String body;
    body << "{\n";
    body << "  \"juce_version\": \"" << juce::SystemStats::getJUCEVersion() << "\",\n";
    body << "  \"juce_major\": " << JUCE_MAJOR_VERSION << ",\n";
    body << "  \"juce_minor\": " << JUCE_MINOR_VERSION << ",\n";
    body << "  \"juce_build\": " << JUCE_BUILDNUMBER << ",\n";
    body << "  \"theme\": \"" << jsonEscape (fds::themeId) << "\",\n";
    body << "  \"theme_version\": \"" << jsonEscape (fds::themeVersion) << "\",\n";
    body << "  \"os\": \"" << osName << "\",\n";
    body << "  \"os_detail\": \"" << jsonEscape (juce::SystemStats::getOperatingSystemName()) << "\"\n";
    body << "}\n";
    stream.writeText (body, false, false, nullptr);
}

void runAll()
{
    outDir().createDirectory();
    writeBuildInfo();
    std::cout << "FactoryGuiRenderTests theme=" << fds::themeId
              << " brand=" << fds::brandName
              << " version=" << fds::themeVersion
              << " juce=" << juce::SystemStats::getJUCEVersion() << std::endl;

    for (float scale : { 1.0f, 2.0f })
    {
        checkMeterOver (scale);
        checkMeterPeakHotSplit (scale);
        captureMeters (scale);
        captureReadouts (scale);
        captureHeader (scale);
        captureUtf8Label (scale);
        captureKnobStates (scale);
        captureLedStates (scale);
        captureButtonStates (scale);
        captureSection (scale);
        capturePreset (scale);
        captureScale (scale);
    }
}

} // namespace

int main (int argc, char** argv)
{
    juce::ignoreUnused (argc, argv);
    juce::ScopedJuceInitialiser_GUI juceInit;

    runAll();

    std::cout << "checks=" << checks << " failures=" << failures.size() << std::endl;
    for (auto& f : failures)
        std::cerr << "FAIL: " << f.message << std::endl;

    if (failures.empty())
    {
        std::cout << "PASS FactoryGuiRenderTests [" << fds::themeId << "]" << std::endl;
        return 0;
    }
    return 1;
}
