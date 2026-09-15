#include "PluginRackPanel.h"
#include "View/LookAndFeel/VMpcLookAndFeel.h"

namespace vmpc::view
{
PluginRackPanel::PluginRackPanel(vmpc::audio::PluginHostService& host)
    : pluginHost(host)
{
    pluginHost.addListener(this);

    statusLabel.setText("Load VST3 (and LV2 on Linux) instruments/effects into slots. MIDI from the active sequencer is sent to instrument slots.",
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

void PluginRackPanel::buttonClicked(juce::Button* button)
{
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

    int id = 1;
    for (const auto& type : types)
    {
        menu.addItem(id++, type.name + " (" + type.pluginFormatName + ")");
    }

    if (types.isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                               "No plug-ins found",
                                               "Click Scan VST plug-ins, then confirm your .vst3 folder is listed in docs/VST_HOSTING.md.");
        return;
    }

    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this, slotIndex, types](int result) {
                           if (result <= 0)
                               return;

                           const auto& desc = types[static_cast<size_t>(result - 1)];
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
        ui.pluginName.setText(state.loaded ? state.name + (state.isInstrument ? " [instrument]" : " [FX]")
                                           : "Empty",
                              juce::dontSendNotification);
        ui.editorButton.setEnabled(state.loaded);
        ui.clearButton.setEnabled(state.loaded);
    }
}
} // namespace vmpc::view
