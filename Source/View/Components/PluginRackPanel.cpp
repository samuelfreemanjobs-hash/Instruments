#include "PluginRackPanel.h"
#include "VibeMixPreviewDialog.h"
#include "Audio/VibeArchetypeLibrary.h"
#include "Audio/VibeRecipeJson.h"
#include "Audio/Internal/InternalPluginTypes.h"

namespace resonance::view
{
resonance::audio::PluginSlotLocation PluginRackPanel::masterSlot(int index) noexcept
{
    return { resonance::audio::PluginSlotLocation::Bus::Master, 0, index };
}

PluginRackPanel::PluginRackPanel(resonance::audio::PluginHostService& host)
    : pluginHost(host)
{
    pluginHost.addListener(this);

    vibeHeading.setText("Vibe Mixing", juce::dontSendNotification);
    vibeHeading.setFont(juce::Font(16.0f, juce::Font::bold));
    vibeHeading.setColour(juce::Label::textColourId, juce::Colour(0xff38bdf8));
    addAndMakeVisible(vibeHeading);

    vibeTagline.setText("Describe the feel, pick a target, preview the chain, then apply.",
                        juce::dontSendNotification);
    vibeTagline.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(vibeTagline);

    vibePrompt.setMultiLine(false);
    vibePrompt.setReturnKeyStartsNewLine(false);
    vibePrompt.setTextToShowWhenEmpty("e.g. warm punchy club master with airy top", juce::Colours::grey);
    vibePrompt.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff0c0e13));
    vibePrompt.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff38bdf8).withAlpha(0.4f));
    vibePrompt.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    vibePrompt.addListener(this);
    addAndMakeVisible(vibePrompt);

    vibeTargetCombo.addItem("Master bus", 1);
    vibeTargetCombo.addItem("Full mix", 2);
    vibeTargetCombo.addItem("Drums (ch 1)", 3);
    vibeTargetCombo.addItem("Vocals (ch 2)", 4);
    vibeTargetCombo.addItem("Synths (ch 3)", 5);
    vibeTargetCombo.addItem("Channel insert", 6);
    vibeTargetCombo.setSelectedId(1, juce::dontSendNotification);
    vibeTargetCombo.addListener(this);
    addAndMakeVisible(vibeTargetCombo);

    for (int ch = 0; ch < resonance::audio::MixConsole::kNumChannels; ++ch)
        vibeChannelCombo.addItem("Channel " + juce::String(ch + 1), ch + 1);
    vibeChannelCombo.setSelectedId(1, juce::dontSendNotification);
    vibeChannelCombo.setVisible(false);
    vibeChannelCombo.addListener(this);
    addAndMakeVisible(vibeChannelCombo);

    vibeCreateButton.addListener(this);
    vibeUndoButton.addListener(this);
    vibeAbButton.addListener(this);
    vibeExportButton.addListener(this);
    vibeSaveButton.addListener(this);
    addAndMakeVisible(vibeCreateButton);
    addAndMakeVisible(vibeUndoButton);
    addAndMakeVisible(vibeAbButton);
    addAndMakeVisible(vibeExportButton);
    addAndMakeVisible(vibeSaveButton);

    vibeResultLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff59e0b));
    addAndMakeVisible(vibeResultLabel);

    addAndMakeVisible(chipRow);
    rebuildChipButtons();

    statusLabel.setText("Or load Resonance internal mix tools / external VST3·LV2·AU per slot.",
                        juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(statusLabel);

    scanButton.addListener(this);
    addAndMakeVisible(scanButton);

    for (int i = 0; i < resonance::audio::PluginSlotChain::kMasterSlots; ++i)
    {
        auto& ui = slots[static_cast<size_t>(i)];
        ui.slotIndex = i;
        ui.title.setText("Slot " + juce::String(i + 1), juce::dontSendNotification);
        ui.title.setJustificationType(juce::Justification::centredLeft);
        ui.pluginName.setText("Empty", juce::dontSendNotification);
        ui.loadButton.addListener(this);
        ui.editorButton.addListener(this);
        ui.clearButton.addListener(this);
        addAndMakeVisible(ui.title);
        addAndMakeVisible(ui.pluginName);
        addAndMakeVisible(ui.loadButton);
        addAndMakeVisible(ui.editorButton);
        addAndMakeVisible(ui.clearButton);
    }

    refreshSlotLabels();
}

PluginRackPanel::~PluginRackPanel()
{
    vibePrompt.removeListener(this);
    vibeTargetCombo.removeListener(this);
    vibeChannelCombo.removeListener(this);
    pluginHost.removeListener(this);
}

resonance::audio::VibeMixTarget PluginRackPanel::currentVibeTarget() const
{
    resonance::audio::VibeMixTarget target;
    switch (vibeTargetCombo.getSelectedId())
    {
        case 2:
            target.kind = resonance::audio::VibeMixTargetKind::FullMix;
            break;
        case 3:
            target.kind = resonance::audio::VibeMixTargetKind::Drums;
            break;
        case 4:
            target.kind = resonance::audio::VibeMixTargetKind::Vocals;
            break;
        case 5:
            target.kind = resonance::audio::VibeMixTargetKind::Synths;
            break;
        case 6:
            target.kind = resonance::audio::VibeMixTargetKind::Channel;
            target.channelIndex = vibeChannelCombo.getSelectedId() - 1;
            break;
        default:
            target.kind = resonance::audio::VibeMixTargetKind::Master;
            break;
    }
    return target;
}

void PluginRackPanel::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &vibeTargetCombo)
        vibeChannelCombo.setVisible(vibeTargetCombo.getSelectedId() == 6);
    resized();
}

void PluginRackPanel::rebuildChipButtons()
{
    chipButtons.clear();
    chipRow.removeAllChildren();

    const auto chips = resonance::audio::VibeArchetypeLibrary::chipSuggestions();
    int x = 0;
    for (const auto& chip : chips)
    {
        auto btn = std::make_unique<juce::TextButton>(chip);
        btn->setBounds(x, 0, 110, 24);
        btn->onClick = [this, chip]() { applyChipBrief(chip); };
        chipRow.addAndMakeVisible(btn.get());
        x += 114;
        chipButtons.push_back(std::move(btn));
    }
    chipRow.setSize(juce::jmax(400, x), 26);
}

void PluginRackPanel::applyChipBrief(const juce::String& chip)
{
    vibePrompt.setText(chip, false);
    beginVibePreviewFromInput();
}

void PluginRackPanel::exportRecipeJsonToClipboard(const resonance::audio::VibeMixRecipe& recipe)
{
    const auto json = resonance::audio::VibeRecipeJson::toJson(recipe);
    juce::SystemClipboard::copyTextToClipboard(json);
    statusLabel.setText("Vibe recipe JSON copied to clipboard.", juce::dontSendNotification);
}

void PluginRackPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111318));
    g.setColour(juce::Colour(0xff38bdf8).withAlpha(0.35f));
    g.drawRect(getLocalBounds(), 1);
}

void PluginRackPanel::resized()
{
    auto area = getLocalBounds().reduced(8);

    vibeHeading.setBounds(area.removeFromTop(22));
    vibeTagline.setBounds(area.removeFromTop(18));
    area.removeFromTop(4);

    auto targetRow = area.removeFromTop(28);
    vibeChannelCombo.setBounds(targetRow.removeFromRight(120).reduced(2));
    vibeTargetCombo.setBounds(targetRow.removeFromRight(160).reduced(2));

    auto vibeRow = area.removeFromTop(30);
    vibeCreateButton.setBounds(vibeRow.removeFromRight(110).reduced(0, 2));
    vibeUndoButton.setBounds(vibeRow.removeFromRight(80).reduced(0, 2));
    vibeAbButton.setBounds(vibeRow.removeFromRight(90).reduced(0, 2));
    vibeExportButton.setBounds(vibeRow.removeFromRight(80).reduced(0, 2));
    vibeSaveButton.setBounds(vibeRow.removeFromRight(80).reduced(0, 2));
    vibePrompt.setBounds(vibeRow.reduced(0, 2));

    chipRow.setBounds(area.removeFromTop(28));
    vibeResultLabel.setBounds(area.removeFromTop(20));
    area.removeFromTop(6);

    auto header = area.removeFromTop(28);
    scanButton.setBounds(header.removeFromRight(160));
    statusLabel.setBounds(header);

    area.removeFromTop(6);
    const int rowH = 32;
    for (auto& ui : slots)
    {
        auto row = area.removeFromTop(rowH);
        ui.title.setBounds(row.removeFromLeft(52));
        ui.pluginName.setBounds(row.removeFromLeft(row.getWidth() - 240));
        ui.loadButton.setBounds(row.removeFromLeft(80).reduced(2));
        ui.editorButton.setBounds(row.removeFromLeft(48).reduced(2));
        ui.clearButton.setBounds(row.removeFromLeft(56).reduced(2));
        area.removeFromTop(2);
    }
}

void PluginRackPanel::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    if (&editor == &vibePrompt)
        beginVibePreviewFromInput();
}

void PluginRackPanel::beginVibePreviewFromInput()
{
    const auto brief = vibePrompt.getText();
    const auto target = currentVibeTarget();
    const auto result = pluginHost.interpretVibeMix(brief, target);

    if (!result.success)
    {
        vibeResultLabel.setText(result.error, juce::dontSendNotification);
        return;
    }

    VibeMixPreviewDialog::show(result, [this, brief](const resonance::audio::VibeMixRecipe& recipe) {
        pluginHost.appendVibeBrief(brief);
        if (pluginHost.applyVibeRecipe(recipe))
        {
            vibeResultLabel.setText(recipe.explanation, juce::dontSendNotification);
            statusLabel.setText("Applied \"" + recipe.title + "\" to " + recipe.target.displayName() + ".",
                                juce::dontSendNotification);
            refreshSlotLabels();
        }
    });
}

void PluginRackPanel::buttonClicked(juce::Button* button)
{
    if (button == &vibeCreateButton)
    {
        beginVibePreviewFromInput();
        return;
    }

    if (button == &vibeUndoButton)
    {
        if (pluginHost.undoLastVibeApply())
        {
            statusLabel.setText("Reverted last vibe apply.", juce::dontSendNotification);
            refreshSlotLabels();
        }
        return;
    }

    if (button == &vibeAbButton)
    {
        if (pluginHost.swapVibeAB())
        {
            statusLabel.setText("Toggled A/B vibe compare.", juce::dontSendNotification);
            refreshSlotLabels();
        }
        return;
    }

    if (button == &vibeExportButton)
    {
        const auto result = pluginHost.interpretVibeMix(vibePrompt.getText(), currentVibeTarget());
        if (result.success)
            exportRecipeJsonToClipboard(result.recipe);
        else
            vibeResultLabel.setText(result.error, juce::dontSendNotification);
        return;
    }

    if (button == &vibeSaveButton)
    {
        const auto result = pluginHost.interpretVibeMix(vibePrompt.getText(), currentVibeTarget());
        if (!result.success)
        {
            vibeResultLabel.setText(result.error, juce::dontSendNotification);
            return;
        }

        pluginHost.saveNamedVibe(result.recipe.title, result.recipe);
        statusLabel.setText("Saved vibe \"" + result.recipe.title + "\" in project.", juce::dontSendNotification);
        return;
    }

    if (button == &scanButton)
    {
        statusLabel.setText("Scanning plug-in folders...", juce::dontSendNotification);
        pluginHost.startBackgroundScan();
        return;
    }

    for (auto& ui : slots)
    {
        if (button == &ui.loadButton)
        {
            showPluginPicker(ui.slotIndex);
            return;
        }
        if (button == &ui.editorButton)
        {
            pluginHost.showEditorForSlot(masterSlot(ui.slotIndex));
            return;
        }
        if (button == &ui.clearButton)
        {
            pluginHost.clearSlot(masterSlot(ui.slotIndex));
            return;
        }
    }
}

void PluginRackPanel::showPluginPicker(int slotIndex)
{
    juce::PopupMenu menu;
    const auto types = pluginHost.getKnownPlugins().getTypes();

    juce::PopupMenu internalMenu;
    int internalBase = 1000;
    for (const auto& info : resonance::audio::internal::allMixPlugins())
    {
        internalMenu.addItem(internalBase + static_cast<int>(info.id),
                             info.displayName + " — " + info.description);
    }
    menu.addSubMenu("Resonance internal mix", internalMenu);

    const int externalBase = 2000;
    int externalId = externalBase;
    for (const auto& type : types)
        menu.addItem(externalId++, type.name + " (" + type.pluginFormatName + ")");

    if (types.isEmpty())
        menu.addItem(-1, "(No external plug-ins — run Scan)", false);

    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this, slotIndex, types, externalBase](int result) {
                           if (result <= 0)
                               return;

                           if (result >= 1000 && result < externalBase)
                           {
                               const auto id = static_cast<resonance::audio::internal::MixPluginId>(result - 1000);
                               pluginHost.loadInternalMixPlugin(masterSlot(slotIndex), id);
                               statusLabel.setText("Internal mix plug-in loaded.", juce::dontSendNotification);
                               refreshSlotLabels();
                               return;
                           }

                           if (result < externalBase)
                               return;

                           const int index = result - externalBase;
                           if (index < 0 || index >= types.size())
                               return;

                           const auto& desc = types.getReference(index);
                           statusLabel.setText("Loading " + desc.name + "...", juce::dontSendNotification);
                           pluginHost.loadPluginIntoSlot(masterSlot(slotIndex), desc, [this](bool ok, const juce::String& err) {
                               statusLabel.setText(ok ? "Plug-in loaded." : ("Load failed: " + err),
                                                   juce::dontSendNotification);
                               refreshSlotLabels();
                           });
                       });
}

void PluginRackPanel::pluginSlotsChanged()
{
    refreshSlotLabels();
}

void PluginRackPanel::pluginScanFinished()
{
    const int count = pluginHost.getKnownPlugins().getNumTypes();
    statusLabel.setText(juce::String(count) + " plug-in(s) in library. Pick a slot and Load VST.",
                        juce::dontSendNotification);
    refreshSlotLabels();
}

void PluginRackPanel::refreshSlotLabels()
{
    vibeUndoButton.setEnabled(pluginHost.hasVibeUndo());
    vibeAbButton.setEnabled(pluginHost.hasVibeABCompare());

    const auto states = pluginHost.getMasterChain().getSlotStates();
    for (int i = 0; i < resonance::audio::PluginSlotChain::kMasterSlots; ++i)
    {
        const auto& state = states[static_cast<size_t>(i)];
        auto& ui = slots[static_cast<size_t>(i)];
        const bool internal = state.name.startsWith("Resonance ");
        ui.pluginName.setText(state.loaded ? state.name + (state.isInstrument ? " [instrument]"
                                                                             : internal ? " [internal]"
                                                                                        : " [FX]")
                                           : "Empty",
                              juce::dontSendNotification);
        ui.editorButton.setEnabled(state.loaded);
        ui.clearButton.setEnabled(state.loaded);
    }
}
} // namespace resonance::view
