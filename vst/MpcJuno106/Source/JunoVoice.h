#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

struct JunoSound : public juce::SynthesiserSound {
  bool appliesToNote(int) override { return true; }
  bool appliesToChannel(int) override { return true; }
};

class JunoVoice : public juce::SynthesiserVoice {
public:
  JunoVoice() { filter_.setType(juce::dsp::StateVariableTPTFilterType::lowpass); }

  bool canPlaySound(juce::SynthesiserSound *s) override {
    return dynamic_cast<JunoSound *>(s) != nullptr;
  }

  void prepare(const juce::dsp::ProcessSpec &spec) {
    filter_.prepare(spec);
    adsr_.setSampleRate(spec.sampleRate);
    sampleRate_ = spec.sampleRate;
  }

  void setGlobals(float lfoRate, float lfoPwm) {
    lfoRate_ = lfoRate;
    lfoPwm_ = lfoPwm;
  }

  void updateParams(float cutoff, float res, float saw, float pulse, float pw,
                    float sub, float a, float d, float s, float r) {
    cutoff_ = cutoff;
    res_ = res;
    saw_ = saw;
    pulse_ = pulse;
    pw_ = pw;
    sub_ = sub;
    adsr_.setParameters({a, d, s, r});
  }

  void startNote(int midiNoteNumber, float velocity,
                 juce::SynthesiserSound *, int) override {
    level_ = velocity * 0.35f;
    freq_ = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    phase_ = pulsePhase_ = subPhase_ = 0.0f;
    pwmLfoPhase_ = 0.0f;
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

    const float inc = freq_ / sampleRate_;
    const float subInc = (freq_ * 0.5f) / sampleRate_;
    const float lfoInc = lfoRate_ / sampleRate_;

    for (int i = 0; i < numSamples; ++i) {
      phase_ += inc;
      pulsePhase_ += inc;
      subPhase_ += subInc;
      pwmLfoPhase_ += lfoInc;
      if (phase_ >= 1.0f)
        phase_ -= 1.0f;
      if (pulsePhase_ >= 1.0f)
        pulsePhase_ -= 1.0f;
      if (subPhase_ >= 1.0f)
        subPhase_ -= 1.0f;
      if (pwmLfoPhase_ >= 1.0f)
        pwmLfoPhase_ -= 1.0f;

      const float saw = 2.0f * (phase_ - 0.5f);
      const float w =
          juce::jlimit(0.05f, 0.95f,
                       pw_ + std::sin(pwmLfoPhase_ * juce::MathConstants<float>::twoPi) *
                                 lfoPwm_);
      const float pulse = pulsePhase_ < w ? 1.0f : -1.0f;
      const float subSq = (subPhase_ < 0.5f ? 1.0f : -1.0f) * sub_;

      const float dry = (saw * saw_ + pulse * pulse_ + subSq) * level_;
      filter_.setCutoffFrequency(cutoff_);
      filter_.setResonance(res_);
      const float filtered = filter_.processSample(0, dry) * adsr_.getNextSample();

      for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        outputBuffer.addSample(ch, startSample + i, filtered);
    }

    if (!adsr_.isActive())
      clearCurrentNote();
  }

private:
  juce::dsp::StateVariableTPTFilter<float> filter_{};
  juce::ADSR adsr_{};
  double sampleRate_ = 44100.0;
  float freq_ = 440.0f;
  float phase_ = 0.0f, pulsePhase_ = 0.0f, subPhase_ = 0.0f, pwmLfoPhase_ = 0.0f;
  float level_ = 0.0f;
  float cutoff_ = 2000.0f, res_ = 0.15f;
  float saw_ = 0.8f, pulse_ = 0.0f, pw_ = 0.5f, sub_ = 0.3f;
  float lfoRate_ = 3.0f, lfoPwm_ = 0.0f;
};