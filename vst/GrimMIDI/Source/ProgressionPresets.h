#pragma once

#include <array>
#include <cstdint>

struct ProgressionStep {
  int8_t degree; // 1-7 diatonic degree, 0 = rest
  uint8_t beats; // quarter-note beats at 4/4
};

struct ProgressionPreset {
  const char* name;
  uint8_t bpm;
  /** 0-11 when preset pins key; 255 = follow global APVTS key. */
  uint8_t keyPc;
  /** GrimChordEngine::ScaleType as uint8. */
  uint8_t scale;
  /** GrimChordEngine::ChordQuality as uint8. */
  uint8_t chordQuality;
  std::array<ProgressionStep, 16> steps {};
  uint8_t numSteps;
};
