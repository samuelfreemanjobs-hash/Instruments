#include "PluginEditor.h"

#include "ParameterIds.h"
#include "Presets/FactoryPresets.h"
#include "Presets/PresetManager.h"
#include "Presets/FeaturedPresets.h"
#include "Presets/UserPresetStore.h"

namespace
{
namespace PID = prophetrev2::ParameterIDs;
// Right preset column anchors at (width - kRightPanelInset). Left chrome needs ~520px
// (title, subtitle, output/mono/unison row) — 850px was too narrow and overlapped controls.
constexpr int kRightPanelInset = 420;
constexpr int kLeftChromeMinWidth = 540;
constexpr int kMinWidth = kRightPanelInset + kLeftChromeMinWidth;
constexpr int kDefaultWidth = kMinWidth + 40;
constexpr int kHeight = 600;

juce::String categoryRoleHint (const juce::String& category)
{
    if (category == "Bass") return "Sub weight · single notes";
    if (category == "Synth") return "Poly chords & melodic beds";
    if (category == "Lead") return "Hooks · mono / top line";
    if (category == "Pad") return "Atmosphere · slow textures";
    if (category == "Pluck/Keys") return "Short rhythmic stabs · keys";
    if (category == prophetrev2::presets::kUserPresetCategory) return "Your saved patches";
    return {};
}

juce::Colour categoryAccentColour (const juce::String& category)
{
    if (category == "Bass") return juce::Colour (0xffe8a035);
    if (category == "Synth") return juce::Colour (0xff3ecfd4);
    if (category == "Lead") return juce::Colour (0xfff0c040);
    if (category == "Pad") return juce::Colour (0xff9b7ed9);
    if (category == "Pluck/Keys") return juce::Colour (0xff5ecf8a);
    return juce::Colour (0xff9090a8);
}
} // namespace

ProphetRev2TrapAudioProcessorEditor::ProphetRev2TrapAudioProcessorEditor (ProphetRev2TrapAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    for (auto* l : { &categoryLabel_, &presetLabel_, &categoryHintLabel_, &ampEnvLabel_, &filtEnvLabel_, &analogLabel_, &presetCountLabel_ })
        addAndMakeVisible (*l);

    presetCountLabel_.setFont (juce::Font (11.0f));
    presetCountLabel_.setColour (juce::Label::textColourId, juce::Colour (0xff808098));
    featuredOnlyToggle_.setToggleState (true, juce::dontSendNotification);
    featuredOnlyToggle_.onClick = [this] { rebuildFilteredPresetList (0); };
    addAndMakeVisible (featuredOnlyToggle_);

    addAndMakeVisible (categoryBox_);
    addAndMakeVisible (presetBox_);
    addAndMakeVisible (presetFilterLabel_);
    presetFilterEditor_.setTextToShowWhenEmpty ("Search presets…", juce::Colours::grey);
    presetFilterEditor_.onTextChange = [this] { rebuildFilteredPresetList (0); };
    addAndMakeVisible (presetFilterEditor_);
    userPresetNameEditor_.setText ("My Patch");
    userPresetNameEditor_.setFont (juce::Font (14.0f));
    addAndMakeVisible (userPresetNameEditor_);
    addAndMakeVisible (saveUserButton_);
    addAndMakeVisible (monoButton_);
    addAndMakeVisible (legatoButton_);
    addAndMakeVisible (unisonBox_);
    addAndMakeVisible (outputSlider_);

    categoryHintLabel_.setFont (juce::Font (12.0f));
    categoryHintLabel_.setColour (juce::Label::textColourId, juce::Colour (0xff9090a8));

    for (const auto& cat : prophetrev2::presets::PresetManager::getCategoryOrder())
        categoryBox_.addItem (cat, categoryBox_.getNumItems() + 1);

    categoryBox_.onChange = [this] {
        updateCategoryHint();
        const bool userCat = prophetrev2::presets::PresetManager::isUserCategory (categoryBox_.getText());
        featuredOnlyToggle_.setEnabled (! userCat);
        refreshPresetListForCategory (categoryBox_.getText(), 0);
        if (! prophetrev2::presets::PresetManager::isUserCategory (categoryBox_.getText())
            && ! filteredPresetGlobals_.isEmpty())
            processor_.applyFactoryPreset (filteredPresetGlobals_[0].getIntValue());
    };

    presetBox_.onChange = [this] { onPresetSelected(); };

    saveUserButton_.onClick = [this] { promptSaveUserPreset(); };

    auto& apvts = processor_.getApvts();
    outputAttachment_ = std::make_unique<SliderAttachment> (apvts, PID::outputGain, outputSlider_);
    monoAttachment_ = std::make_unique<ButtonAttachment> (apvts, PID::monoMode, monoButton_);
    legatoAttachment_ = std::make_unique<ButtonAttachment> (apvts, PID::legatoMode, legatoButton_);
    unisonAttachment_ = std::make_unique<ComboAttachment> (apvts, PID::unisonVoices, unisonBox_);

    const struct KnobDef { const char* id; const char* label; } defs[] = {
        { PID::osc1Level, "Osc 1" }, { PID::osc2Level, "Osc 2" }, { PID::osc2Detune, "Detune" },
        { PID::oscMix, "Mix" }, { PID::filterCutoff, "Cutoff" }, { PID::filterRes, "Res" },
        { PID::filtEnvAmt, "F.Env" }, { PID::keyTrack, "Key Trk" }, { PID::filterDrive, "F.Drive" },
        { PID::circuitDrive, "Circuit" }, { PID::unisonSpread, "Spread" }, { PID::glideMs, "Glide" },
        { PID::ampAttack, "Amp A" }, { PID::ampDecay, "Amp D" }, { PID::ampSustain, "Amp S" }, { PID::ampRelease, "Amp R" },
        { PID::filtAttack, "Flt A" }, { PID::filtDecay, "Flt D" }, { PID::filtSustain, "Flt S" }, { PID::filtRelease, "Flt R" },
    };
    for (const auto& d : defs)
        addKnob (d.id, d.label);

    syncUiToCurrentProgram();
    updateCategoryHint();
    setResizeLimits (kMinWidth, kHeight, 2000, kHeight);
    setSize (kDefaultWidth, kHeight);
}

ProphetRev2TrapAudioProcessorEditor::~ProphetRev2TrapAudioProcessorEditor() = default;

void ProphetRev2TrapAudioProcessorEditor::addKnob (const char* paramId, const juce::String& label)
{
    auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    addAndMakeVisible (*slider);
    knobAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, *slider));
    knobs_.push_back (std::move (slider));

    auto lab = std::make_unique<juce::Label> (label, label);
    lab->setJustificationType (juce::Justification::centred);
    addAndMakeVisible (*lab);
    knobLabels_.push_back (std::move (lab));
}

void ProphetRev2TrapAudioProcessorEditor::onPresetSelected()
{
    const juce::String cat = categoryBox_.getText();
    const int local = presetBox_.getSelectedItemIndex();
    if (local < 0)
        return;

    if (prophetrev2::presets::PresetManager::isUserCategory (cat))
    {
        processor_.applyUserPreset (presetBox_.getText());
        return;
    }

    if (local >= 0 && local < filteredPresetGlobals_.size())
        processor_.applyFactoryPreset (filteredPresetGlobals_[local].getIntValue());
}

void ProphetRev2TrapAudioProcessorEditor::rebuildFilteredPresetList (int selectLocalIndex)
{
    presetBox_.clear();
    filteredPresetGlobals_.clear();

    const juce::String cat = categoryBox_.getText();
    if (prophetrev2::presets::PresetManager::isUserCategory (cat))
    {
        const auto names = prophetrev2::presets::PresetManager::getPresetNamesForCategory (cat);
        for (int i = 0; i < names.size(); ++i)
            presetBox_.addItem (names[i], i + 1);
        if (names.isEmpty())
            presetBox_.addItem ("(empty)", 1);
        presetBox_.setSelectedItemIndex (juce::jlimit (0, juce::jmax (0, names.size() - 1), selectLocalIndex),
                                         juce::dontSendNotification);
        updatePresetBrowserSummary();
        return;
    }

    activeFactoryCategory_ = cat;
    const auto filter = presetFilterEditor_.getText().trim().toLowerCase();
    const auto& bank = prophetrev2::presets::getFactoryPresets();

    for (int i = 0; i < static_cast<int> (bank.size()); ++i)
    {
        if (bank[static_cast<std::size_t> (i)].category != cat.toStdString())
            continue;
        const juce::String name (bank[static_cast<std::size_t> (i)].name);
        if (featuredOnlyToggle_.getToggleState()
            && ! prophetrev2::presets::isFeaturedFactoryPreset (cat.toStdString(), name.toStdString()))
            continue;
        if (filter.isNotEmpty() && ! name.toLowerCase().contains (filter))
            continue;
        filteredPresetGlobals_.add (juce::String (i));
        presetBox_.addItem (name, presetBox_.getNumItems() + 1);
    }

    if (filteredPresetGlobals_.isEmpty())
        presetBox_.addItem ("(no match)", 1);

    presetBox_.setSelectedItemIndex (juce::jlimit (0, juce::jmax (0, filteredPresetGlobals_.size() - 1), selectLocalIndex),
                                     juce::dontSendNotification);
    updatePresetBrowserSummary();
}

void ProphetRev2TrapAudioProcessorEditor::updatePresetBrowserSummary()
{
    const juce::String cat = categoryBox_.getText();
    if (prophetrev2::presets::PresetManager::isUserCategory (cat))
    {
        presetCountLabel_.setText ("User bank", juce::dontSendNotification);
        return;
    }

    int totalInCategory = 0;
    for (const auto& p : prophetrev2::presets::getFactoryPresets())
        if (p.category == cat.toStdString())
            ++totalInCategory;

    juce::String summary = juce::String (filteredPresetGlobals_.size()) + " shown";
    if (totalInCategory > 0)
        summary += " of " + juce::String (totalInCategory);
    if (featuredOnlyToggle_.getToggleState())
        summary += " · featured";
    summary += " · " + cat;
    presetCountLabel_.setText (summary, juce::dontSendNotification);
}

juce::Colour ProphetRev2TrapAudioProcessorEditor::categoryAccentColour (const juce::String& category) const
{
    return ::categoryAccentColour (category);
}

void ProphetRev2TrapAudioProcessorEditor::promptSaveUserPreset()
{
    const auto name = userPresetNameEditor_.getText().trim();
    if (name.isEmpty())
        return;

    processor_.saveUserPreset (name);
    categoryBox_.setText (prophetrev2::presets::kUserPresetCategory, juce::dontSendNotification);
    refreshPresetListForCategory (prophetrev2::presets::kUserPresetCategory, 0);
    for (int i = 0; i < presetBox_.getNumItems(); ++i)
        if (presetBox_.getItemText (i) == name)
            presetBox_.setSelectedItemIndex (i, juce::dontSendNotification);
}

void ProphetRev2TrapAudioProcessorEditor::refreshPresetListForCategory (const juce::String& category,
                                                                        int selectLocalIndex)
{
    juce::ignoreUnused (category);
    rebuildFilteredPresetList (selectLocalIndex);
}

void ProphetRev2TrapAudioProcessorEditor::syncUiToCurrentProgram()
{
    if (processor_.getActiveUserPresetName().isNotEmpty())
    {
        categoryBox_.setText (prophetrev2::presets::kUserPresetCategory, juce::dontSendNotification);
        refreshPresetListForCategory (prophetrev2::presets::kUserPresetCategory, 0);
        for (int i = 0; i < presetBox_.getNumItems(); ++i)
            if (presetBox_.getItemText (i) == processor_.getActiveUserPresetName())
                presetBox_.setSelectedItemIndex (i, juce::dontSendNotification);
        return;
    }

    juce::String cat;
    int local = 0;
    prophetrev2::presets::PresetManager::getCategoryAndLocalIndex (processor_.getCurrentProgram(), cat, local);

    for (int i = 0; i < categoryBox_.getNumItems(); ++i)
        if (categoryBox_.getItemText (i) == cat)
            categoryBox_.setSelectedItemIndex (i, juce::dontSendNotification);

    rebuildFilteredPresetList (local);
    for (int i = 0; i < filteredPresetGlobals_.size(); ++i)
        if (filteredPresetGlobals_[i].getIntValue() == processor_.getCurrentProgram())
            presetBox_.setSelectedItemIndex (i, juce::dontSendNotification);
}

void ProphetRev2TrapAudioProcessorEditor::updateCategoryHint()
{
    categoryHintLabel_.setText (categoryRoleHint (categoryBox_.getText()), juce::dontSendNotification);
}

void ProphetRev2TrapAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0e0e14));

    const float headerW = static_cast<float> (getWidth()) - 16.0f;
    g.setColour (juce::Colour (0xff252532));
    g.fillRoundedRectangle (8.0f, 8.0f, headerW, 96.0f, 8.0f);

    const auto accent = categoryAccentColour (categoryBox_.getText());
    g.setColour (accent);
    g.fillRoundedRectangle (8.0f, 8.0f, 5.0f, 96.0f, 2.0f);

    const int panelX = getWidth() - kRightPanelInset + 70;
    g.setColour (juce::Colour (0xff1a1a24));
    g.fillRoundedRectangle (static_cast<float> (panelX), 12.0f,
                            static_cast<float> (getWidth() - panelX - 12), 82.0f, 6.0f);
    g.setColour (accent.withAlpha (0.85f));
    g.fillRect (static_cast<float> (panelX), 12.0f, 3.0f, 82.0f);
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (24.0f, juce::Font::bold));
    g.drawText ("Night Circuit", 20, 12, 240, 30, juce::Justification::left);
    g.setFont (13.0f);
    g.setColour (juce::Colour (0xffb0b0c8));
    g.drawText ("v0.1 · 1,028 factory programs · analog ladder · 16-voice poly", 20, 40, 480, 18, juce::Justification::left);
}

void ProphetRev2TrapAudioProcessorEditor::resized()
{
    const int w = getWidth();
    categoryLabel_.setBounds (w - kRightPanelInset, 18, 70, 22);
    categoryBox_.setBounds (w - kRightPanelInset + 80, 16, 150, 24);
    presetLabel_.setBounds (w - kRightPanelInset, 46, 52, 22);
    presetFilterLabel_.setBounds (w - kRightPanelInset + 80, 44, 44, 22);
    presetFilterEditor_.setBounds (w - kRightPanelInset + 130, 44, 100, 22);
    presetBox_.setBounds (w - kRightPanelInset + 80, 68, 150, 24);
    featuredOnlyToggle_.setBounds (w - kRightPanelInset + 80, 94, 150, 22);
    presetCountLabel_.setBounds (w - kRightPanelInset, 94, 230, 22);
    userPresetNameEditor_.setBounds (w - 180, 16, 160, 24);
    saveUserButton_.setBounds (w - 180, 44, 160, 26);

    categoryHintLabel_.setBounds (20, 72, getWidth() - 40, 16);
    outputSlider_.setBounds (20, 108, 280, 22);
    monoButton_.setBounds (320, 104, 70, 28);
    legatoButton_.setBounds (395, 104, 70, 28);
    unisonBox_.setBounds (480, 106, 80, 24);

    analogLabel_.setBounds (20, 134, 200, 18);
    const int kx = 20, ky = 154, kw = 68, kh = 78, cols = 6;
    for (std::size_t i = 0; i < knobs_.size() && i < 12; ++i)
    {
        const int col = static_cast<int> (i) % cols;
        const int row = static_cast<int> (i) / cols;
        knobs_[i]->setBounds (kx + col * (kw + 6), ky + row * (kh + 4), kw, kh);
        if (i < knobLabels_.size())
            knobLabels_[i]->setBounds (knobs_[i]->getX(), knobs_[i]->getBottom() - 2, kw, 14);
    }

    ampEnvLabel_.setBounds (20, 316, 120, 18);
    filtEnvLabel_.setBounds (20, 446, 140, 18);

    for (std::size_t i = 12; i < knobs_.size(); ++i)
    {
        const int idx = static_cast<int> (i - 12);
        const int col = idx % 4;
        const int row = idx / 4;
        const int baseY = (i < 16) ? 320 : 450;
        const int localRow = (i < 16) ? 0 : 1;
        juce::ignoreUnused (row, baseY);
        knobs_[i]->setBounds (20 + col * (kw + 8), (i < 16 ? 320 : 450) + localRow * 0, kw, kh);
        if (i < knobLabels_.size())
            knobLabels_[i]->setBounds (knobs_[i]->getX(), knobs_[i]->getBottom() - 2, kw, 14);
    }

    // Fix amp/filter rows layout
    for (int i = 0; i < 4; ++i)
    {
        knobs_[static_cast<std::size_t> (12 + i)]->setBounds (20 + i * (kw + 10), 338, kw, kh);
        knobLabels_[static_cast<std::size_t> (12 + i)]->setBounds (20 + i * (kw + 10), 414, kw, 14);
        knobs_[static_cast<std::size_t> (16 + i)]->setBounds (20 + i * (kw + 10), 468, kw, kh);
        knobLabels_[static_cast<std::size_t> (16 + i)]->setBounds (20 + i * (kw + 10), 544, kw, 14);
    }
}
