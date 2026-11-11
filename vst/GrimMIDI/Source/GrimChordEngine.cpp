#include "GrimChordEngine.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr int kScaleIntervals[4][7] = {
    {0, 2, 3, 5, 7, 8, 10},
    {0, 2, 3, 5, 7, 8, 11},
    {0, 1, 3, 5, 7, 8, 10},
    {0, 1, 4, 5, 7, 8, 10},
};

int clampMidi(int n) noexcept {
  return std::max(0, std::min(127, n));
}

uint32_t lcg(uint32_t& s) noexcept {
  s = s * 1664525u + 1013904223u;
  return s;
}

} // namespace

const int* GrimChordEngine::scaleIntervals(ScaleType scale) noexcept {
  const auto i = static_cast<int>(scale);
  if (i < 0 || i >= 4) {
    return kScaleIntervals[0];
  }
  return kScaleIntervals[i];
}

int GrimChordEngine::scaleDegreeSemitone(ScaleType scale, int degreeIndex1Based,
                                         int keyPc) noexcept {
  const int deg = std::max(1, std::min(7, degreeIndex1Based));
  const int* iv = scaleIntervals(scale);
  const int key = ((keyPc % 12) + 12) % 12;
  return key + iv[deg - 1];
}

int GrimChordEngine::degreeToRootMidi(int keyPc, ScaleType scale, int degree1to7,
                                    int octaveShift) noexcept {
  const int pc = scaleDegreeSemitone(scale, degree1to7, keyPc);
  const int baseOct = 4 + octaveShift;
  return clampMidi(baseOct * 12 + pc);
}

namespace {

constexpr int kDegreePadMap[12] = {1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7};

} // namespace

int GrimChordEngine::rootMidiFromTrigger(int triggerNote, int keyPc, ScaleType scale,
                                         int octaveShift) noexcept {
  const int pc = ((triggerNote % 12) + 12) % 12;
  const int key = ((keyPc % 12) + 12) % 12;
  const int rel = ((pc - key) + 12) % 12;
  const int* iv = scaleIntervals(scale);

  int bestDegree = 0;
  int bestDist = 128;
  for (int d = 0; d < 7; ++d) {
    const int dist = std::abs(iv[d] - rel);
    if (dist < bestDist) {
      bestDist = dist;
      bestDegree = d;
    }
  }

  const int degreePc = (key + iv[bestDegree]) % 12;
  const int octave = triggerNote / 12;
  const int root = octave * 12 + degreePc + octaveShift * 12;
  return clampMidi(root);
}

int GrimChordEngine::rootMidiDegreePads(int triggerNote, int keyPc, ScaleType scale,
                                       int octaveShift) noexcept {
  const int pc = ((triggerNote % 12) + 12) % 12;
  const int degree = kDegreePadMap[pc];
  const int octave = triggerNote / 12;
  const int pcRoot = scaleDegreeSemitone(scale, degree, keyPc) % 12;
  return clampMidi(octave * 12 + pcRoot + octaveShift * 12);
}

int GrimChordEngine::rootMidiChromatic(int triggerNote, int octaveShift) noexcept {
  return clampMidi(triggerNote + octaveShift * 12);
}

int GrimChordEngine::resolveTriggerRoot(TriggerMode mode, int triggerNote, int keyPc, ScaleType scale,
                                       int octaveShift) noexcept {
  switch (mode) {
    case TriggerMode::DegreePads:
      return rootMidiDegreePads(triggerNote, keyPc, scale, octaveShift);
    case TriggerMode::ChromaticRoot:
      return rootMidiChromatic(triggerNote, octaveShift);
    case TriggerMode::NearestDegree:
    default:
      return rootMidiFromTrigger(triggerNote, keyPc, scale, octaveShift);
  }
}

void GrimChordEngine::chordIntervals(ChordQuality q, bool phrygianColor, int& outCount,
                                     int* outIntervals, int maxCount) noexcept {
  outCount = 0;
  auto push = [&](int semi) {
    if (outCount < maxCount) {
      outIntervals[outCount++] = semi;
    }
  };

  switch (q) {
    case ChordQuality::MinorTriad:
      push(0);
      push(3);
      push(7);
      break;
    case ChordQuality::Minor7:
      push(0);
      push(3);
      push(7);
      push(10);
      break;
    case ChordQuality::Minor9:
      push(0);
      push(3);
      push(7);
      push(10);
      push(14);
      break;
    case ChordQuality::Minor11:
      push(0);
      push(3);
      push(7);
      push(10);
      push(14);
      push(17);
      break;
    case ChordQuality::Sus2Minor:
      push(0);
      push(2);
      push(7);
      push(10);
      break;
    case ChordQuality::MinorAdd9:
      push(0);
      push(3);
      push(7);
      push(14);
      break;
    case ChordQuality::Diminished7:
      push(0);
      push(3);
      push(6);
      push(9);
      break;
    case ChordQuality::Fifth:
      push(0);
      push(7);
      break;
    default:
      push(0);
      push(3);
      push(7);
      push(10);
      break;
  }

  if (phrygianColor && outCount > 0 && outCount < maxCount) {
  // Add upper b2 color tone when spread allows (dark phrygian cluster).
    push(13);
  }
}

void GrimChordEngine::applyInversionAndSpread(const int* base, int baseCount, int inversion,
                                              int spreadPct, int rootMidi,
                                              VoicingResult& out) noexcept {
  out.numNotes = 0;
  if (baseCount <= 0) {
    return;
  }

  const int inv = ((inversion % baseCount) + baseCount) % baseCount;
  const int spread = std::max(0, std::min(100, spreadPct));

  int stacked[GrimChordEngine::kMaxChordTones];
  int stackedCount = 0;
  for (int i = 0; i < baseCount && stackedCount < GrimChordEngine::kMaxChordTones; ++i) {
    const int src = (i + inv) % baseCount;
    stacked[stackedCount++] = base[src];
  }

  for (int i = 0; i < stackedCount && out.numNotes < kMaxChordTones; ++i) {
    int octaveBumps = 0;
    if (spread > 0) {
      octaveBumps = (i * spread) / (stackedCount * 40);
    }
    const int note = clampMidi(rootMidi + stacked[i] + octaveBumps * 12);
    bool dup = false;
    for (int j = 0; j < out.numNotes; ++j) {
      if (out.notes[static_cast<size_t>(j)] == note) {
        dup = true;
        break;
      }
    }
    if (!dup) {
      out.notes[static_cast<size_t>(out.numNotes)] = note;
      out.numNotes++;
    }
  }
}

int GrimChordEngine::humanizeVelocity(int velocity, int humanize, uint32_t& rngState) noexcept {
  if (humanize <= 0) {
    return std::max(1, std::min(127, velocity));
  }
  const int delta = static_cast<int>(lcg(rngState) % (static_cast<uint32_t>(humanize) * 2u + 1u))
                    - humanize;
  return std::max(1, std::min(127, velocity + delta));
}

void GrimChordEngine::buildVoicing(const VoicingRequest& req, VoicingResult& out) noexcept {
  const bool phrygian =
      req.addPhrygianColor
      && (req.scale == ScaleType::Phrygian || req.scale == ScaleType::PhrygianDominant);

  int intervals[kMaxChordTones];
  int intervalCount = 0;
  chordIntervals(req.quality, phrygian, intervalCount, intervals, kMaxChordTones);

  int base[kMaxChordTones];
  for (int i = 0; i < intervalCount; ++i) {
    base[i] = intervals[i];
  }

  const int root = clampMidi(req.rootMidi);
  applyInversionAndSpread(base, intervalCount, req.inversion, req.spreadPct, root, out);

  uint32_t rng = static_cast<uint32_t>(root * 1103515245 + intervalCount);
  for (int i = 0; i < out.numNotes; ++i) {
    out.velocities[static_cast<size_t>(i)] =
        humanizeVelocity(req.velocity, req.velocityHumanize, rng);
  }
}
