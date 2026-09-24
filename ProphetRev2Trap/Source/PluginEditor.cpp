#include "PluginEditor.h"

#include "ParameterIds.h"
#include "Presets/FactoryPresets.h"
#include "Presets/PresetManager.h"
#include "Presets/FeaturedPresets.h"
#include "Presets/FavoritesStore.h"
#include "Presets/UserPresetStore.h"

namespace
{
namespace PID = prophetrev2::ParameterIDs;

constexpr int kMinWidth = 1120;
constexpr int kMinHeight = 760;
constexpr int kDefaultWidth = 1280;
constexpr int kDefaultHeight = 820;
constexpr int kBrowserBandHeight = 118;
constexpr int kMargin = 12;

juce::Colour colourGraphite() { return juce::Colour (0xff10101a); }
juce::Colour colourPanel() { return juce::Colour (0xff272236); }
juce::Colour colourViolet() { return juce::Colour (0xffb77aec); }
juce::Colour colourTextPrimary() { return juce::Colour (0xfff5effa); }
juce::Colour colourTextMuted() { return juce::Colour (0xffaa9dbb); }

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

void styleCombo (juce::ComboBox& box)
{
    box.setColour (juce::ComboBox::backgroundColourId, colourPanel().brighter (0.08f));
    box.setColour (juce::ComboBox::textColourId, colourTextPrimary());
    box.setColour (juce::ComboBox::outlineColourId, colourViolet().withAlpha (0.35f));
}

void styleToggle (juce::ToggleButton& t)
{
    t.setColour (juce::ToggleButton::textColourId, colourTextMuted());
    t.setColour (juce::ToggleButton::tickColourId, colourViolet());
}

void styleLabel (juce::Label& l, bool muted = false)
{
    l.setColour (juce::Label::textColourId, muted ? colourTextMuted() : colourTextPrimary());
}

} // namespace

ProphetRev2TrapAudioProcessorEditor::ProphetRev2TrapAudioProcessorEditor (ProphetRev2TrapAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    for (auto* l : { &categoryLabel_, &presetLabel_, &categoryHintLabel_, &oscSectionLabel_, &ampEnvLabel_,
                     &filtEnvLabel_, &perfSectionLabel_, &presetCountLabel_, &activePresetNameLabel_,
                     &voiceModeLabel_ })
    {
        addAndMakeVisible (*l);
        styleLabel (*l, l == &presetCountLabel_ || l == &categoryHintLabel_ || l == &voiceModeLabel_);
    }

    modifiedBadgeLabel_.setColour (juce::Label::textColourId, colourViolet());
    modifiedBadgeLabel_.setFont (juce::Font (12.0f, juce::Font::bold));
    modifiedBadgeLabel_.setVisible (false);
    addAndMakeVisible (modifiedBadgeLabel_);

    activePresetNameLabel_.setFont (juce::Font (18.0f, juce::Font::bold));
    presetCountLabel_.setFont (juce::Font (12.0f));

    featuredOnlyToggle_.setToggleState (true, juce::dontSendNotification);
    featuredOnlyToggle_.onClick = [this] { rebuildFilteredPresetList (0); };
    favoritesOnlyToggle_.onClick = [this] { rebuildFilteredPresetList (0); };
    styleToggle (featuredOnlyToggle_);
    styleToggle (favoritesOnlyToggle_);
    addAndMakeVisible (featuredOnlyToggle_);
    addAndMakeVisible (favoritesOnlyToggle_);

    styleCombo (categoryBox_);
    styleCombo (presetBox_);
    addAndMakeVisible (categoryBox_);
    addAndMakeVisible (presetBox_);
    addAndMakeVisible (presetFilterLabel_);
    presetFilterEditor_.setTextToShowWhenEmpty ("Search presets…", colourTextMuted());
    presetFilterEditor_.setColour (juce::TextEditor::backgroundColourId, colourPanel().brighter (0.06f));
    presetFilterEditor_.setColour (juce::TextEditor::textColourId, colourTextPrimary());
    presetFilterEditor_.onTextChange = [this] { rebuildFilteredPresetList (0); };
    addAndMakeVisible (presetFilterEditor_);

    prevPresetButton_.onClick = [this] { stepPreset (-1); };
    nextPresetButton_.onClick = [this] { stepPreset (1); };
    favoriteStarButton_.onClick = [this] { toggleFavoriteForCurrent(); };
    addAndMakeVisible (prevPresetButton_);
    addAndMakeVisible (nextPresetButton_);
    addAndMakeVisible (favoriteStarButton_);

    userPresetNameEditor_.setText ("My Patch");
    userPresetNameEditor_.setFont (juce::Font (14.0f));
    addAndMakeVisible (userPresetNameEditor_);
    addAndMakeVisible (saveUserButton_);
    saveUserButton_.onClick = [this] { promptSaveUserPreset(); };

    addAndMakeVisible (monoButton_);
    addAndMakeVisible (legatoButton_);
    styleToggle (monoButton_);
    styleToggle (legatoButton_);
    addAndMakeVisible (unisonBox_);
    addAndMakeVisible (outputSlider_);

    for (const auto& cat : prophetrev2::presets::PresetManager::getCategoryOrder())
        categoryBox_.addItem (cat, categoryBox_.getNumItems() + 1);

    categoryBox_.onChange = [this] {
        updateCategoryHint();
        const bool userCat = prophetrev2::presets::PresetManager::isUserCategory (categoryBox_.getText());
        featuredOnlyToggle_.setEnabled (! userCat);
        favoritesOnlyToggle_.setEnabled (! userCat);
        refreshPresetListForCategory (categoryBox_.getText(), 0);
        if (! userCat && ! filteredPresetGlobals_.isEmpty())
            processor_.applyFactoryPreset (filteredPresetGlobals_[0].getIntValue());
    };

    presetBox_.onChange = [this] { onPresetSelected(); };

    auto& apvts = processor_.getApvts();
    outputAttachment_ = std::make_unique<SliderAttachment> (apvts, PID::outputGain, outputSlider_);
    monoAttachment_ = std::make_unique<ButtonAttachment> (apvts, PID::monoMode, monoButton_);
    legatoAttachment_ = std::make_unique<ButtonAttachment> (apvts, PID::legatoMode, legatoButton_);
    unisonAttachment_ = std::make_unique<ComboAttachment> (apvts, PID::unisonVoices, unisonBox_);
    monoButton_.onClick = [this] { updateVoiceModeLabel(); };

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

    categoryHintLabel_.setFont (juce::Font (12.0f));
    oscSectionLabel_.setFont (juce::Font (13.0f, juce::Font::bold));
    ampEnvLabel_.setFont (juce::Font (13.0f, juce::Font::bold));
    filtEnvLabel_.setFont (juce::Font (13.0f, juce::Font::bold));
    perfSectionLabel_.setFont (juce::Font (13.0f, juce::Font::bold));

    syncUiToCurrentProgram();
    updateCategoryHint();
    updateActivePresetHeader();
    updateFavoriteStar();
    updateVoiceModeLabel();

    setResizeLimits (kMinWidth, kMinHeight, 2400, 1200);
    setSize (kDefaultWidth, kDefaultHeight);
    startTimerHz (8);
}

ProphetRev2TrapAudioProcessorEditor::~ProphetRev2TrapAudioProcessorEditor() = default;

void ProphetRev2TrapAudioProcessorEditor::timerCallback()
{
    const bool mod = processor_.isPresetModified();
    if (modifiedBadgeLabel_.isVisible() != mod)
        modifiedBadgeLabel_.setVisible (mod);
    updateVoiceModeLabel();
}

void ProphetRev2TrapAudioProcessorEditor::addKnob (const char* paramId, const juce::String& label)
{
    auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    slider->setColour (juce::Slider::rotarySliderFillColourId, colourViolet());
    slider->setColour (juce::Slider::thumbColourId, colourViolet());
    slider->setColour (juce::Slider::textBoxTextColourId, colourTextPrimary());
    addAndMakeVisible (*slider);
    knobAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, *slider));
    knobs_.push_back (std::move (slider));

    auto lab = std::make_unique<juce::Label> (label, label);
    lab->setJustificationType (juce::Justification::centred);
    lab->setColour (juce::Label::textColourId, colourTextMuted());
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
        updateActivePresetHeader();
        updateFavoriteStar();
        return;
    }

    if (local >= 0 && local < filteredPresetGlobals_.size())
        processor_.applyFactoryPreset (filteredPresetGlobals_[local].getIntValue());

    updateActivePresetHeader();
    updateFavoriteStar();
}

void ProphetRev2TrapAudioProcessorEditor::stepPreset (int delta)
{
    if (filteredPresetGlobals_.isEmpty())
        return;

    int local = presetBox_.getSelectedItemIndex();
    if (local < 0)
        local = 0;
    local = juce::jlimit (0, filteredPresetGlobals_.size() - 1, local + delta);
    presetBox_.setSelectedItemIndex (local, juce::sendNotificationSync);
}

void ProphetRev2TrapAudioProcessorEditor::toggleFavoriteForCurrent()
{
    const juce::String cat = categoryBox_.getText();
    if (prophetrev2::presets::PresetManager::isUserCategory (cat))
    {
        const auto name = presetBox_.getText();
        const bool next = ! prophetrev2::presets::FavoritesStore::isUserFavorite (name);
        prophetrev2::presets::FavoritesStore::setUserFavorite (name, next);
    }
    else
    {
        const int local = presetBox_.getSelectedItemIndex();
        if (local < 0 || local >= filteredPresetGlobals_.size())
            return;
        const int global = filteredPresetGlobals_[local].getIntValue();
        const bool next = ! prophetrev2::presets::FavoritesStore::isFactoryFavorite (global);
        prophetrev2::presets::FavoritesStore::setFactoryFavorite (global, next);
    }
    updateFavoriteStar();
    if (favoritesOnlyToggle_.getToggleState())
        rebuildFilteredPresetList (presetBox_.getSelectedItemIndex());
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
        {
            if (favoritesOnlyToggle_.getToggleState()
                && ! prophetrev2::presets::FavoritesStore::isUserFavorite (names[i]))
                continue;
            presetBox_.addItem (names[i], presetBox_.getNumItems() + 1);
        }
        if (presetBox_.getNumItems() == 0)
            presetBox_.addItem ("(empty)", 1);
        presetBox_.setSelectedItemIndex (juce::jlimit (0, juce::jmax (0, presetBox_.getNumItems() - 1), selectLocalIndex),
                                         juce::dontSendNotification);
        updatePresetBrowserSummary();
        updateActivePresetHeader();
        updateFavoriteStar();
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
        if (favoritesOnlyToggle_.getToggleState()
            && ! prophetrev2::presets::FavoritesStore::isFactoryFavorite (i))
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
    updateActivePresetHeader();
    updateFavoriteStar();
}

void ProphetRev2TrapAudioProcessorEditor::updatePresetBrowserSummary()
{
    const juce::String cat = categoryBox_.getText();
    if (prophetrev2::presets::PresetManager::isUserCategory (cat))
    {
        presetCountLabel_.setText ("User bank · saved on disk", juce::dontSendNotification);
        return;
    }

    int totalInCategory = 0;
    for (const auto& p : prophetrev2::presets::getFactoryPresets())
        if (p.category == cat.toStdString())
            ++totalInCategory;

    juce::String summary = juce::String (totalInCategory) + " " + cat + " presets · "
                           + juce::String (prophetrev2::presets::kFactoryPresetCount) + " total";
    if (featuredOnlyToggle_.getToggleState() || favoritesOnlyToggle_.getToggleState())
        summary += " · " + juce::String (filteredPresetGlobals_.size()) + " shown";
    presetCountLabel_.setText (summary, juce::dontSendNotification);
}

void ProphetRev2TrapAudioProcessorEditor::updateActivePresetHeader()
{
    juce::String name;
    if (processor_.getActiveUserPresetName().isNotEmpty())
        name = processor_.getActiveUserPresetName();
    else if (presetBox_.getSelectedItemIndex() >= 0)
        name = presetBox_.getText();
    else
        name = processor_.getProgramName (processor_.getCurrentProgram());

    activePresetNameLabel_.setText (name, juce::dontSendNotification);
}

void ProphetRev2TrapAudioProcessorEditor::updateFavoriteStar()
{
    const juce::String cat = categoryBox_.getText();
    bool fav = false;
    if (prophetrev2::presets::PresetManager::isUserCategory (cat))
        fav = prophetrev2::presets::FavoritesStore::isUserFavorite (presetBox_.getText());
    else
    {
        const int local = presetBox_.getSelectedItemIndex();
        if (local >= 0 && local < filteredPresetGlobals_.size())
            fav = prophetrev2::presets::FavoritesStore::isFactoryFavorite (filteredPresetGlobals_[local].getIntValue());
    }
    favoriteStarButton_.setButtonText (fav ? "★ Fav" : "☆ Star");
    favoriteStarButton_.setToggleState (fav, juce::dontSendNotification);
}

void ProphetRev2TrapAudioProcessorEditor::updateVoiceModeLabel()
{
    voiceModeLabel_.setText (monoButton_.getToggleState() ? "Mono + glide" : "Poly 16",
                             juce::dontSendNotification);
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
    processor_.capturePresetBaseline();
    categoryBox_.setText (prophetrev2::presets::kUserPresetCategory, juce::dontSendNotification);
    refreshPresetListForCategory (prophetrev2::presets::kUserPresetCategory, 0);
    for (int i = 0; i < presetBox_.getNumItems(); ++i)
        if (presetBox_.getItemText (i) == name)
            presetBox_.setSelectedItemIndex (i, juce::dontSendNotification);
    updateActivePresetHeader();
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

void ProphetRev2TrapAudioProcessorEditor::layoutKnobGrid (juce::Rectangle<int> area,
                                                          std::size_t startIndex,
                                                          int count,
                                                          int columns)
{
    const int kw = juce::jmax (56, area.getWidth() / columns - 8);
    const int kh = juce::jmin (88, area.getHeight() - 4);
    for (int i = 0; i < count; ++i)
    {
        const auto idx = startIndex + static_cast<std::size_t> (i);
        if (idx >= knobs_.size())
            break;
        const int col = i % columns;
        const int row = i / columns;
        const int x = area.getX() + col * (kw + 8);
        const int y = area.getY() + row * (kh + 18);
        knobs_[idx]->setBounds (x, y, kw, kh);
        if (idx < knobLabels_.size())
            knobLabels_[idx]->setBounds (x, y + kh - 4, kw, 14);
    }
}

void ProphetRev2TrapAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (colourGraphite());

    const auto accent = categoryAccentColour (categoryBox_.getText());
    const float w = static_cast<float> (getWidth());
    g.setColour (colourPanel());
    g.fillRoundedRectangle (kMargin, kMargin, w - 2.0f * kMargin, static_cast<float> (kBrowserBandHeight), 10.0f);
    g.setColour (accent);
    g.fillRoundedRectangle (kMargin, kMargin, 6.0f, static_cast<float> (kBrowserBandHeight), 3.0f);

    const int contentTop = kMargin + kBrowserBandHeight + 8;
    const int contentH = getHeight() - contentTop - kMargin;
    const int leftW = juce::roundToInt ((getWidth() - 2 * kMargin) * 0.58f);
    const int rightW = getWidth() - 2 * kMargin - leftW - 8;

    auto drawCard = [&] (int x, int y, int cw, int ch) {
        g.setColour (colourPanel().darker (0.12f));
        g.fillRoundedRectangle (static_cast<float> (x), static_cast<float> (y),
                                static_cast<float> (cw), static_cast<float> (ch), 8.0f);
        g.setColour (colourViolet().withAlpha (0.25f));
        g.drawRoundedRectangle (static_cast<float> (x), static_cast<float> (y),
                                static_cast<float> (cw), static_cast<float> (ch), 8.0f, 1.0f);
    };

    drawCard (kMargin, contentTop, leftW, juce::roundToInt (contentH * 0.52f));
    drawCard (kMargin, contentTop + juce::roundToInt (contentH * 0.54f), leftW, juce::roundToInt (contentH * 0.22f));
    drawCard (kMargin, contentTop + juce::roundToInt (contentH * 0.78f), leftW, contentH - juce::roundToInt (contentH * 0.78f));
    drawCard (kMargin + leftW + 8, contentTop, rightW, contentH);

    g.setColour (colourTextPrimary());
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawText ("Night Circuit", kMargin + 16, kMargin + 8, 220, 28, juce::Justification::left);
    g.setFont (12.5f);
    g.setColour (colourTextMuted());
    g.drawText ("VST3 · analog ladder · 1,028 factory programs", kMargin + 16, kMargin + 34, 420, 18,
                juce::Justification::left);
}

void ProphetRev2TrapAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int bandY = kMargin + 52;
    const int row2 = bandY + 28;

    categoryLabel_.setBounds (kMargin + 14, bandY, 64, 20);
    categoryBox_.setBounds (kMargin + 78, bandY - 2, 130, 26);
    presetLabel_.setBounds (kMargin + 220, bandY, 48, 20);
    presetBox_.setBounds (kMargin + 270, bandY - 2, juce::jmin (280, w / 3), 26);

    presetFilterLabel_.setBounds (kMargin + 560, bandY, 52, 20);
    presetFilterEditor_.setBounds (kMargin + 612, bandY - 2, 140, 26);

    prevPresetButton_.setBounds (w - kMargin - 380, bandY - 2, 32, 26);
    nextPresetButton_.setBounds (w - kMargin - 344, bandY - 2, 32, 26);
    favoriteStarButton_.setBounds (w - kMargin - 306, bandY - 2, 72, 26);
    featuredOnlyToggle_.setBounds (w - kMargin - 228, bandY - 2, 88, 26);
    favoritesOnlyToggle_.setBounds (w - kMargin - 134, bandY - 2, 88, 26);

    saveUserButton_.setBounds (w - kMargin - 130, row2, 118, 26);
    userPresetNameEditor_.setBounds (w - kMargin - 280, row2, 142, 26);

    activePresetNameLabel_.setBounds (kMargin + 14, row2, juce::jmin (420, w / 2), 24);
    modifiedBadgeLabel_.setBounds (kMargin + 14 + juce::jmin (420, w / 2) + 8, row2 + 2, 80, 20);
    presetCountLabel_.setBounds (kMargin + 270, row2 + 2, w - kMargin - 290, 20);
    categoryHintLabel_.setBounds (kMargin + 14, row2 + 26, w - 2 * kMargin, 16);

    const int contentTop = kMargin + kBrowserBandHeight + 8;
    const int contentH = getHeight() - contentTop - kMargin;
    const int leftW = juce::roundToInt ((w - 2 * kMargin) * 0.58f);
    const int rightW = w - 2 * kMargin - leftW - 8;

    oscSectionLabel_.setBounds (kMargin + 16, contentTop + 8, 200, 18);
    const auto oscArea = juce::Rectangle<int> (kMargin + 12, contentTop + 28, leftW - 24,
                                               juce::roundToInt (contentH * 0.52f) - 36);
    layoutKnobGrid (oscArea, 0, 8, 4);

    ampEnvLabel_.setBounds (kMargin + 16, contentTop + juce::roundToInt (contentH * 0.54f) + 6, 120, 18);
    const int ampY = contentTop + juce::roundToInt (contentH * 0.54f) + 26;
    layoutKnobGrid ({ kMargin + 12, ampY, leftW - 24, juce::roundToInt (contentH * 0.22f) - 30 }, 12, 4, 4);

    filtEnvLabel_.setBounds (kMargin + 16, contentTop + juce::roundToInt (contentH * 0.78f) + 6, 140, 18);
    const int filtY = contentTop + juce::roundToInt (contentH * 0.78f) + 26;
    layoutKnobGrid ({ kMargin + 12, filtY, leftW - 24, contentH - juce::roundToInt (contentH * 0.78f) - 32 }, 16, 4, 4);

    perfSectionLabel_.setBounds (kMargin + leftW + 20, contentTop + 8, 220, 18);
    voiceModeLabel_.setBounds (kMargin + leftW + 20, contentTop + 30, 120, 20);
    outputSlider_.setBounds (kMargin + leftW + 20, contentTop + 56, rightW - 32, 22);
    monoButton_.setBounds (kMargin + leftW + 20, contentTop + 88, 72, 26);
    legatoButton_.setBounds (kMargin + leftW + 98, contentTop + 88, 72, 26);
    unisonBox_.setBounds (kMargin + leftW + 178, contentTop + 88, 88, 26);

    const int perfKnobY = contentTop + 130;
    layoutKnobGrid ({ kMargin + leftW + 16, perfKnobY, rightW - 24, contentH - 140 }, 8, 4, 2);
}
