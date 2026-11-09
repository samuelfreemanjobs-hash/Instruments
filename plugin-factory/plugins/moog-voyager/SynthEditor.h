#pragma once

#include "SynthProcessor.h"
#include "MoogStackLineage.h"
#include "VoyagerUiTokens.h"
#include <JuceHeader.h>
#include <array>

class VoyagerPanelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Colour accent { 0xfff4a62a };

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                          float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle,
                          juce::Slider&) override;
};

/** Four-position wave rotary (mockup-style) bound to APVTS wave choice. */
class WaveformSelectorStrip : public juce::Component
{
public:
    void bind(MoogVoyagerAudioProcessor& processor, std::string_view waveParamId, int oscIndex);
    void setAccentColour(juce::Colour c) noexcept;
    void syncFromParameter();

    void resized() override;

private:
    MoogVoyagerAudioProcessor* proc = nullptr;
    std::string paramKey;
    juce::Colour accent { 0xfff4a62a };
    std::array<juce::TextButton, 4> waveButtons {};
    void selectWave(int index);
    static void paintWaveGlyph(juce::Graphics& g, juce::Rectangle<float> area, int waveIndex, juce::Colour colour);
};

/** Mockup performance row: XY surface, pitch/mod wheels, on-screen MIDI keyboard. */
class ModBusMatrixPanel : public juce::Component
{
public:
    void setAccentColour(juce::Colour c) noexcept { accent = c; }
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    juce::Colour accent { 0xfff4a62a };
    /** [source row][bus column 0 = A, 1 = B] */
    std::array<std::array<bool, 2>, 4> routes { { { true, true }, { false, true }, { true, false }, { false, false } } };
    bool hitCell(juce::Point<float> pos, int& rowOut, int& colOut) const;
};

class VoyagerPerformanceStrip : public juce::Component
{
public:
    VoyagerPerformanceStrip(MoogVoyagerAudioProcessor& p, juce::MidiKeyboardComponent& keyboard);

    void setAccentColour(juce::Colour c) noexcept { accent = c; }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    MoogVoyagerAudioProcessor& processor;
    juce::MidiKeyboardComponent& keyboard;
    juce::Colour accent { 0xfff4a62a };
    float xyX = 0.5f;
    float xyY = 0.5f;
    float pitchWheel = 0.5f;
    float modWheel = 0.25f;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    juce::Rectangle<float> xyBounds {};
    juce::Rectangle<float> pitchWheelBounds {};
    juce::Rectangle<float> modWheelBounds {};
    void applyPerformanceToEngine();
};

class MeterScopePanel : public juce::Component
{
public:
    explicit MeterScopePanel(MoogVoyagerAudioProcessor& p) : processor(p) {}

    void setAccentColour(juce::Colour c) noexcept { accent = c; }

    void paint(juce::Graphics& g) override;

private:
    MoogVoyagerAudioProcessor& processor;
    juce::Colour accent { 0xfff4a62a };
};

class MoogVoyagerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::AudioProcessorValueTreeState::Listener,
                                        private juce::Timer
{
public:
    explicit MoogVoyagerAudioProcessorEditor(MoogVoyagerAudioProcessor&);
    ~MoogVoyagerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void refreshPatchReadout();
    void refreshStackChrome();
    juce::Colour accentForCurrentStack() const;
    void selectStackMode(int index, bool applyTemplate);
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void timerCallback() override;

private:
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    static juce::Colour accentForLineage(moogstack::Lineage lineage);
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    static void styleKnob(juce::Slider& s, const juce::String& name);
    static void styleMixFader(juce::Slider& s, const juce::String& name);
    static void styleEnvSlider(juce::Slider& s, const juce::String& name);
    static void bindCombo(juce::ComboBox& box, const juce::StringArray& items);
    static void sectionLabel(juce::Label& label, const juce::String& text);
    void applyAccentToSlider(juce::Slider& s, juce::Colour accent, bool isFader) const;
    void applyLookAndFeelToControls();
    void syncFilterKbStepsFromParameter();
    void setFilterKbStep(int stepIndex);

    MoogVoyagerAudioProcessor& processorRef;
    VoyagerPanelLookAndFeel panelLookAndFeel;

    int ledPhase = 0;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label patchReadoutLabel;
    juce::Label stackBadgeLabel;
    juce::Label stackSwitchLabel;
    std::array<juce::TextButton, 4> stackModeSwitch;
    juce::Label presetLabel;
    juce::ComboBox presetBox;
    juce::TextButton presetPrevButton;
    juce::TextButton presetNextButton;
    juce::Label filterEnvRowLabel;
    juce::Label ampEnvRowLabel;
    juce::Label oversampleLabel;
    juce::ComboBox oversampleBox;
    MeterScopePanel meterScope;
    juce::ToggleButton noisePinkButton;
    juce::ToggleButton mixerOdButton;
    juce::ToggleButton filterOdButton;
    juce::Label modLfoLabel;
    juce::Label modWheelPitchLabel;
    juce::Label modWheelFilterLabel;
    ModBusMatrixPanel modBusMatrix;
    juce::ToggleButton osc23SyncButton;
    std::array<juce::TextButton, 3> oversampleSwitch;
    juce::Label filterKbStepLabel;
    std::array<juce::TextButton, 4> filterKbStepSwitch;
    juce::Label outputSectionLabel;
    juce::Label cpuStatusLabel;
    juce::MidiKeyboardComponent midiKeyboard;
    VoyagerPerformanceStrip performanceStrip;
    juce::Label oscSectionLabel, filterSectionLabel, envSectionLabel, modSectionLabel;

    juce::Slider masterSlider;
    juce::Slider oscLevel[3], oscFine[3], noiseSlider;
    juce::ComboBox oscOctave[3], oscWave[3];
    std::array<WaveformSelectorStrip, 3> waveSelectors;
    juce::Label oscIndexLabel[3], oscOctLabel[3], oscWaveLabel[3];

    juce::Slider filterCutoff, filterRes, filterDrive, filterEnvAmt, filterKb, mixerDriveSlider, subOscSlider;
    juce::Slider fAttack, fDecay, fSustain, fRelease;
    juce::Slider aAttack, aDecay, aSustain, aRelease;
    juce::Slider lfoRate, lfoPitch, lfoFilter, glide;

    std::unique_ptr<SliderAttachment> masterAttachment;
    std::array<std::unique_ptr<SliderAttachment>, 3> oscLevelAttachments, oscFineAttachments;
    std::array<std::unique_ptr<ComboAttachment>, 3> oscOctaveAttachments, oscWaveAttachments;
    std::unique_ptr<SliderAttachment> noiseAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment, filterDriveAttachment;
    std::unique_ptr<SliderAttachment> filterEnvAmtAttachment, filterKbAttachment, mixerDriveAttachment;
    std::unique_ptr<SliderAttachment> subOscAttachment;
    std::unique_ptr<ButtonAttachment> noisePinkAttachment;
    std::unique_ptr<ButtonAttachment> mixerOdAttachment;
    std::unique_ptr<ButtonAttachment> filterOdAttachment;
    std::unique_ptr<ComboAttachment> oversampleAttachment;
    std::unique_ptr<ButtonAttachment> osc23SyncAttachment;
    std::unique_ptr<SliderAttachment> fAttackAttachment, fDecayAttachment, fSustainAttachment, fReleaseAttachment;
    std::unique_ptr<SliderAttachment> aAttackAttachment, aDecayAttachment, aSustainAttachment, aReleaseAttachment;
    std::unique_ptr<SliderAttachment> lfoRateAttachment, lfoPitchAttachment, lfoFilterAttachment, glideAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MoogVoyagerAudioProcessorEditor)
};
