#pragma once

#include <array>
#include <cstdint>

/** Scale + voicing helpers for Grim-MIDI (no heap; audio-thread safe). */
struct GrimChordEngine {
  static constexpr int kMaxChordTones = 8;

  enum class ScaleType : int {
    NaturalMinor = 0,
    HarmonicMinor,
    Phrygian,
    PhrygianDominant,
    kCount
  };

  enum class ChordQuality : int {
    MinorTriad = 0,
    Minor7,
    Minor9,
    Minor11,
    Sus2Minor,
    MinorAdd9,
    Diminished7,
    Fifth,
    kCount
  };

  struct VoicingRequest {
    int rootMidi = 60;
    ScaleType scale = ScaleType::NaturalMinor;
    ChordQuality quality = ChordQuality::Minor7;
    int inversion = 0;
    int spreadPct = 40;
    int octaveShift = 0;
    int velocity = 100;
    int velocityHumanize = 0;
    bool addPhrygianColor = true;
  };

  struct VoicingResult {
    int numNotes = 0;
    std::array<int, kMaxChordTones> notes {};
    std::array<int, kMaxChordTones> velocities {};
  };

  static int scaleDegreeSemitone(ScaleType scale, int degreeIndex1Based, int keyPc) noexcept;

  static void buildVoicing(const VoicingRequest& req, VoicingResult& out) noexcept;

  static int degreeToRootMidi(int keyPc, ScaleType scale, int degree1to7, int octaveShift) noexcept;

  enum class TriggerMode : int {
    NearestDegree = 0,
    DegreePads,
    ChromaticRoot,
    kCount
  };

  /** Map incoming pitch class to closest diatonic degree root (MIDI note). */
  static int rootMidiFromTrigger(int triggerNote, int keyPc, ScaleType scale, int octaveShift) noexcept;

  static int rootMidiDegreePads(int triggerNote, int keyPc, ScaleType scale,
                                 int octaveShift) noexcept;

  static int rootMidiChromatic(int triggerNote, int octaveShift) noexcept;

  static int resolveTriggerRoot(TriggerMode mode, int triggerNote, int keyPc, ScaleType scale,
                                int octaveShift) noexcept;

private:
  static const int* scaleIntervals(ScaleType scale) noexcept;
  static void chordIntervals(ChordQuality q, bool phrygianColor, int& outCount,
                             int* outIntervals, int maxCount) noexcept;
  static void applyInversionAndSpread(const int* base, int baseCount, int inversion, int spreadPct,
                                      int rootMidi, VoicingResult& out) noexcept;
  static int humanizeVelocity(int velocity, int humanize, uint32_t& rngState) noexcept;
};
