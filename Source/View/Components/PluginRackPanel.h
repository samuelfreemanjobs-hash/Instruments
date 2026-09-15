#pragma once

#include <JuceHeader.h>
#include "Audio/PluginHostService.h"

namespace resonance::view
{
class PluginRackPanel : public juce::Component,
                        public juce::Button::Listener,
                        public juce::TextEditor::Listener,
                        private resonance::audio::PluginHostService::Listener
{
public:
    explicit PluginRackPanel(resonance::audio::PluginHostService& host);
    ~PluginRackPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;

private:
    void createVibePresetFromInput();
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

    resonance::audio::PluginHostService& pluginHost;

    juce::Label vibeHeading;
    juce::Label vibeTagline;
    juce::TextEditor vibePrompt;
    juce::TextButton vibeCreateButton { "Create vibe preset" };
    juce::Label vibeResultLabel;

    juce::TextButton scanButton { "Scan VST plug-ins" };
    juce::Label statusLabel;
    std::array<SlotUi, resonance::audio::PluginSlotChain::kMasterSlots> slots {};

    static resonance::audio::PluginSlotLocation masterSlot(int index) noexcept;
};
} // namespace resonance::view
