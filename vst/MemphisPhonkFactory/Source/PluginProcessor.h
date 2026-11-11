#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include "Dsp/DrumPattern.h"
#include "Dsp/MemphisPhonkEngine.h"

class MemphisPhonkFactoryAudioProcessor : public juce::AudioProcessor {
public:
  MemphisPhonkFactoryAudioProcessor();
  ~MemphisPhonkFactoryAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int, const juce::String &) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState &getAPVTS() { return apvts_; }
  MemphisPhonkEngine &getEngine() { return engine_; }

  DrumPattern getPattern() const;
  void setPattern(const DrumPattern &pattern);

  void refreshPreviewLoop();

  juce::String exportKitToFolder(const juce::File &parentDir);
  juce::String exportBatchToFolder(const juce::File &parentDir);

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  juce::AudioProcessorValueTreeState apvts_;
  MemphisPhonkEngine engine_;
  DrumPattern pattern_;

  std::shared_ptr<const std::vector<float>> previewLoop_;
  std::atomic<int64_t> playhead_{0};

  void applyEngineSettingsFromParams();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemphisPhonkFactoryAudioProcessor)
};
