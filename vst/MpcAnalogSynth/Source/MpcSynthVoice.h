#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

struct MpcSynthSound : public juce::SynthesiserSound {
  bool appliesToNote(int) override { return true; }
  bool appliesToChannel(int) override { return true; }
};

class MpcSynthVoice : public juce::SynthesiserVoice {
public:
  MpcSynthVoice() {
    osc1_.initialise([](float x) { return std::sin(x); }, 128);
    osc2_.initialise(
        [](float x) { return x / juce::MathConstants<float>::pi; }, 128);
    filter_.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
  }

  bool canPlaySound(juce::SynthesiserSound *s) override {
    return dynamic_cast<MpcSynthSound *>(s) != nullptr;
  }

  void prepare(const juce::dsp::ProcessSpec &spec) {
    osc1_.prepare(spec);
    osc2_.prepare(spec);
    filter_.prepare(spec);
    adsr_.setSampleRate(spec.sampleRate);
  }

  void updateParams(float cutoff, float res, float detune, float a, float d,
                    float s, float r) {
    filter_.setCutoffFrequency(cutoff);
    filter_.setResonance(res);
    detuneRatio_ = std::pow(2.0f, detune / 12.0f);
    adsr_.setParameters({a, d, s, r});
  }

  void startNote(int midiNoteNumber, float velocity,
                 juce::SynthesiserSound *, int) override {
    level_ = velocity * 0.25f;
    const float freq =
        static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    osc1_.setFrequency(freq, true);
    osc2_.setFrequency(freq * detuneRatio_, true);
    adsr_.noteOn();
  }

  void stopNote(float, bool allowTailOff) override {
    adsr_.noteOff();
    if (!allowTailOff || !adsr_.isActive())
      clearCurrentNote();
  }

  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}

  void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample,
                       int numSamples) override {
    if (!adsr_.isActive())
      return;

    for (int i = 0; i < numSamples; ++i) {
      const float raw =
          (osc1_.processSample(0.0f) + osc2_.processSample(0.0f)) * level_;
      const float filtered = filter_.processSample(0, raw) * adsr_.getNextSample();

      for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        outputBuffer.addSample(ch, startSample + i, filtered);
    }

    if (!adsr_.isActive())
      clearCurrentNote();
  }

private:
  juce::dsp::Oscillator<float> osc1_{}, osc2_{};
  juce::dsp::StateVariableTPTFilter<float> filter_{};
  juce::ADSR adsr_{};
  float level_ = 0.0f;
  float detuneRatio_ = 1.0f;
};