#include "PlaySynthVoice.h"

namespace play_series {

void PlaySynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *, int) {
  const double sampleRate = getSampleRate();
  if (sampleRate > 0.0 && !prepared_) {
    juce::dsp::ProcessSpec spec{sampleRate, 512, 1};
    ladder_.prepare(spec);
    ladder_.setMode(juce::dsp::LadderFilterMode::LPF12);
    ladder_.reset();
    juce::ADSR::Parameters p;
    p.attack = 0.01f;
    p.decay = 0.15f;
    p.sustain = 0.7f;
    p.release = 0.25f;
    ampAdsr_.setParameters(p);
    ampAdsr_.setSampleRate(sampleRate);
    layerA_.prepare(sampleRate);
    layerB_.prepare(sampleRate);
    prepared_ = true;
  }
  currentHz_ = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
  level_ = velocity * 0.35f;
  ampAdsr_.noteOn();
  layerA_.reset();
  layerB_.reset();
}

void PlaySynthVoice::stopNote(float, bool allowTailOff) {
  if (allowTailOff)
    ampAdsr_.noteOff();
  else {
    ampAdsr_.reset();
    clearCurrentNote();
  }
}

void PlaySynthVoice::renderNextBlock(juce::AudioBuffer<float> &output, int startSample,
                                     int numSamples) {
  if (!isVoiceActive() || !prepared_)
    return;

  juce::dsp::AudioBlock<float> block(output);
  auto slice = block.getSubBlock(static_cast<size_t>(startSample), static_cast<size_t>(numSamples));

  ladder_.setCutoffFrequencyHz(targets_.filterCutoffHz);
  ladder_.setResonance(targets_.filterResonance);

  const float blendA = equalPowerBlendA(targets_.layerABlend);
  const float blendB = equalPowerBlendB(targets_.layerABlend);

  for (int i = 0; i < numSamples; ++i) {
    float a = layerA_.nextSample(currentHz_, targets_.layerABrightness, targets_.subLevel);
    float b = layerB_.nextSample(currentHz_, targets_.layerBWavetable, targets_.layerBAir) *
              targets_.layerBBoost;
    float s = a * blendA + b * blendB;
    if (targets_.masterDirt > 0.01f) {
      const float drive = 1.0f + targets_.masterDirt * 4.0f;
      s = std::tanh(s * drive) / drive;
    }
    const float env = ampAdsr_.getNextSample();
    s *= env * level_ * masterGain_;

    float filtered = s;
    float *chans[] = {&filtered};
    juce::dsp::AudioBlock<float> one(chans, 1, 1);
    juce::dsp::ProcessContextReplacing<float> ctx(one);
    ladder_.process(ctx);
    const float out = filtered;

    for (int ch = 0; ch < output.getNumChannels(); ++ch)
      output.addSample(ch, startSample + i, out);

    if (!ampAdsr_.isActive()) {
      clearCurrentNote();
      break;
    }
  }
}

} // namespace play_series