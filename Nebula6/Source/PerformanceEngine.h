#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

namespace play_series {

enum class ScaleLockMode : int { Off = 0, CMinor, FSharpPhrygianDom, DDorian, NumModes };

class PerformanceEngine {
public:
  void prepare(double sampleRate, int maxBlock) noexcept;
  void reset() noexcept;

  void process(juce::MidiBuffer &midi, bool arpOn, int arpMode, int arpRateIndex, int chordMode,
               int scaleLockMode, double hostBpm) noexcept;

  const std::vector<juce::uint8> &getLastPatternNotes() const noexcept { return lastPatternNotes_; }
  bool writePatternToMidiFile(const juce::File &destination, double bpm) const;

private:
  void applyChordMode(juce::MidiBuffer &midi, int chordMode) const noexcept;
  void applyScaleLock(juce::MidiBuffer &midi, int scaleMode) const noexcept;
  void applyArp(juce::MidiBuffer &midi, int arpMode, int arpRateIndex, double hostBpm) noexcept;
  int quantizeToScale(int note, int scaleMode) const noexcept;

  double sampleRate_ = 48000.0;
  int blockCounter_ = 0;
  int arpStep_ = 0;
  std::vector<int> heldNotes_;
  std::vector<juce::uint8> lastPatternNotes_;
};

} // namespace play_series