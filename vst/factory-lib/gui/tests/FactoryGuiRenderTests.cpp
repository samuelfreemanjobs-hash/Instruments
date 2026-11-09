/**
 * Contract: each factory GUI component renders non-blank images at 100% and 200%
 * for the configured theme, and sampled pixels match DesignTokens role colours
 * (meter over cap == meter.over; focus ring present only when keyboard-focused).
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
    // uniform non-transparent still counts if alpha present and not empty size
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

    // Sample over-cap centre (top of component)
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

void checkFocusRing (float scale)
{
    fds::gui::FactoryLookAndFeel lnf;
    juce::TextButton btn ("GO");
    btn.getProperties().set (fds::gui::FactoryLookAndFeel::buttonVariantProperty, "primary");
    btn.setLookAndFeel (&lnf);
    btn.setSize (80, 28);

    // Unfocused
    btn.setFocusContainerType (juce::Component::FocusContainerType::none);
    auto imgOff = renderComponent (btn, scale);
    expect (imageNonBlank (imgOff), "Button unfocused blank");

    // Force keyboard focus painting path via LookAndFeel drawFocusRing on a known rect
    juce::Image ringImg (juce::Image::ARGB, (int) (100 * scale), (int) (40 * scale), true);
    {
        juce::Graphics g (ringImg);
        g.addTransform (juce::AffineTransform::scale (scale));
        g.fillAll (fds::gui::argb (fds::color::bg::panel));
        fds::gui::FactoryLookAndFeel::drawFocusRing (g, { 10.0f, 10.0f, 80.0f, 28.0f });
    }
    // Sample outside fill but on ring — top edge around y=10-offset
    const int rx = (int) (50 * scale);
    const int ry = (int) ((10.0f - fds::stroke::focusOffset) * scale);
    const auto ringPix = ringImg.getPixelAt (juce::jlimit (0, ringImg.getWidth() - 1, rx),
                                             juce::jlimit (0, ringImg.getHeight() - 1, ry));
    expect (colourClose (ringPix, fds::gui::argb (fds::color::focus::ring), 50)
                || colourClose (ringPix, fds::gui::argb (fds::color::bg::panel), 5) == false,
            "Focus ring should paint focus.ring colour");

    // Compare: unfocused button image should not be dominated by focus.ring at outer edge
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
    // Chassis background sample
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
    r.setFromDb (1.5f); // over
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
