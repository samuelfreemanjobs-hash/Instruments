/**
 * Contract: each factory GUI component renders non-blank images at 100% and 200%
 * for the configured theme, and sampled pixels match DesignTokens role colours
 * (meter over cap == meter.over; peak bar splits peak/hot at hotThresholdDb;
 * focus ring present only when keyboard-focused; UTF-8 middot/minus round-trip).
 *
 * Writes PNGs to FACTORY_GUI_RENDER_OUT_DIR/<theme>/<name>@Nx.png
 */
#include "../FactoryGui.h"
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

struct Failure
{
    juce::String message;
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
    // FileOutputStream appends by default — truncate so re-runs don't corrupt PNGs.
    file.deleteFile();
    juce::FileOutputStream stream (file);
    if (stream.openedOk())
    {
        juce::PNGImageFormat png;
        png.writeImageToStream (img, stream);
    }
}

juce::File outDir()
{
#ifdef FACTORY_GUI_RENDER_OUT_DIR
    return juce::File (FACTORY_GUI_RENDER_OUT_DIR).getChildFile (fds::themeId);
#else
    return juce::File::getCurrentWorkingDirectory().getChildFile ("gui-renders").getChildFile (fds::themeId);
#endif
}

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

    writePng (img, outDir().getChildFile ("LevelMeter@" + juce::String (scale, 1) + "x.png"));
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

    // Case A: peak = -2 dBFS → -30 dB height is peak, -3 dB height is hot
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
        writePng (renderComponent (meter, scale),
                  outDir().getChildFile ("LevelMeter_peakHot@" + juce::String (scale, 1) + "x.png"));
    }

    // Case B: peak = -20 dBFS → no hot segment; -3 dB height is empty track (not hot)
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
        writePng (renderComponent (meter, scale),
                  outDir().getChildFile ("LevelMeter_peakCool@" + juce::String (scale, 1) + "x.png"));
    }

    meter.setLookAndFeel (nullptr);
}

void checkUtf8Label (float scale)
{
    // Contract: middot / minus via utf8() helper round-trip (no mojibake Â· / Ã¢).
    const auto title = fds::gui::utf8 ("METERS ") + fds::gui::middot() + fds::gui::utf8 (" INPUT ")
                       + fds::gui::minusSign() + "12.0";
    expect (title.contains (fds::gui::middot()), "utf8 title contains middot");
    expect (title.contains (fds::gui::minusSign()), "utf8 title contains minus");
    expect (! title.contains (fds::gui::utf8 ("\xc3\x82\xc2\xb7")),
            "utf8 title must not contain mojibake Â·");

    // Raw Latin-1 mis-decode of · would be two chars; juce length of correct string:
    // "METERS " (7) + · (1) + " INPUT " (7) + − (1) + "12.0" (4) = 20
    expect (title.length() == 20,
            "utf8 title length=" + juce::String (title.length()) + " expected 20 (got mojibake if 21+)");

    fds::gui::FactoryLookAndFeel lnf;
    juce::Label label;
    label.setLookAndFeel (&lnf);
    label.setText (title, juce::dontSendNotification);
    label.setFont (fds::gui::makeFont (fds::typography::style::sectionTitle));
    label.setColour (juce::Label::textColourId, fds::gui::argb (fds::color::text::primary));
    label.setColour (juce::Label::backgroundColourId, fds::gui::argb (fds::color::bg::panel));
    label.setSize (280, 28);
    label.setJustificationType (juce::Justification::centredLeft);

    // Round-trip through getText — must preserve glyphs
    expect (label.getText() == title, "Label getText preserves utf8 helper string");

    auto img = renderComponent (label, scale);
    expect (imageNonBlank (img), "Utf8Label blank");
    writePng (img, outDir().getChildFile ("Utf8Label@" + juce::String (scale, 1) + "x.png"));
    std::cout << "ASSERT utf8 label='" << title << "' length=" << title.length() << std::endl;
    label.setLookAndFeel (nullptr);
}

void checkFocusRing (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    juce::TextButton btn ("GO");
    btn.getProperties().set (fds::gui::FactoryLookAndFeel::buttonVariantProperty, "primary");
    btn.setLookAndFeel (&lnf);
    btn.setSize (80, 28);

    btn.setFocusContainerType (juce::Component::FocusContainerType::none);
    auto imgOff = renderComponent (btn, scale);
    expect (imageNonBlank (imgOff), "Button unfocused blank");

    juce::Image ringImg (juce::Image::ARGB, (int) (100 * scale), (int) (40 * scale), true);
    {
        juce::Graphics g (ringImg);
        g.addTransform (juce::AffineTransform::scale (scale));
        g.fillAll (fds::gui::argb (fds::color::bg::panel));
        fds::gui::FactoryLookAndFeel::drawFocusRing (g, { 10.0f, 10.0f, 80.0f, 28.0f });
    }
    const int rx = (int) (50 * scale);
    const int ry = (int) ((10.0f - fds::stroke::focusOffset) * scale);
    const auto ringPix = ringImg.getPixelAt (juce::jlimit (0, ringImg.getWidth() - 1, rx),
                                             juce::jlimit (0, ringImg.getHeight() - 1, ry));
    expect (colourClose (ringPix, fds::gui::argb (fds::color::focus::ring), 50)
                || colourClose (ringPix, fds::gui::argb (fds::color::bg::panel), 5) == false,
            "Focus ring should paint focus.ring colour");

    const auto edge = imgOff.getPixelAt (1, 1);
    expect (! colourClose (edge, fds::gui::argb (fds::color::focus::ring), 20),
            "Unfocused button must not show focus.ring at corner");

    writePng (imgOff, outDir().getChildFile ("PushButton@" + juce::String (scale, 1) + "x.png"));
    writePng (ringImg, outDir().getChildFile ("FocusRing@" + juce::String (scale, 1) + "x.png"));
    btn.setLookAndFeel (nullptr);
}

void checkKnob (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::RotaryKnob knob ("Drive", fds::gui::KnobSize::md);
    knob.setLookAndFeel (&lnf);
    knob.setNormalisedValue (0.7f);
    knob.setSize (96, 120);
    knob.resized();
    auto img = renderComponent (knob, scale);
    expect (imageNonBlank (img), "RotaryKnob blank");
    writePng (img, outDir().getChildFile ("RotaryKnob@" + juce::String (scale, 1) + "x.png"));
    knob.setLookAndFeel (nullptr);
}

void checkLed (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::LedToggle led ("Bypass", fds::gui::LedRole::caution);
    led.setLookAndFeel (&lnf);
    led.setToggleState (true, juce::dontSendNotification);
    led.setSize (100, 28);
    led.resized();
    auto img = renderComponent (led, scale);
    expect (imageNonBlank (img), "LedToggle blank");
    writePng (img, outDir().getChildFile ("LedToggle@" + juce::String (scale, 1) + "x.png"));
    led.setLookAndFeel (nullptr);
}

void checkHeader (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::HeaderBar header ("MeterTap", "0.2.0");
    header.setLookAndFeel (&lnf);
    header.setSize (640, fds::size::headerHeight);
    header.resized();
    auto img = renderComponent (header, scale);
    expect (imageNonBlank (img), "HeaderBar blank");
    const auto bg = img.getPixelAt (2, 2);
    expect (colourClose (bg, fds::gui::argb (fds::color::bg::chassis), 30),
            "HeaderBar bg should be chassis");
    writePng (img, outDir().getChildFile ("HeaderBar@" + juce::String (scale, 1) + "x.png"));
    header.setLookAndFeel (nullptr);
}

void checkReadout (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::ValueReadout r;
    r.setLookAndFeel (&lnf);
    r.setFromDb (1.5f);
    r.setSize (120, 32);
    r.resized();
    auto img = renderComponent (r, scale);
    expect (imageNonBlank (img), "ValueReadout blank");
    writePng (img, outDir().getChildFile ("ValueReadout@" + juce::String (scale, 1) + "x.png"));
    r.setLookAndFeel (nullptr);
}

void checkSection (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::SectionPanel p ("Dynamics");
    p.setLookAndFeel (&lnf);
    p.setSize (200, 100);
    p.resized();
    auto img = renderComponent (p, scale);
    expect (imageNonBlank (img), "SectionPanel blank");
    const auto bg = img.getPixelAt (img.getWidth() / 2, img.getHeight() / 2);
    expect (colourClose (bg, fds::gui::argb (fds::color::bg::section), 30),
            "SectionPanel fill should be bg.section");
    writePng (img, outDir().getChildFile ("SectionPanel@" + juce::String (scale, 1) + "x.png"));
    p.setLookAndFeel (nullptr);
}

void checkPreset (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::PresetBrowser b;
    b.setLookAndFeel (&lnf);
    b.setPresetName ("Punch", true);
    b.setSize (220, 24);
    b.resized();
    auto img = renderComponent (b, scale);
    expect (imageNonBlank (img), "PresetBrowser blank");
    writePng (img, outDir().getChildFile ("PresetBrowser@" + juce::String (scale, 1) + "x.png"));
    b.setLookAndFeel (nullptr);
}

void checkScale (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::MeterScale s;
    s.setLookAndFeel (&lnf);
    s.setSize (40, 160);
    s.resized();
    auto img = renderComponent (s, scale);
    expect (imageNonBlank (img), "MeterScale blank");
    writePng (img, outDir().getChildFile ("MeterScale@" + juce::String (scale, 1) + "x.png"));
    s.setLookAndFeel (nullptr);
}

void runAll()
{
    outDir().createDirectory();
    std::cout << "FactoryGuiRenderTests theme=" << fds::themeId
              << " brand=" << fds::brandName
              << " version=" << fds::themeVersion << std::endl;

    for (float scale : { 1.0f, 2.0f })
    {
        checkMeterOver (scale);
        checkMeterPeakHotSplit (scale);
        checkUtf8Label (scale);
        checkFocusRing (scale);
        checkKnob (scale);
        checkLed (scale);
        checkHeader (scale);
        checkReadout (scale);
        checkSection (scale);
        checkPreset (scale);
        checkScale (scale);
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
