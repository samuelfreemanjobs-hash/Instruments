#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MpcCs80AudioProcessor : public juce::AudioProcessor {
public:
  static constexpr int kMaxVoices = 4;

  MpcCs80AudioProcessor();
  ~MpcCs80AudioProcessor() override = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override { return nullptr; }
  bool hasEditor() const override { return false; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override;
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int, const juce::String &) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
  juce::AudioProcessorValueTreeState &getAPVTS() { return apvts_; }

private:
  void applyFactoryProgram(int index);
  float renderLayer(float phase1, float phase2, float subPhase, float shape,
                    float pwm, float ringAmt, float subLvl);

  struct Voice {
    bool active = false;
    int note = -1;
    float vel = 0.0f;
    float phase1 = 0.0f, phase2 = 0.0f, subPhase = 0.0f;
    float freq = 110.0f;
    float pressure = 0.0f;
    juce::ADSR ampEnv, filterEnv;
    juce::dsp::StateVariableTPTFilter<float> hpf, lpf;
  };

  juce::AudioProcessorValueTreeState apvts_;
  std::array<Voice, kMaxVoices> voices_{};
  double sampleRate_ = 44100.0;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcCs80AudioProcessor)
};