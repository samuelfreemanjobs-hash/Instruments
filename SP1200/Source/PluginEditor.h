#pragma once

#include "PluginProcessor.h"
#include "UI/ChopModalComponent.h"
#include "UI/KeypadComponent.h"
#include "UI/LcdEditField.h"
#include "UI/LcdPanelComponent.h"
#include "UI/PianoRollComponent.h"
#include "UI/StepStackPanel.h"

#include <array>
#include <memory>

class SP1200AudioProcessorEditor : public juce::AudioProcessorEditor,
                                   private juce::Timer,
                                   private juce::Button::Listener,
                                   private juce::KeyListener
{
public:
    explicit SP1200AudioProcessorEditor (SP1200AudioProcessor&);
    ~SP1200AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress& key, juce::Component*) override;

private:
    enum class ViewMode
    {
        console,
        sequencer,
        song,
        setup,
        filter
    };

    void timerCallback() override;
    void refreshMemoryLabel();
    void refreshSeqInfo();
    void triggerPad (int padIndex);
    void importSample();
    void toggleRecordInput();
    void setView (ViewMode mode);
    void runAutoChop16();
    void openChopModal();
    void closeChopModal();
    void saveProject();
    void loadProject();
    void syncUIFromEngine();
    void applyVinylTuneIfNeeded (int padIndex);
    void refreshLearnStatus();
    void refreshLcd();
    void refreshFilterRoleLabel();
    void selectBank (int bankIndex);
    void updatePadHighlight();
    void syncPianoRollFromControls();
    void handleKeypadDigit (int digit);
    void commitKeypadEntry();
    void cancelKeypadEntry();
    void armCombineWithSecondPad();
    void cycleFaderMode();
    void cycleLcdEditField();
    void nudgeLcdEditField (int direction);
    void lcdYesExec();
    void lcdNoBack();
    void beginLcdEdit (sp1200::LcdEditField field);
    void applyStagedLcdEdit();
    void armClearPatternConfirm();
    [[nodiscard]] float readEditFieldValue (sp1200::LcdEditField field) const;
    void writeEditFieldValue (sp1200::LcdEditField field, float value);
    [[nodiscard]] float editFieldStep (sp1200::LcdEditField field) const;
    int padIndexForComputerKey (const juce::KeyPress& key) const;
    void buttonClicked (juce::Button* button) override;

    SP1200AudioProcessor& processor_;

    ViewMode view_ = ViewMode::console;

    juce::TextButton consoleTab_ { "10 CONSOLE" };
    juce::TextButton seqTab_ { "MOD 20 SEQ" };
    juce::TextButton songTab_ { "24 SONG" };
    juce::TextButton setupTab_ { "10 SETUP" };
    juce::TextButton filterTab_ { "15 FILTER" };

    juce::Label headerLabel_;
    juce::Label rateLabel_;
    juce::Label memoryLabel_;
    std::unique_ptr<LcdPanelComponent> lcdPanel_;
    std::unique_ptr<KeypadComponent> keypad_;
    enum class KeypadEntryMode
    {
        idle,
        pattern,
        bank
    };
    KeypadEntryMode keypadMode_ = KeypadEntryMode::idle;
    juce::String keypadBuffer_;
    sp1200::LcdEditField editField_ = sp1200::LcdEditField::none;
    bool editStaged_ = false;
    float stagedEditValue_ = 0.0f;
    bool confirmPending_ = false;
    enum class PendingConfirmAction
    {
        none,
        clearPattern
    };
    PendingConfirmAction pendingConfirm_ = PendingConfirmAction::none;
    juce::TextButton lcdMinusButton_ { "-1" };
    juce::TextButton lcdPlusButton_ { "+1" };
    juce::TextButton lcdNoButton_ { "NO·BACK" };
    juce::TextButton lcdYesButton_ { "YES·EXEC" };
    juce::Label filterRoleLabel_;
    juce::Label filterTopologyLabel_;
    std::array<juce::TextButton, sp1200::kNumBanks> bankButtons_;
    juce::Label engagedLabel_;

    juce::TextButton importButton_ { "Import WAV" };
    juce::TextButton recordButton_ { "ARM INPUT" };
    juce::TextButton faderModeButton_ { "FADER: VOL" };
    juce::TextButton chopButton_ { "Auto-chop 16 (transient)" };
    juce::TextButton mod11Button_ { "MOD 11 CHOP" };
    juce::TextButton saveProjectButton_ { "Save .sp12p" };
    juce::TextButton loadProjectButton_ { "Load .sp12p" };
    juce::ToggleButton multiPitchButton_ { "MULTI-PITCH (16 cap)" };
    juce::Slider busFilterSlider_;
    juce::Slider busResSlider_;

    juce::Slider patternSlider_;
    juce::Slider barsSlider_;
    juce::Slider bpmSlider_;
    juce::Slider swingSlider_;
    juce::Label seqInfoLabel_;
    juce::TextButton seqPlayButton_ { "PLAY PATTERN" };
    juce::TextButton seqStopButton_ { "STOP" };
    juce::ToggleButton seqRecordSteps_ { "Record steps (pads)" };
    juce::ToggleButton chromaticMapButton_ { "CHROMATIC SSM" };
    juce::ComboBox chromaticTuneBox_;
    juce::TextButton clearPatternButton_ { "Clear pattern" };

    std::unique_ptr<PianoRollComponent> pianoRoll_;
    std::unique_ptr<StepStackPanel> stepStacks_;
    juce::TextButton stackVelButton_ { "VELOCITY" };
    juce::TextButton stackPitchButton_ { "PITCH" };
    juce::TextButton stackPanButton_ { "PAN" };
    juce::TextButton stackFilterButton_ { "FILTER" };
    juce::Slider stackPadSlider_;

    juce::Label songInfoLabel_;
    std::array<juce::ComboBox, 8> songSlotBoxes_;
    juce::TextButton songPlayButton_ { "PLAY SONG" };
    juce::ToggleButton songLoopButton_ { "Loop song chain" };
    juce::Slider midiChannelSlider_;
    juce::ToggleButton midiOmniButton_ { "MIDI omni channel" };
    juce::Label setupInfoLabel_;
    juce::ComboBox clockModeBox_;
    juce::ComboBox learnPadBox_;
    juce::TextButton learnNoteButton_ { "Learn NOTE" };
    juce::TextButton learnCcButton_ { "Learn CC" };
    juce::TextButton resetMidiMapButton_ { "Reset MIDI maps" };
    juce::Label learnStatusLabel_;
    std::array<juce::ComboBox, sp1200::kNumPads> chokeGroupBoxes_;
    juce::ToggleButton vinylImportButton_ { "VINYL 33→45 import" };
    juce::TextButton mod30CombineButton_ { "MOD 30 COMBINE" };
    juce::TextButton mod30MoveBankButton_ { "SEG → BANK" };
    bool combineArm_ = false;

    std::array<juce::TextButton, sp1200::kNumPads> padButtons_;
    std::array<juce::Slider, sp1200::kNumPads> faders_;

    int faderMode_ = 0;
    int lastSegmentForChop_ = -1;
    bool seqRecordMode_ = false;
    std::unique_ptr<ChopModalComponent> chopModal_;
};
