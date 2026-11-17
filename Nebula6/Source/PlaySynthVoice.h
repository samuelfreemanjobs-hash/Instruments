#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include "LayerAEngine.h"
#include "LayerBEngine.h"
#include "MacroRouter.h"
#include "PlaySynthSound.h"

namespace play_series {

class PlaySynthVoice : public juce::SynthesiserVoice {
public:
  bool canPlaySound(juce::SynthesiserSound *sound) override {
    return dynamic_cast<PlaySynthSound *>(sound) != nullptr;
  }

  void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *, int) override;
  void stopNote(float, bool allowTailOff) override;
  void renderNextBlock(juce::AudioBuffer<float> &output, int start, int numSamples) override;
  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}

  void setMacroTargets(MacroTargets targets) noexcept { targets_ = targets; }
  void setMasterGain(float g) noexcept { masterGain_ = g; }

private:
  LayerAEngine layerA_;
  LayerBEngine layerB_;
  juce::ADSR ampAdsr_;
  juce::dsp::LadderFilter<float> ladder_;
  MacroTargets targets_{};
  float level_ = 0.0f;
  float currentHz_ = 440.0f;
  float masterGain_ = 0.75f;
  bool prepared_ = false;
};

} // namespace play_series