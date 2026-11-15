#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include "Dsp/DrumPattern.h"
#include "Dsp/MemphisPhonkEngine.h"
#include "Dsp/PackExportOptions.h"
#include "Export/ExportWorker.h"

class MemphisPhonkFactoryAudioProcessor : public juce::AudioProcessor,
                                          private juce::Timer {
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

  DrumPattern getPattern() const;
  void setPattern(const DrumPattern &pattern);

  void setPatternB(const DrumPattern &p) { patternB_ = p; }
  DrumPattern getPatternB() const { return patternB_; }
  void togglePatternAB();
  bool isUsingPatternB() const { return usePatternB_; }

  void shiftPattern(int stepsRight);
  bool savePatternToFile(const juce::File &f) const;
  bool loadPatternFromFile(const juce::File &f);

  void schedulePreviewRefresh();
  void refreshPreviewLoop();

  float getEffectiveBpm() const;
  float getLoopDurationSec() const;

  void startBackgroundExport(const juce::File &parent, bool batch,
                             std::function<void(juce::String)> onComplete,
                             std::function<void(float)> onProgress = {});
  bool isExportBusy() const { return exportWorker_.isBusy(); }

  PackExportOptions buildPackOptions() const;
  MemphisPhonkEngine::Settings buildEngineSettings() const;

  ExportWorker &getExportWorker() { return exportWorker_; }

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  juce::AudioProcessorValueTreeState apvts_;
  MemphisPhonkEngine engine_;
  ExportWorker exportWorker_;

  DrumPattern patternA_;
  DrumPattern patternB_;
  bool usePatternB_ = false;

  std::shared_ptr<const std::vector<float>> previewLoop_;
  std::atomic<int64_t> playhead_{0};
  std::atomic<float> hostBpm_{0.0f};
  bool previewRefreshPending_ = false;

  void timerCallback() override;

  void applyEngineSettingsFromParams();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemphisPhonkFactoryAudioProcessor)
};
