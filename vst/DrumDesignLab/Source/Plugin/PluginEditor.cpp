#include "PluginEditor.h"
#include "../Presets/GenreBankLibrary.h"

namespace
{
constexpr const char* kPadNames[16] = {
    "Kick", "Rim", "Snare", "Clap", "808", "Perc", "CH", "808B",
    "CH2", "Tom", "OH", "FX", "Kick2", "Ride", "Snare2", "Impact"
};
} // namespace

DrumDesignLabAudioProcessorEditor::DrumDesignLabAudioProcessorEditor(DrumDesignLabAudioProcessor& p)
    : AudioProcessorEditor(&p), proc_(p)
{
    setSize(1280, 800);
    setResizable(true, true);
    setResizeLimits(960, 620, 1920, 1200);

    title_.setText("DiskLordz", juce::dontSendNotification);
    title_.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    title_.setColour(juce::Label::textColourId, juce::Colour(0xfff4b35b));
    addAndMakeVisible(title_);

    subtitle_.setText("Drum Kit Factory", juce::dontSendNotification);
    subtitle_.setFont(juce::FontOptions(16.0f));
    subtitle_.setColour(juce::Label::textColourId, juce::Colour(0xffc8c8d0));
    addAndMakeVisible(subtitle_);

    formulaLabel_.setText("Kit Formula", juce::dontSendNotification);
    formulaLabel_.setColour(juce::Label::textColourId, juce::Colour(0xfff4b35b));
    addAndMakeVisible(formulaLabel_);
    for (int i = 0; i < disklordz::genre_bank::kFormulaCount; ++i)
        formulaBox_.addItem(disklordz::genre_bank::formulaAt(i).displayName, i + 1);
    addAndMakeVisible(formulaBox_);
    formulaAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_dna_formula", formulaBox_);
    formulaBox_.onChange = [this] {
        // Selecting a formula forces DNA bank mode (all 16 pads)
        proc_.loadDnaFormula(formulaBox_.getSelectedItemIndex());
    };

    engineLabel_.setText("Producer Engine", juce::dontSendNotification);
    addAndMakeVisible(engineLabel_);
    for (int i = 0; i < static_cast<int>(disklordz::KitStyle::Count); ++i)
        engineBox_.addItem(disklordz::kitStyleName(static_cast<disklordz::KitStyle>(i)), i + 1);
    addAndMakeVisible(engineBox_);
    engineAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_kit_style", engineBox_);

    useEngineToggle_.setToggleState(false, juce::dontSendNotification);
    useEngineToggle_.onClick = [this] {
        if (useEngineToggle_.getToggleState())
        {
            // Leave DNA bank; use selected producer engine
            if (auto* eng = proc_.apvts.getParameter("ddl_kit_style"))
            {
                // Trigger style apply via setKitStyle
                proc_.setKitStyle(static_cast<disklordz::KitStyle>(engineBox_.getSelectedItemIndex()));
            }
        }
        else
        {
            proc_.loadDnaFormula(formulaBox_.getSelectedItemIndex());
        }
    };
    addAndMakeVisible(useEngineToggle_);

    gainLabel_.setText("Output", juce::dontSendNotification);
    addAndMakeVisible(gainLabel_);
    gainSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 52, 18);
    addAndMakeVisible(gainSlider_);
    gainAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_output_gain", gainSlider_);

    factoryTitle_.setText("Build Pack", juce::dontSendNotification);
    factoryTitle_.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    factoryTitle_.setColour(juce::Label::textColourId, juce::Colour(0xff8fd49a));
    addAndMakeVisible(factoryTitle_);

    variationsLabel_.setText("Variations", juce::dontSendNotification);
    addAndMakeVisible(variationsLabel_);
    variationsSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    variationsSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 18);
    variationsSlider_.setRange(1.0, 8.0, 1.0);
    addAndMakeVisible(variationsSlider_);
    variationsAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_factory_variations", variationsSlider_);

    exportCurrentBtn_.setColour(juce::TextButton::buttonColourId, juce::Colour(0xfff4b35b));
    exportCurrentBtn_.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    exportCurrentBtn_.onClick = [this] { exportCurrentKit(); };
    addAndMakeVisible(exportCurrentBtn_);

    exportAllBtn_.onClick = [this] { exportAllKits(); };
    addAndMakeVisible(exportAllBtn_);

    qaLabel_.setColour(juce::Label::textColourId, juce::Colour(0xffa0a0a8));
    qaLabel_.setJustificationType(juce::Justification::topLeft);
    addAndMakeVisible(qaLabel_);

    statusLabel_.setColour(juce::Label::textColourId, juce::Colour(0xffc8c8d0));
    addAndMakeVisible(statusLabel_);

    buildPads();
    // Default product path: DNA kit formula (all pads work)
    proc_.loadDnaFormula(formulaBox_.getSelectedItemIndex() >= 0 ? formulaBox_.getSelectedItemIndex() : 8);
    refreshPadHighlights();
    startTimerHz(15);
}

void DrumDesignLabAudioProcessorEditor::buildPads()
{
    for (int i = 0; i < 16; ++i)
    {
        pads_[static_cast<size_t>(i)].setButtonText(kPadNames[i]);
        pads_[static_cast<size_t>(i)].onClick = [this, i] {
            proc_.selectPad(i, true);
            refreshPadHighlights();
        };
        addAndMakeVisible(pads_[static_cast<size_t>(i)]);
    }
}

void DrumDesignLabAudioProcessorEditor::refreshPadHighlights()
{
    const int sel = proc_.getSelectedPad();
    for (int i = 0; i < 16; ++i)
    {
        auto& b = pads_[static_cast<size_t>(i)];
        const bool on = (i == sel);
        b.setColour(juce::TextButton::buttonColourId,
                    on ? juce::Colour(0xfff4b35b) : juce::Colour(0xff2a2a32));
        b.setColour(juce::TextButton::textColourOffId,
                    on ? juce::Colours::black : juce::Colours::white);
    }
}

void DrumDesignLabAudioProcessorEditor::exportCurrentKit()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Export kit pack",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("DiskLordz_Kits"),
        "*");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                         [this, chooser](const juce::FileChooser& fc) {
                             const auto dir = fc.getResult();
                             if (dir == juce::File())
                                 return;
                             const int vars = static_cast<int>(
                                 proc_.apvts.getRawParameterValue("ddl_factory_variations")->load());
                             qaLabel_.setText(proc_.exportKitPack(dir, false, vars), juce::dontSendNotification);
                         });
}

void DrumDesignLabAudioProcessorEditor::exportAllKits()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Export all 32 DNA kits",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("DiskLordz_Kits"),
        "*");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                         [this, chooser](const juce::FileChooser& fc) {
                             const auto dir = fc.getResult();
                             if (dir == juce::File())
                                 return;
                             const int vars = static_cast<int>(
                                 proc_.apvts.getRawParameterValue("ddl_factory_variations")->load());
                             qaLabel_.setText(proc_.exportKitPack(dir, true, vars), juce::dontSendNotification);
                         });
}

void DrumDesignLabAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101014));
    auto bounds = getLocalBounds().toFloat().reduced(10.0f);
    g.setColour(juce::Colour(0xff18181e));
    g.fillRoundedRectangle(bounds, 12.0f);
    g.setColour(juce::Colour(0xff2a2a32));
    g.drawRoundedRectangle(bounds, 12.0f, 1.0f);

    // Factory panel backdrop
    auto r = getLocalBounds().reduced(16);
    r.removeFromTop(100);
    auto right = r.removeFromRight(r.getWidth() / 2).reduced(8);
    g.setColour(juce::Colour(0xff14141a));
    g.fillRoundedRectangle(right.toFloat(), 10.0f);
}

void DrumDesignLabAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(20);
    auto header = r.removeFromTop(36);
    title_.setBounds(header.removeFromLeft(160));
    subtitle_.setBounds(header.removeFromLeft(180));
    header.removeFromLeft(12);
    gainLabel_.setBounds(header.removeFromLeft(50));
    gainSlider_.setBounds(header.removeFromLeft(180));

    r.removeFromTop(8);
    auto row = r.removeFromTop(32);
    formulaLabel_.setBounds(row.removeFromLeft(90));
    formulaBox_.setBounds(row.removeFromLeft(320));
    row.removeFromLeft(16);
    engineLabel_.setBounds(row.removeFromLeft(110));
    engineBox_.setBounds(row.removeFromLeft(240));
    row.removeFromLeft(12);
    useEngineToggle_.setBounds(row);

    r.removeFromTop(8);
    statusLabel_.setBounds(r.removeFromTop(22));
    r.removeFromTop(8);

    auto left = r.removeFromLeft(r.getWidth() / 2).reduced(4);
    auto right = r.reduced(8);

    const int cols = 4;
    const int rows = 4;
    const int padW = left.getWidth() / cols;
    const int padH = left.getHeight() / rows;
    for (int i = 0; i < 16; ++i)
    {
        const int c = i % cols;
        const int rw = i / cols;
        pads_[static_cast<size_t>(i)].setBounds(left.getX() + c * padW + 3,
                                                left.getY() + rw * padH + 3,
                                                padW - 6, padH - 6);
    }

    factoryTitle_.setBounds(right.removeFromTop(28));
    right.removeFromTop(8);
    auto varRow = right.removeFromTop(28);
    variationsLabel_.setBounds(varRow.removeFromLeft(80));
    variationsSlider_.setBounds(varRow);
    right.removeFromTop(16);
    exportCurrentBtn_.setBounds(right.removeFromTop(44));
    right.removeFromTop(10);
    exportAllBtn_.setBounds(right.removeFromTop(36));
    right.removeFromTop(16);
    qaLabel_.setBounds(right);
}

void DrumDesignLabAudioProcessorEditor::timerCallback()
{
    if (proc_.getSelectedPad() != lastPad_)
    {
        lastPad_ = proc_.getSelectedPad();
        refreshPadHighlights();
    }
    const auto& f = disklordz::genre_bank::formulaAt(proc_.getDnaFormula());
    statusLabel_.setText(
        juce::String("Selected: ") + kPadNames[proc_.getSelectedPad()] + "  ·  " + f.displayName
            + (proc_.engineUsesThreeLayerPads() ? "  ·  DNA kit (16 pads)" : "  ·  producer engine"),
        juce::dontSendNotification);
}
