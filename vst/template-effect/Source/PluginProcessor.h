#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "biquad.h"
#include "parameter_smoothing.h"

class TemplateEffectAudioProcessor : public juce::AudioProcessor {
public:
  TemplateEffectAudioProcessor();
  ~TemplateEffectAudioProcessor() override;

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
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  static float normToCutoff(float norm, float sampleRate);
  static float softClip(float x, float drive);

  juce::AudioProcessorValueTreeState apvts_;
  double sampleRate_ = 48000.0;
  DspBiquad toneL_;
  DspBiquad toneR_;
  DspParamSmooth driveSmooth_;
  DspParamSmooth toneSmooth_;
  DspParamSmooth mixSmooth_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TemplateEffectAudioProcessor)
};
