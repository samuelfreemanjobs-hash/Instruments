#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "GrimChordEngine.h"
#include "ProgressionPresetsData.h"

struct ProgressionRenderParams {
  int rootKeyPc = 0;
  GrimChordEngine::ScaleType scale = GrimChordEngine::ScaleType::NaturalMinor;
  GrimChordEngine::ChordQuality chordQuality = GrimChordEngine::ChordQuality::Minor7;
  int inversion = 0;
  int spreadPct = 45;
  /** Total strum duration across a chord (seconds); 0 = simultaneous. */
  double strumTotalSeconds = 0.0;
  int octaveShift = 0;
};

bool buildProgressionMidiFile(const ProgressionPreset& preset, const ProgressionRenderParams& params,
                            juce::MidiFile& outFile);
