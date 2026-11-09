#pragma once
/**
 * MeterTap editor — design-spec.md is NORMATIVE (480×440, rects, roles, tab order).
 * Widgets: only vst/factory-lib/gui via FactoryGui.h + DesignTokens.h.
 */
#include "PluginProcessor.h"

#include <FactoryGui.h>
#include "MeterBallistics.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

class MeterTapAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
  explicit MeterTapAudioProcessorEditor(MeterTapAudioProcessor&);
  ~MeterTapAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void visibilityChanged() override;

  /** Test hooks: force bypass/over/hold visuals without audio. */
  void setTestBypass(bool on);
  void setTestOverLatched(bool left, bool right);
  void setTestLevels(int channel, float peakDb, float rmsDb, float holdDb, bool over);
  void forcePaintForTest();
  void seedSilenceReadouts();

  fds::gui::LevelMeter& getMeterL() noexcept { return meterL_; }
  fds::gui::LevelMeter& getMeterR() noexcept { return meterR_; }
  fds::gui::LedToggle& getBypassToggle() noexcept { return header_.getBypassToggle(); }
  fds::gui::LedToggle& getHoldToggle() noexcept { return holdToggle_; }
  fds::gui::SectionPanel& getMeterSection() noexcept { return meterSection_; }
  fds::gui::ValueReadout& getPeakL() noexcept { return peakL_; }
  fds::gui::ValueReadout& getPeakR() noexcept { return peakR_; }
  fds::gui::ValueReadout& getRmsL() noexcept { return rmsL_; }
  fds::gui::ValueReadout& getRmsR() noexcept { return rmsR_; }
  fds::gui::RotaryKnob& getTrimKnob() noexcept { return trimKnob_; }
  fds::gui::HeaderBar& getHeader() noexcept { return header_; }
  juce::String getMeterTitleText() const { return meterSection_.getTitleText(); }
  juce::String getTrimValueText();
  juce::String getPresetNameText();

private:
  void timerCallback() override;
  void applyMeterTitle(bool bypassed);
  void clearUiHoldAndOver();
  void updateReadout(fds::gui::ValueReadout& readout, float db, bool overLatched,
                     juce::String& lastText);
  static juce::String formatDbNoUnit(float db);
  static juce::String metersTitleUtf8(bool bypassed);
  static juce::String middleDotUtf8();
  void setupTooltips();
  void setupTabOrder();
  void setupHeaderGlyph();
  void syncTrimValueLabel();

  MeterTapAudioProcessor& proc_;

  fds::gui::FactoryLookAndFeel lnf_;
  fds::gui::HeaderBar header_;
  fds::gui::SectionPanel meterSection_;
  fds::gui::SectionPanel inputSection_;
  fds::gui::SectionPanel readoutSection_;

  fds::gui::LevelMeter meterL_{fds::gui::MeterBarSize::xl};
  fds::gui::LevelMeter meterR_{fds::gui::MeterBarSize::xl};
  fds::gui::MeterScale meterScale_;
  juce::Label legendL_;
  juce::Label legendR_;

  fds::gui::RotaryKnob trimKnob_{"TRIM", fds::gui::KnobSize::lg};

  juce::Label peakHeader_;
  juce::Label rmsHeader_;
  juce::Label rowLegendL_;
  juce::Label rowLegendR_;
  fds::gui::ValueReadout peakL_;
  fds::gui::ValueReadout peakR_;
  fds::gui::ValueReadout rmsL_;
  fds::gui::ValueReadout rmsR_;
  fds::gui::LedToggle holdToggle_{"HOLD", fds::gui::LedRole::standard, fds::gui::ToggleSize::md};
  juce::TextButton clearButton_{"CLEAR"};

  /** Caution dot before meter title when bypassed (design-spec §4). */
  class CautionDot : public juce::Component
  {
  public:
    void paint(juce::Graphics& g) override
    {
      g.setColour(fds::gui::argb(fds::color::state::caution));
      g.fillEllipse(getLocalBounds().toFloat());
    }
  };
  CautionDot bypassCautionDot_;
  bool showBypassDot_ = false;

  std::array<dl::analysis::MeterBallistics, 2> ballistics_{};
  std::array<bool, 2> overLatched_{{false, false}};
  juce::int64 lastTimerMs_ = 0;

  juce::String lastPeakLText_;
  juce::String lastPeakRText_;
  juce::String lastRmsLText_;
  juce::String lastRmsRText_;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> holdAttach_;
  std::unique_ptr<juce::TooltipWindow> tooltipWindow_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterTapAudioProcessorEditor)
};
