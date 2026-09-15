#pragma once

#include <JuceHeader.h>
#include "Audio/PluginHostService.h"

namespace vmpc::view
{
/** Two insert slots for one mixer channel. */
class ChannelInsertPanel : public juce::Component,
                           public juce::Button::Listener,
                           private vmpc::audio::PluginHostService::Listener
{
public:
    ChannelInsertPanel(vmpc::audio::PluginHostService& host, int channelIndex);
    ~ChannelInsertPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

private:
    struct InsertUi
    {
        juce::Label label;
        juce::Label name;
        juce::TextButton load { "Load" };
        juce::TextButton ui { "UI" };
        juce::TextButton clear { "Clr" };
        int slot = 0;
    };

    void pluginSlotsChanged() override;
    void pluginScanFinished() override {}
    void refresh();
    void showPicker(int slotIndex);
    vmpc::audio::PluginSlotLocation location(int slotIndex) const noexcept;

    vmpc::audio::PluginHostService& pluginHost;
    int channel = 0;
    juce::Label heading;
    std::array<InsertUi, vmpc::audio::PluginSlotChain::kChannelInsertSlots> inserts {};
};
} // namespace vmpc::view
