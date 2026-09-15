#include "PluginRackPanel.h"
#include "Audio/Internal/InternalPluginTypes.h"

namespace vmpc::view
{
PluginRackPanel::PluginRackPanel(vmpc::audio::PluginHostService& host)
    : pluginHost(host)
{
    pluginHost.addListener(this);

    vibeHeading.setText("Vibe Mixing", juce::dontSendNotification);
    vibeHeading.setFont(juce::Font(16.0f, juce::Font::bold));
    vibeHeading.setColour(juce::Label::textColourId, juce::Colour(0xff38bdf8));
    addAndMakeVisible(vibeHeading);

    vibeTagline.setText("Type the feel you want — the DAW builds an internal plug-in chain and preset.",
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

    vibeCreateButton.addListener(this);
    addAndMakeVisible(vibeCreateButton);

    vibeResultLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff59e0b));
    addAndMakeVisible(vibeResultLabel);

    statusLabel.setText("Or load VMPC internal mix tools / external VST3·LV2·AU per slot. MIDI goes to instrument slots.",
                        juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(statusLabel);

    scanButton.addListener(this);
    addAndMakeVisible(scanButton);

    for (int i = 0; i < vmpc::audio::PluginSlotChain::kNumSlots; ++i)
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
    pluginHost.removeListener(this);
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

    auto vibeRow = area.removeFromTop(30);
    vibeCreateButton.setBounds(vibeRow.removeFromRight(150).reduced(0, 2));
    vibePrompt.setBounds(vibeRow.reduced(0, 2));

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
        createVibePresetFromInput();
}

void PluginRackPanel::createVibePresetFromInput()
{
    const auto brief = vibePrompt.getText();
    const auto result = pluginHost.applyVibeMixFromBrief(brief);

    if (!result.success)
    {
        vibeResultLabel.setText(result.error, juce::dontSendNotification);
        return;
    }

    vibeResultLabel.setText(result.recipe.explanation, juce::dontSendNotification);
    statusLabel.setText("Vibe preset \"" + result.recipe.title + "\" loaded — tweak slots or open plug-in UI.",
                        juce::dontSendNotification);
    refreshSlotLabels();
}

void PluginRackPanel::buttonClicked(juce::Button* button)
{
    if (button == &vibeCreateButton)
    {
        createVibePresetFromInput();
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
            pluginHost.showEditorForSlot(ui.slotIndex);
            return;
        }
        if (button == &ui.clearButton)
        {
            pluginHost.clearSlot(ui.slotIndex);
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
    for (const auto& info : vmpc::audio::internal::allMixPlugins())
    {
        internalMenu.addItem(internalBase + static_cast<int>(info.id),
                             info.displayName + " — " + info.description);
    }
    menu.addSubMenu("VMPC internal mix", internalMenu);

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
                               const auto id = static_cast<vmpc::audio::internal::MixPluginId>(result - 1000);
                               pluginHost.loadInternalMixPlugin(slotIndex, id);
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
                           pluginHost.loadPluginIntoSlot(slotIndex, desc, [this](bool ok, const juce::String& err) {
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
    const auto states = pluginHost.getChain().getSlotStates();
    for (int i = 0; i < vmpc::audio::PluginSlotChain::kNumSlots; ++i)
    {
        const auto& state = states[static_cast<size_t>(i)];
        auto& ui = slots[static_cast<size_t>(i)];
        const bool internal = state.name.startsWith("VMPC ");
        ui.pluginName.setText(state.loaded ? state.name + (state.isInstrument ? " [instrument]"
                                                                             : internal ? " [internal]"
                                                                                        : " [FX]")
                                           : "Empty",
                              juce::dontSendNotification);
        ui.editorButton.setEnabled(state.loaded);
        ui.clearButton.setEnabled(state.loaded);
    }
}
} // namespace vmpc::view
