#pragma once

#include <JuceHeader.h>
#include "Audio/PluginHostService.h"

namespace resonance::view
{
/** Two insert slots for one mixer channel. */
class ChannelInsertPanel : public juce::Component,
                           public juce::Button::Listener,
                           private resonance::audio::PluginHostService::Listener
{
public:
    ChannelInsertPanel(resonance::audio::PluginHostService& host, int channelIndex);
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
    resonance::audio::PluginSlotLocation location(int slotIndex) const noexcept;

    resonance::audio::PluginHostService& pluginHost;
    int channel = 0;
    juce::Label heading;
    std::array<InsertUi, resonance::audio::PluginSlotChain::kChannelInsertSlots> inserts {};
};
} // namespace resonance::view
