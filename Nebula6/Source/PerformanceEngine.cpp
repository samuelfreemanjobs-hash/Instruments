#include "PerformanceEngine.h"

#include <algorithm>

namespace play_series {

namespace {
int samplesPer16th(double bpm, double sampleRate) noexcept {
  if (bpm < 20.0)
    bpm = 120.0;
  return static_cast<int>(60.0 / bpm * sampleRate / 4.0);
}
} // namespace

void PerformanceEngine::prepare(double sampleRate, int) noexcept {
  sampleRate_ = sampleRate;
  reset();
}

void PerformanceEngine::reset() noexcept {
  blockCounter_ = 0;
  arpStep_ = 0;
  heldNotes_.clear();
  lastPatternNotes_.clear();
}

void PerformanceEngine::process(juce::MidiBuffer &midi, bool arpOn, int arpMode, int arpRateIndex,
                                int chordMode, int scaleLockMode, double hostBpm) noexcept {
  applyChordMode(midi, chordMode);
  applyScaleLock(midi, scaleLockMode);

  heldNotes_.clear();
  for (const auto meta : midi)
    if (meta.getMessage().isNoteOn())
      heldNotes_.push_back(meta.getMessage().getNoteNumber());

  if (arpOn && !heldNotes_.empty())
    applyArp(midi, arpMode, arpRateIndex, hostBpm);

  lastPatternNotes_.clear();
  for (const auto meta : midi)
    if (meta.getMessage().isNoteOn())
      lastPatternNotes_.push_back(
          static_cast<juce::uint8>(meta.getMessage().getNoteNumber()));
}

void PerformanceEngine::applyChordMode(juce::MidiBuffer &midi, int chordMode) const noexcept {
  if (chordMode <= 0)
    return;

  juce::MidiBuffer out;
  for (const auto meta : midi) {
    const auto &msg = meta.getMessage();
    out.addEvent(msg, meta.samplePosition);
    if (!msg.isNoteOn())
      continue;
    const int root = msg.getNoteNumber();
    auto add = [&](int offset) {
      auto n = juce::MidiMessage::noteOn(msg.getChannel(), root + offset, msg.getVelocity());
      out.addEvent(n, meta.samplePosition);
    };
    switch (chordMode) {
    case 1:
      add(12);
      break;
    case 2:
      add(3);
      add(7);
      add(10);
      break;
    case 3:
      add(3);
      add(7);
      add(10);
      add(14);
      break;
    case 4:
      add(3);
      add(7);
      break;
    default:
      break;
    }
  }
  midi.swapWith(out);
}

int PerformanceEngine::quantizeToScale(int note, int scaleMode) const noexcept {
  if (scaleMode <= 0)
    return note;
  static const int cMinor[] = {0, 2, 3, 5, 7, 8, 10};
  static const int fSharpPhryg[] = {1, 3, 4, 6, 7, 9, 10};
  static const int dDorian[] = {0, 2, 3, 5, 7, 9, 10};
  const int *scale = cMinor;
  int scaleLen = 7;
  if (scaleMode == static_cast<int>(ScaleLockMode::FSharpPhrygianDom)) {
    scale = fSharpPhryg;
  } else if (scaleMode == static_cast<int>(ScaleLockMode::DDorian)) {
    scale = dDorian;
  }
  const int pc = ((note % 12) + 12) % 12;
  int best = scale[0];
  int bestDist = 12;
  for (int i = 0; i < scaleLen; ++i) {
    const int dist = std::abs(pc - scale[i]);
    if (dist < bestDist) {
      bestDist = dist;
      best = scale[i];
    }
  }
  return note - pc + best;
}

void PerformanceEngine::applyScaleLock(juce::MidiBuffer &midi, int scaleMode) const noexcept {
  if (scaleMode <= 0)
    return;
  juce::MidiBuffer out;
  for (const auto meta : midi) {
    auto msg = meta.getMessage();
    if (msg.isNoteOn() || msg.isNoteOff()) {
      const int q = quantizeToScale(msg.getNoteNumber(), scaleMode);
      if (msg.isNoteOn())
        msg = juce::MidiMessage::noteOn(msg.getChannel(), q, msg.getVelocity());
      else
        msg = juce::MidiMessage::noteOff(msg.getChannel(), q);
    }
    out.addEvent(msg, meta.samplePosition);
  }
  midi.swapWith(out);
}

void PerformanceEngine::applyArp(juce::MidiBuffer &midi, int arpMode, int arpRateIndex,
                                 double hostBpm) noexcept {
  if (heldNotes_.empty())
    return;

  const int stepSamples =
      arpRateIndex == 0 ? samplesPer16th(hostBpm, sampleRate_) / 2
                        : samplesPer16th(hostBpm, sampleRate_) / 4;
  if (stepSamples <= 0)
    return;

  std::vector<int> pattern = heldNotes_;
  if (arpMode == 1)
    std::sort(pattern.begin(), pattern.end(), std::greater<int>());
  else if (arpMode == 2)
    std::sort(pattern.begin(), pattern.end());
  else if (arpMode == 3)
    std::sort(pattern.begin(), pattern.end());
  else if (arpMode == 4) {
    const int idx = arpStep_ % static_cast<int>(pattern.size());
    pattern = {pattern[static_cast<size_t>(idx)]};
  }

  midi.clear();
  if (pattern.empty())
    return;

  const int note = pattern[static_cast<size_t>(arpStep_ % static_cast<int>(pattern.size()))];
  if (arpMode == 2 && (arpStep_ % 2 == 1))
    midi.addEvent(juce::MidiMessage::noteOn(1, pattern[static_cast<size_t>(pattern.size() - 1)],
                                            (juce::uint8)110),
                  0);
  else if (arpMode == 2 && arpStep_ % 2 == 0)
    midi.addEvent(juce::MidiMessage::noteOn(1, pattern[0], (juce::uint8)110), 0);
  else
    midi.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8)110), 0);

  blockCounter_++;
  if (blockCounter_ >= stepSamples / 512 + 1) {
    blockCounter_ = 0;
    arpStep_++;
  }
}

bool PerformanceEngine::writePatternToMidiFile(const juce::File &destination,
                                               double bpm) const {
  if (lastPatternNotes_.empty())
    return false;
  juce::MidiFile file;
  file.setTicksPerQuarterNote(480);
  juce::MidiMessageSequence seq;
  for (size_t i = 0; i < lastPatternNotes_.size(); ++i) {
    seq.addEvent(juce::MidiMessage::noteOn(1, lastPatternNotes_[i], (juce::uint8)100),
                 static_cast<double>(i) * 0.5);
    seq.addEvent(juce::MidiMessage::noteOff(1, lastPatternNotes_[i]), static_cast<double>(i) * 0.5 + 0.45);
  }
  juce::MidiMessageSequence tempoMeta;
  tempoMeta.addEvent(juce::MidiMessage::tempoMetaEvent(60.0 / bpm), 0.0);
  file.addTrack(tempoMeta);
  file.addTrack(seq);
  std::unique_ptr<juce::FileOutputStream> stream(destination.createOutputStream());
  if (stream == nullptr)
    return false;
  return file.writeTo(*stream);
}

} // namespace play_series