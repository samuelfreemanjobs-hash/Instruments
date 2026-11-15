#pragma once

#include "DrumPattern.h"

/** MPC-oriented export bundle (WAV + optional stems/MIDI/manifest). */
struct PackExportOptions {
  float primaryBpm = 118.0f;
  float swing = 0.03f;
  int bars = 2;
  DrumPattern pattern;

  bool includeClassicTempos = true;
  bool exportStems = false;
  bool exportMidi = true;
  bool writeMpcManifest = true;

  /** Per-kit BPM spread when batching (kit 1 → from, kit N → to). */
  bool batchBpmSweep = false;
  float batchBpmFrom = 110.0f;
  float batchBpmTo = 130.0f;

  static float loopDurationSec(float bpm, int bars) {
    return static_cast<float>(bars) * 4.0f * (60.0f / bpm);
  }
};
