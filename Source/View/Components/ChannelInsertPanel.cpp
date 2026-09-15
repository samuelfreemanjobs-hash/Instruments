#include "ChannelInsertPanel.h"
#include "Audio/Internal/InternalPluginTypes.h"

namespace vmpc::view
{
ChannelInsertPanel::ChannelInsertPanel(vmpc::audio::PluginHostService& host, int channelIndex)
    : pluginHost(host)
    , channel(channelIndex)
{
    pluginHost.addListener(this);

    heading.setText("Channel " + juce::String(channel + 1) + " inserts", juce::dontSendNotification);
    heading.setFont(juce::FontOptions(15.0f).withStyle("Bold"));
    addAndMakeVisible(heading);

    for (int i = 0; i < vmpc::audio::PluginSlotChain::kChannelInsertSlots; ++i)
    {
        auto& ui = inserts[static_cast<size_t>(i)];
        ui.slot = i;
        ui.label.setText("Ins " + juce::String(i + 1), juce::dontSendNotification);
        ui.load.addListener(this);
        ui.ui.addListener(this);
        ui.clear.addListener(this);
        addAndMakeVisible(ui.label);
        addAndMakeVisible(ui.name);
        addAndMakeVisible(ui.load);
        addAndMakeVisible(ui.ui);
        addAndMakeVisible(ui.clear);
    }

    refresh();
}

ChannelInsertPanel::~ChannelInsertPanel()
{
    pluginHost.removeListener(this);
}

vmpc::audio::PluginSlotLocation ChannelInsertPanel::location(int slotIndex) const noexcept
{
    return { vmpc::audio::PluginSlotLocation::Bus::Channel, channel, slotIndex };
}

void ChannelInsertPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0c0e13));
}

void ChannelInsertPanel::resized()
{
    auto area = getLocalBounds().reduced(10);
    heading.setBounds(area.removeFromTop(24));
    area.removeFromTop(6);

    for (auto& ui : inserts)
    {
        auto row = area.removeFromTop(30);
        ui.label.setBounds(row.removeFromLeft(44));
        ui.name.setBounds(row.removeFromLeft(row.getWidth() - 150));
        ui.load.setBounds(row.removeFromLeft(50).reduced(2));
        ui.ui.setBounds(row.removeFromLeft(40).reduced(2));
        ui.clear.setBounds(row.removeFromLeft(44).reduced(2));
        area.removeFromTop(4);
    }
}

void ChannelInsertPanel::buttonClicked(juce::Button* button)
{
    for (auto& ui : inserts)
    {
        if (button == &ui.load)
        {
            showPicker(ui.slot);
            return;
        }
        if (button == &ui.ui)
        {
            pluginHost.showEditorForSlot(location(ui.slot));
            return;
        }
        if (button == &ui.clear)
        {
            pluginHost.clearSlot(location(ui.slot));
            return;
        }
    }
}

void ChannelInsertPanel::showPicker(int slotIndex)
{
    juce::PopupMenu menu;
    const auto types = pluginHost.getKnownPlugins().getTypes();

    int internalBase = 1000;
    juce::PopupMenu internalMenu;
    for (const auto& info : vmpc::audio::internal::allMixPlugins())
        internalMenu.addItem(internalBase + static_cast<int>(info.id), info.displayName);
    menu.addSubMenu("VMPC internal", internalMenu);

    const int externalBase = 2000;
    int id = externalBase;
    for (const auto& type : types)
        menu.addItem(id++, type.name);

    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this, slotIndex, types, externalBase](int result) {
                           if (result <= 0)
                               return;

                           const auto loc = location(slotIndex);

                           if (result >= 1000 && result < externalBase)
                           {
                               pluginHost.loadInternalMixPlugin(
                                   loc, static_cast<vmpc::audio::internal::MixPluginId>(result - 1000));
                               refresh();
                               return;
                           }

                           const int index = result - externalBase;
                           if (index < 0 || index >= types.size())
                               return;

                           pluginHost.loadPluginIntoSlot(loc, types.getReference(index), [this](bool, const juce::String&) {
                               refresh();
                           });
                       });
}

void ChannelInsertPanel::pluginSlotsChanged()
{
    refresh();
}

void ChannelInsertPanel::refresh()
{
    const auto states = pluginHost.getMixConsole().getChannelInserts(channel).getSlotStates();
    for (int i = 0; i < static_cast<int>(states.size()); ++i)
    {
        auto& ui = inserts[static_cast<size_t>(i)];
        const auto& state = states[static_cast<size_t>(i)];
        ui.name.setText(state.loaded ? state.name : "Empty", juce::dontSendNotification);
        ui.ui.setEnabled(state.loaded);
        ui.clear.setEnabled(state.loaded);
    }
}
} // namespace vmpc::view
