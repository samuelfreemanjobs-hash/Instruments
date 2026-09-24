#include "PluginEditor.h"

#include "ParameterIds.h"
#include "Presets/PresetManager.h"

namespace
{
namespace PID = prophetrev2::ParameterIDs;
constexpr int kWidth = 640;
constexpr int kHeight = 420;
} // namespace

ProphetRev2TrapAudioProcessorEditor::ProphetRev2TrapAudioProcessorEditor (ProphetRev2TrapAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    categoryLabel_.setJustificationType (juce::Justification::centredRight);
    presetLabel_.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (categoryLabel_);
    addAndMakeVisible (presetLabel_);
    addAndMakeVisible (categoryBox_);
    addAndMakeVisible (presetBox_);
    addAndMakeVisible (ampEnvLabel_);
    addAndMakeVisible (filtEnvLabel_);

    for (const auto& cat : prophetrev2::presets::PresetManager::getCategoryOrder())
        categoryBox_.addItem (cat, categoryBox_.getNumItems() + 1);

    categoryBox_.onChange = [this] {
        const juce::String cat = categoryBox_.getText();
        refreshPresetListForCategory (cat, 0);
        const int global = prophetrev2::presets::PresetManager::getGlobalIndexForCategoryPreset (cat, 0);
        processor_.applyFactoryPreset (global);
    };

    presetBox_.onChange = [this] {
        const juce::String cat = categoryBox_.getText();
        const int local = presetBox_.getSelectedItemIndex();
        if (local < 0)
            return;
        const int global = prophetrev2::presets::PresetManager::getGlobalIndexForCategoryPreset (cat, local);
        processor_.applyFactoryPreset (global);
    };

    outputSlider_.setSliderStyle (juce::Slider::LinearHorizontal);
    outputSlider_.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 18);
    addAndMakeVisible (outputSlider_);
    outputAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::outputGain, outputSlider_);

    const char* ampIds[] = { PID::ampAttack, PID::ampDecay, PID::ampSustain, PID::ampRelease };
    const char* filtIds[] = { PID::filtAttack, PID::filtDecay, PID::filtSustain, PID::filtRelease };
    const juce::String adsrLabels[] = { "A", "D", "S", "R" };

    for (int i = 0; i < 4; ++i)
    {
        addEnvelopeKnob (ampIds[i], "Amp " + adsrLabels[i], {});
        addEnvelopeKnob (filtIds[i], "Filt " + adsrLabels[i], {});
    }

    syncUiToCurrentProgram();
    setSize (kWidth, kHeight);
}

ProphetRev2TrapAudioProcessorEditor::~ProphetRev2TrapAudioProcessorEditor() = default;

void ProphetRev2TrapAudioProcessorEditor::addEnvelopeKnob (const char* paramId, juce::String label,
                                                           juce::Rectangle<int>)
{
    auto slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    slider->setName (label);
    addAndMakeVisible (*slider);
    envAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, *slider));
    envSliders_.push_back (std::move (slider));
}

void ProphetRev2TrapAudioProcessorEditor::refreshPresetListForCategory (const juce::String& category,
                                                                        int selectLocalIndex)
{
    presetBox_.clear();
    const auto names = prophetrev2::presets::PresetManager::getPresetNamesForCategory (category);
    for (int i = 0; i < names.size(); ++i)
        presetBox_.addItem (names[i], i + 1);
    presetBox_.setSelectedItemIndex (juce::jlimit (0, names.size() - 1, selectLocalIndex),
                                     juce::dontSendNotification);
}

void ProphetRev2TrapAudioProcessorEditor::syncUiToCurrentProgram()
{
    juce::String cat;
    int local = 0;
    prophetrev2::presets::PresetManager::getCategoryAndLocalIndex (processor_.getCurrentProgram(), cat, local);

    for (int i = 0; i < categoryBox_.getNumItems(); ++i)
    {
        if (categoryBox_.getItemText (i) == cat)
        {
            categoryBox_.setSelectedItemIndex (i, juce::dontSendNotification);
            break;
        }
    }
    refreshPresetListForCategory (cat, local);
}

void ProphetRev2TrapAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff12121a));
    g.setColour (juce::Colour (0xff2a2a38));
    g.fillRoundedRectangle (8.0f, 8.0f, static_cast<float> (getWidth()) - 16.0f, 72.0f, 8.0f);

    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawText ("Rev2 Trap", 20, 14, 200, 28, juce::Justification::left);
    g.setFont (14.0f);
    g.setColour (juce::Colour (0xffb8b8d0));
    g.drawText ("Prophet-style poly · synthetic trap factory presets", 20, 40, 400, 20, juce::Justification::left);
}

void ProphetRev2TrapAudioProcessorEditor::resized()
{
    auto top = getLocalBounds().reduced (12).removeFromTop (56);
    categoryLabel_.setBounds (top.removeFromRight (280).removeFromLeft (70));
    categoryBox_.setBounds (top.removeFromRight (130));
    top = getLocalBounds().reduced (12).removeFromTop (56).withTrimmedLeft (320);
    presetLabel_.setBounds (top.removeFromRight (280).removeFromLeft (52));
    presetBox_.setBounds (getWidth() - 12 - 200, 44, 200, 24);

    outputSlider_.setBounds (20, 88, getWidth() - 40, 24);

    ampEnvLabel_.setBounds (20, 118, 120, 20);
    filtEnvLabel_.setBounds (20, 248, 140, 20);

    auto ampRow = getLocalBounds().reduced (20).withTop (140).withHeight (100);
    auto filtRow = getLocalBounds().reduced (20).withTop (270).withHeight (100);
    const int kw = ampRow.getWidth() / 4;

    for (int i = 0; i < 4; ++i)
    {
        if (i < static_cast<int> (envSliders_.size()))
            envSliders_[static_cast<std::size_t> (i)]->setBounds (ampRow.removeFromLeft (kw).reduced (6));
    }
    for (int i = 0; i < 4; ++i)
    {
        const std::size_t idx = static_cast<std::size_t> (4 + i);
        if (idx < envSliders_.size())
            envSliders_[idx]->setBounds (filtRow.removeFromLeft (kw).reduced (6));
    }
}
