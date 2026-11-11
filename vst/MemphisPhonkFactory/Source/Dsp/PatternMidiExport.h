#pragma once

#include "DrumPattern.h"
#include <juce_audio_basics/juce_audio_basics.h>

namespace PatternMidiExport {
constexpr int kKick = 36;
constexpr int kSnare = 38;
constexpr int kClosedHat = 42;
constexpr int kOpenHat = 46;

inline bool writePatternMidi(const juce::File &dest, float bpm, int bars, float swing,
                             const DrumPattern &pattern) {
  const int totalSteps = bars * 16;
  const double ppq = 480.0;
  const double stepDurBeats = 0.25; // 16th note
  juce::MidiMessageSequence seq;

  for (int step = 0; step < totalSteps; ++step) {
    const int barStep = step % 16;
    double tBeats = step * stepDurBeats;
    if (step % 2 == 1) {
      tBeats += stepDurBeats * swing;
    }
    const double tSec = tBeats * (60.0 / bpm);
    auto noteOn = [&](int note, uint8_t vel) {
      seq.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(vel)), tSec);
      seq.addEvent(juce::MidiMessage::noteOff(1, note), tSec + 0.05);
    };

    if (pattern.stepKick(barStep)) {
      noteOn(kKick, 110);
    }
    if (pattern.stepSnare(barStep)) {
      noteOn(kSnare, 105);
    }
    if (pattern.stepOpenHat(barStep)) {
      noteOn(kOpenHat, 90);
    } else {
      noteOn(kClosedHat, barStep % 2 == 0 ? 80 : 55);
    }
  }

  juce::MidiFile file;
  file.setTicksPerQuarterNote(static_cast<int>(ppq));
  file.addTrack(seq);

  juce::FileOutputStream out(dest);
  if (!out.openedOk()) {
    return false;
  }
  return file.writeTo(out, 0);
}
} // namespace PatternMidiExport
