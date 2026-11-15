#pragma once

#include "LabeledFader.h"
#include "OscMonitorComponent.h"
#include "Parameters/ParameterIds.h"
#include "PresetLcdStrip.h"
#include "VoiceLedCluster.h"
#include "PeakMeterComponent.h"
#include "WheelV2Strip.h"
#include "../JunovaLookAndFeel.h"
#include "../UiLayout.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

namespace junovax::ui::celestial
{
class ModuleShell final : public juce::Component
{
public:
    ModuleShell (juce::String title, juce::Colour accent, juce::Colour fill);

    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::Rectangle<int> getContentArea() const;

private:
    juce::String title_;
    juce::Colour accent_;
    juce::Colour fill_;
    juce::Label titleLabel_ { {}, {} };
};

class CelestialMainPanel final : public juce::Component,
                                 private juce::Timer
{
public:
    explicit CelestialMainPanel (JunovaXAudioProcessor& processor);
    ~CelestialMainPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void refreshPresetLabel();
    void refreshHostBar();
    void syncPerformanceMeters();

    void setVaultOpener (std::function<void()> fn) { onOpenVault_ = std::move (fn); }

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    LabeledFader& addFader (juce::Component& parent, const char* paramId, const juce::String& label,
                            junovax::ui::JunovaLookAndFeel::Accent accent);
    void layoutFaderRow (juce::Rectangle<int> area, const std::vector<LabeledFader*>& row, int labelStrip = 0);
    void addChorusModeButtons (juce::Component& parent);
    void wireChipRow (const std::vector<juce::TextButton*>& buttons, const char* paramId, int numChoices);

    JunovaXAudioProcessor& processor_;
    junovax::ui::JunovaLookAndFeel laf_;

    juce::Label brand_ { {}, "JUNOVA-X" };
    juce::Label tagline_ { {}, "CELESTIAL POLYPHONIC SYNTHESIZER" };
    juce::Label hostBar_ { {}, {} };
    PresetLcdStrip presetLcd_;
    juce::TextButton saveBtn_ { "SAVE" };
    juce::TextButton initBtn_ { "INIT" };
    juce::TextButton vaultBtn_ { "VAULT" };
    juce::TextButton abBtn_ { "A/B" };
    juce::Label oversampleBadge_ { {}, "OVERSAMPLE 2x (v2)" };

    std::function<void()> onOpenVault_;

    OscMonitorComponent oscMonitor_;
    VoiceLedCluster voiceLeds_;

    juce::TextButton lfoTri_ { "TRI" };
    juce::TextButton lfoSqr_ { "SQR" };
    juce::TextButton lfoSh_ { "S&H" };
    juce::ToggleButton lfoSync_ { "BPM SYNC" };

    juce::TextButton dcoPulse_ { "PULSE" };
    juce::TextButton dcoSaw_ { "SAW" };
    juce::TextButton dcoSub_ { "SUB" };
    juce::TextButton dcoNoise_ { "NOISE" };

    void timerCallback() override;

    ModuleShell lfoModule_ { "LFO / PORTA", Theme::electricCyan(), Theme::panelBlue() };
    ModuleShell dcoModule_ { "ORBIT DCO MODULE", Theme::electricCyan(), Theme::panelBlue() };
    ModuleShell hpfModule_ { "HPF", Theme::textMuted(), Theme::panelGrey() };
    juce::Label hpfCaption_ { {}, "BASS CUT" };
    ModuleShell vcfModule_ { "STELLAR VCF", Theme::stellarCrimson(), Theme::panelRed() };
    ModuleShell envModule_ { "ENVELOPE // VCA", Theme::stellarCrimson(), Theme::panelRed() };
    ModuleShell chorusModule_ { "MN3009 BBD CHORUS", Theme::electricCyan(), Theme::panelGrey() };
    ModuleShell driftModule_ { "DRIFT & DETUNE", Theme::electricCyan(), Theme::panelBlue() };
    ModuleShell arpModule_ { "ARPEGGIATOR", Theme::electricCyan(), Theme::panelGrey() };
    ModuleShell masterModule_ { "MASTER", Theme::stellarCrimson(), Theme::panelRed() };

    juce::TextButton voicePoly1_ { "POLY 1" };
    juce::TextButton voicePoly2_ { "POLY 2" };
    juce::TextButton voiceUnison_ { "UNISON" };
    juce::TextButton voiceMono_ { "MONO" };

    juce::TextButton chorusOff_ { "OFF" };
    juce::TextButton chorusI_ { "CH I" };
    juce::TextButton chorusII_ { "CH II" };
    juce::TextButton chorusBoth_ { "I+II" };

    juce::TextButton hpfOff_ { "OFF" };
    juce::TextButton hpf1_ { "80" };
    juce::TextButton hpf2_ { "120" };
    juce::TextButton hpf3_ { "220" };

    juce::ToggleButton arpOn_ { "ARP" };
    juce::ToggleButton arpLatch_ { "LATCH" };

    PeakMeterComponent peakMeter_;
    WheelV2Strip pitchWheel_ { WheelV2Strip::Kind::pitch };
    WheelV2Strip modWheel_ { WheelV2Strip::Kind::mod };
    juce::TextButton transposeDown_ { "-12" };
    juce::TextButton transposeZero_ { "0" };
    juce::TextButton transposeUp_ { "+12" };
    juce::Label transposeLabel_ { {}, {} };
    juce::Label ampEnvTag_ { {}, {} };
    juce::Label filtEnvTag_ { {}, {} };
    std::unique_ptr<juce::MidiKeyboardComponent> keyboard_;

    std::vector<std::unique_ptr<LabeledFader>> faders_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
    std::unique_ptr<ButtonAttachment> arpOnAttachment_;
    std::unique_ptr<ButtonAttachment> arpLatchAttachment_;
    std::unique_ptr<ButtonAttachment> lfoSyncAttachment_;

    // Fader row indices for layout
    std::vector<LabeledFader*> lfoFaders_;
    std::vector<LabeledFader*> dcoFaders_;
    std::vector<LabeledFader*> vcfFaders_;
    std::vector<LabeledFader*> ampFaders_;
    std::vector<LabeledFader*> filtFaders_;
    std::vector<LabeledFader*> driftFaders_;
    std::vector<LabeledFader*> arpFaders_;
    LabeledFader* masterFader_ = nullptr;

    float peakLevel_ = 0.0f;
    int transposeDisplay_ = 0;

    void refreshTransposeUi();
    static int countWhiteKeysInRange (int lowNote, int highNote) noexcept;
};
} // namespace junovax::ui::celestial