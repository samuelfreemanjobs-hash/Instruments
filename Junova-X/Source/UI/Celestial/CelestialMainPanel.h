#pragma once

#include "OscMonitorComponent.h"
#include "Parameters/ParameterIds.h"
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

class CelestialMainPanel final : public juce::Component
{
public:
    explicit CelestialMainPanel (JunovaXAudioProcessor& processor);
    ~CelestialMainPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    juce::Slider& addVerticalFader (juce::Component& parent, const char* paramId, juce::String name);
    void addChorusModeButtons (juce::Component& parent);

    JunovaXAudioProcessor& processor_;
    juce::LookAndFeel_V4 laf_;

    juce::Label brand_ { {}, "JUNOVA-X" };
    juce::Label tagline_ { {}, "Celestial Polyphonic Synthesizer" };
    juce::Label hostBar_ { {}, "HOST: STANDALONE | BPM: 120.00 | 4/4 | CPU: -- | 48 kHz" };
    juce::Label presetLabel_ { {}, "J-106 CELESTIAL PAD" };

    OscMonitorComponent oscMonitor_;

    ModuleShell lfoModule_ { "LFO / PORTA", Theme::glowBlue(), Theme::panelBlue() };
    ModuleShell dcoModule_ { "ORBIT DCO MODULE", Theme::glowBlue(), Theme::panelBlue() };
    ModuleShell hpfModule_ { "HPF", Theme::textMuted(), Theme::panelGrey() };
    ModuleShell vcfModule_ { "STELLAR VCF", Theme::glowRed(), Theme::panelRed() };
    ModuleShell envModule_ { "ENVELOPE // VCA", Theme::glowRed(), Theme::panelRed() };
    ModuleShell chorusModule_ { "MODERN BBD CHORUS", Theme::glowCyan(), Theme::panelGrey() };
    ModuleShell driftModule_ { "DRIFT & AGE", Theme::glowBlue(), Theme::panelBlue() };
    ModuleShell arpModule_ { "ARPEGGIATOR", Theme::glowCyan(), Theme::panelGrey() };
    ModuleShell masterModule_ { "MASTER", Theme::glowRed(), Theme::panelRed() };

    juce::TextButton voicePoly1_ { "POLY 1" };
    juce::TextButton voicePoly2_ { "POLY 2" };
    juce::TextButton voiceUnison_ { "UNISON" };
    juce::TextButton voiceMono_ { "MONO" };

    juce::TextButton chorusOff_ { "OFF" };
    juce::TextButton chorusI_ { "CHORUS I" };
    juce::TextButton chorusII_ { "CHORUS II" };
    juce::TextButton chorusBoth_ { "I + II" };

    juce::Component keyboardStrip_;

    std::vector<std::unique_ptr<juce::Slider>> sliders_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
};
} // namespace junovax::ui::celestial
