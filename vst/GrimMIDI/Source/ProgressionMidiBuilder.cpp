#include "ProgressionMidiBuilder.h"

bool buildProgressionMidiFile(const ProgressionPreset& preset, const ProgressionRenderParams& params,
                            juce::MidiFile& outFile) {
  if (preset.numSteps == 0) {
    return false;
  }

  const int rootKey = (preset.keyPc < 12) ? static_cast<int>(preset.keyPc) : params.rootKeyPc;
  const auto scale = static_cast<GrimChordEngine::ScaleType>(preset.scale);
  const auto quality = static_cast<GrimChordEngine::ChordQuality>(preset.chordQuality);

  outFile.setTicksPerQuarterNote(480);
  juce::MidiMessageSequence seq;

  const double bpm = static_cast<double>(preset.bpm);
  seq.addEvent(juce::MidiMessage::tempoMetaEvent(static_cast<int>(60000000.0 / bpm)), 0);
  const double secondsPerBeat = 60.0 / bpm;
  double timeSec = 0.0;

  for (int s = 0; s < preset.numSteps; ++s) {
    const auto& step = preset.steps[static_cast<size_t>(s)];
    if (step.degree <= 0 || step.beats == 0) {
      continue;
    }

    const int root =
        GrimChordEngine::degreeToRootMidi(rootKey, scale, step.degree, params.octaveShift);

    GrimChordEngine::VoicingRequest req;
    req.rootMidi = root;
    req.scale = scale;
    req.quality = quality;
    req.inversion = params.inversion;
    req.spreadPct = params.spreadPct;
    req.octaveShift = 0;
    req.velocity = 100;
    req.velocityHumanize = 0;
    req.addPhrygianColor = true;

    GrimChordEngine::VoicingResult chord;
    GrimChordEngine::buildVoicing(req, chord);

    const double stepDur = secondsPerBeat * static_cast<double>(step.beats);
    const double strumStep =
        (chord.numNotes > 1 && params.strumTotalSeconds > 0.0)
            ? params.strumTotalSeconds / static_cast<double>(chord.numNotes - 1)
            : 0.0;

    for (int i = 0; i < chord.numNotes; ++i) {
      const double noteOnSec = timeSec + strumStep * static_cast<double>(i);
      const double noteOffSec = timeSec + stepDur;
      seq.addEvent(juce::MidiMessage::noteOn(1, chord.notes[static_cast<size_t>(i)],
                                             static_cast<juce::uint8>(chord.velocities[static_cast<size_t>(i)])),
                   noteOnSec);
      seq.addEvent(juce::MidiMessage::noteOff(1, chord.notes[static_cast<size_t>(i)]), noteOffSec);
    }

    timeSec += stepDur;
  }

  outFile.addTrack(seq);
  return seq.getNumEvents() > 0;
}
