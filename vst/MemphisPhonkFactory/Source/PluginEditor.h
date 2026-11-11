#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "Ui/PatternGridComponent.h"

class MemphisPhonkFactoryAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::AudioProcessorValueTreeState::Listener,
      private juce::Timer {
public:
  explicit MemphisPhonkFactoryAudioProcessorEditor(MemphisPhonkFactoryAudioProcessor &);
  ~MemphisPhonkFactoryAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  MemphisPhonkFactoryAudioProcessor &processor_;

  PatternGridComponent patternGrid_;
  juce::Slider bpmSlider_;
  juce::TextButton bpmDown_{"−"};
  juce::TextButton bpmUp_{"+"};
  juce::TextButton tapTempo_{"Tap"};
  juce::Label bpmValueLabel_;
  juce::Label loopInfoLabel_;

  juce::Slider swingSlider_;
  juce::Slider dustSlider_;
  juce::Slider masterSlider_;
  juce::Slider variationSlider_;
  juce::Slider batchCountSlider_;
  juce::Slider batchFromSlider_;
  juce::Slider batchToSlider_;

  juce::ComboBox barsBox_;
  juce::ToggleButton previewButton_;
  juce::ToggleButton halfTimeButton_;
  juce::ToggleButton syncHostButton_;
  juce::ToggleButton tightenButton_;
  juce::ToggleButton classicLoopsButton_;
  juce::ToggleButton stemsButton_;
  juce::ToggleButton midiButton_;
  juce::ToggleButton manifestButton_;
  juce::ToggleButton batchSweepButton_;

  juce::TextButton exportButton_{"Export MPC pack"};
  juce::TextButton batchExportButton_{"Batch export"};
  juce::ProgressBar exportProgress_{exportProgressValue_};
  double exportProgressValue_ = 0.0;
  juce::Label statusLabel_;

  std::vector<double> tapTimesSec_;

  using SAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<SAtt> swingAtt_, dustAtt_, masterAtt_, variationAtt_, batchAtt_, batchFromAtt_,
      batchToAtt_, bpmAtt_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> barsAtt_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> previewAtt_, halfAtt_,
      syncAtt_, tightenAtt_, classicAtt_, stemsAtt_, midiAtt_, manifestAtt_, sweepAtt_;

  void timerCallback() override;
  void parameterChanged(const juce::String &parameterID, float newValue) override;
  void nudgeBpm(float delta);
  void syncBpmLabel();
  void syncLoopInfo();
  void registerTapTempo();
  void wirePatternTools();
  void runExport(bool batch);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemphisPhonkFactoryAudioProcessorEditor)
};
