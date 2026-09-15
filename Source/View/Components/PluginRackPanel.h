#pragma once

#include <JuceHeader.h>
#include "Audio/PluginHostService.h"

namespace resonance::view
{
class PluginRackPanel : public juce::Component,
                        public juce::Button::Listener,
                        public juce::TextEditor::Listener,
                        public juce::ComboBox::Listener,
                        private resonance::audio::PluginHostService::Listener
{
public:
    explicit PluginRackPanel(resonance::audio::PluginHostService& host);
    ~PluginRackPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    void comboBoxChanged(juce::ComboBox* box) override;

private:
    void beginVibePreviewFromInput();
    resonance::audio::VibeMixTarget currentVibeTarget() const;
    void applyChipBrief(const juce::String& chip);
    void exportRecipeJsonToClipboard(const resonance::audio::VibeMixRecipe& recipe);

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
    void rebuildChipButtons();

    resonance::audio::PluginHostService& pluginHost;

    juce::Label vibeHeading;
    juce::Label vibeTagline;
    juce::TextEditor vibePrompt;
    juce::ComboBox vibeTargetCombo;
    juce::ComboBox vibeChannelCombo;
    juce::TextButton vibeCreateButton { "Preview vibe" };
    juce::TextButton vibeUndoButton { "Undo vibe" };
    juce::TextButton vibeAbButton { "A/B compare" };
    juce::TextButton vibeExportButton { "Copy JSON" };
    juce::TextButton vibeSaveButton { "Save vibe" };
    juce::Label vibeResultLabel;

    juce::Component chipRow;
    std::vector<std::unique_ptr<juce::TextButton>> chipButtons;

    juce::TextButton scanButton { "Scan VST plug-ins" };
    juce::Label statusLabel;
    std::array<SlotUi, resonance::audio::PluginSlotChain::kMasterSlots> slots {};

    static resonance::audio::PluginSlotLocation masterSlot(int index) noexcept;
};
} // namespace resonance::view
