#include <juce_audio_processors/juce_audio_processors.h>

#include "../Source/PluginProcessor.h"

static void processOneBlock(XDPatchArchitectAudioProcessor &proc) {
  juce::MidiBuffer midi;
  juce::AudioBuffer<float> buf{2, 64};
  proc.prepareToPlay(48000.0, 64);
  proc.processBlock(buf, midi);
}

int main() {
  XDPatchArchitectAudioProcessor proc;
  if (!proc.isMidiEffect()) {
    return 1;
  }
  processOneBlock(proc);
  return 0;
}
