#pragma once

#include <JuceHeader.h>
#include "Audio/PluginHostService.h"

namespace vmpc::view
{
class PluginRackPanel : public juce::Component,
                        public juce::Button::Listener,
                        private vmpc::audio::PluginHostService::Listener
{
public:
    explicit PluginRackPanel(vmpc::audio::PluginHostService& host);
    ~PluginRackPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

private:
    struct SlotUi
    {
        juce::Label title;
        juce::Label pluginName;
        juce::TextButton loadButton { "Load VST" };
        juce::TextButton editorButton { "UI" };
        juce::TextButton clearButton { "Clear" };
        int slotIndex = 0;
    };

    void pluginSlotsChanged() override;
    void pluginScanFinished() override;
    void refreshSlotLabels();
    void showPluginPicker(int slotIndex);

    vmpc::audio::PluginHostService& pluginHost;
    juce::TextButton scanButton { "Scan VST plug-ins" };
    juce::Label statusLabel;
    std::array<SlotUi, vmpc::audio::PluginSlotChain::kNumSlots> slots {};
};
} // namespace vmpc::view
