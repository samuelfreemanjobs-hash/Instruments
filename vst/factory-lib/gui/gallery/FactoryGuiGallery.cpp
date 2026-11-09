#include "../FactoryGui.h"
#include <juce_gui_extra/juce_gui_extra.h>

class GalleryComponent : public juce::Component
{
public:
    GalleryComponent()
    {
        setLookAndFeel (&lnf);
        addAndMakeVisible (header);
        header.getPresetBrowser().setPresetList ({ "Init", "Punch", "Wide" }, { "User A" });

        addAndMakeVisible (section);
        section.setTitle ("Controls");

        knob.setLegend ("Drive");
        knob.setNormalisedValue (0.62f);
        knob.setDefaultNormalised (0.5f);
        addAndMakeVisible (knob);

        bipolar.setLegend ("Trim");
        bipolar.setBipolar (true, 0.5f);
        bipolar.setNormalisedValue (0.35f);
        addAndMakeVisible (bipolar);

        hold.setLegend ("Hold");
        hold.setToggleState (true);
        addAndMakeVisible (hold);

        bypassDemo.setLegend ("Mute");
        bypassDemo.setLedRole (fds::gui::LedRole::caution);
        addAndMakeVisible (bypassDemo);

        primary.getProperties().set (fds::gui::FactoryLookAndFeel::buttonVariantProperty, "primary");
        primary.setButtonText ("Render");
        addAndMakeVisible (primary);

        secondary.getProperties().set (fds::gui::FactoryLookAndFeel::buttonVariantProperty, "secondary");
        secondary.setButtonText ("Reset");
        addAndMakeVisible (secondary);

        brandBtn.getProperties().set (fds::gui::FactoryLookAndFeel::buttonVariantProperty, "brand");
        brandBtn.setButtonText ("Selected");
        addAndMakeVisible (brandBtn);

        readout.setFromDb (-8.0f);
        addAndMakeVisible (readout);

        meter.setLevels (-6.5f, -14.0f, -3.0f, true);
        addAndMakeVisible (meter);
        addAndMakeVisible (scale);

        themeInfo.setText ("Theme: " + juce::String (fds::themeId) + " / " + juce::String (fds::themeName)
                               + " v" + juce::String (fds::themeVersion),
                           juce::dontSendNotification);
        themeInfo.setColour (juce::Label::textColourId, fds::gui::argb (fds::color::text::secondary));
        themeInfo.setFont (fds::gui::makeFont (fds::typography::style::body));
        addAndMakeVisible (themeInfo);

        setSize (720, 480);
    }

    ~GalleryComponent() override { setLookAndFeel (nullptr); }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (fds::gui::argb (fds::color::bg::panel));
    }

    void resized() override
    {
        auto r = getLocalBounds();
        header.setBounds (r.removeFromTop (fds::size::headerHeight));
        themeInfo.setBounds (r.removeFromBottom (24).reduced (16, 0));
        auto body = r.reduced (fds::size::windowPadding);
        section.setBounds (body);
        auto c = section.getContentArea();
        knob.setBounds (c.removeFromLeft (90).removeFromTop (110));
        c.removeFromLeft (12);
        bipolar.setBounds (c.removeFromLeft (90).removeFromTop (110));
        c.removeFromLeft (16);
        auto toggles = c.removeFromLeft (110);
        hold.setBounds (toggles.removeFromTop (28));
        toggles.removeFromTop (8);
        bypassDemo.setBounds (toggles.removeFromTop (28));
        toggles.removeFromTop (12);
        primary.setBounds (toggles.removeFromTop (28));
        toggles.removeFromTop (6);
        secondary.setBounds (toggles.removeFromTop (28));
        toggles.removeFromTop (6);
        brandBtn.setBounds (toggles.removeFromTop (28));
        c.removeFromLeft (16);
        readout.setBounds (c.removeFromLeft (120).removeFromTop (32));
        c.removeFromLeft (12);
        meter.setBounds (c.removeFromLeft (24).removeFromTop (160));
        scale.setBounds (c.removeFromLeft (40).removeFromTop (160));
    }

private:
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::HeaderBar header { "MeterTap", "0.2.0" };
    fds::gui::SectionPanel section;
    fds::gui::RotaryKnob knob { "Drive", fds::gui::KnobSize::md };
    fds::gui::RotaryKnob bipolar { "Trim", fds::gui::KnobSize::md };
    fds::gui::LedToggle hold { "Hold", fds::gui::LedRole::standard };
    fds::gui::LedToggle bypassDemo { "Mute", fds::gui::LedRole::caution };
    juce::TextButton primary, secondary, brandBtn;
    fds::gui::ValueReadout readout;
    fds::gui::LevelMeter meter { fds::gui::MeterBarSize::md };
    fds::gui::MeterScale scale;
    juce::Label themeInfo;
};

class GalleryApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "FactoryGuiGallery"; }
    const juce::String getApplicationVersion() override { return "0.2.0"; }
    void initialise (const juce::String&) override
    {
        mainWindow = std::make_unique<juce::DocumentWindow> (
            "Factory GUI Gallery — " + juce::String (fds::themeId),
            fds::gui::argb (fds::color::bg::chassis),
            juce::DocumentWindow::allButtons);
        mainWindow->setUsingNativeTitleBar (true);
        mainWindow->setContentOwned (new GalleryComponent(), true);
        mainWindow->centreWithSize (720, 480);
        mainWindow->setVisible (true);
    }
    void shutdown() override { mainWindow = nullptr; }

private:
    std::unique_ptr<juce::DocumentWindow> mainWindow;
};

START_JUCE_APPLICATION (GalleryApp)
