/**
 * Contract tests: one note in -> chord out, note-off balance, parameter panic, progression export.
 */
#include "../Source/PluginProcessor.h"

#include <cmath>
#include <cstdio>
#include <set>
#include <vector>

namespace {

int gFailures = 0;

void expect(bool cond, const char* name) {
  if (!cond) {
    std::fprintf(stderr, "FAIL: %s\n", name);
    ++gFailures;
  } else {
    std::printf("PASS: %s\n", name);
  }
}

void setParamChoice(GrimMIDIAudioProcessor& p, const char* id, int index) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(index)));
    param->endChangeGesture();
  }
}

void setParamFloat(GrimMIDIAudioProcessor& p, const char* id, float v) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->convertTo0to1(v));
    param->endChangeGesture();
  }
}

std::vector<juce::MidiMessage> collectMessages(const juce::MidiBuffer& midi) {
  std::vector<juce::MidiMessage> out;
  for (const auto metadata : midi) {
    out.push_back(metadata.getMessage());
  }
  return out;
}

int countNoteOns(const std::vector<juce::MidiMessage>& msgs) {
  int n = 0;
  for (const auto& m : msgs) {
    if (m.isNoteOn()) {
      ++n;
    }
  }
  return n;
}

int countNoteOffs(const std::vector<juce::MidiMessage>& msgs) {
  int n = 0;
  for (const auto& m : msgs) {
    if (m.isNoteOff()) {
      ++n;
    }
  }
  return n;
}

} // namespace

int main() {
  constexpr double kSr = 48000.0;
  constexpr int kBlock = 256;

  GrimMIDIAudioProcessor proc;
  proc.prepareToPlay(kSr, kBlock);

  setParamChoice(proc, GrimMIDIAudioProcessor::kParamRootKey, 0); // C
  setParamChoice(proc, GrimMIDIAudioProcessor::kParamScale, 0);
  setParamChoice(proc, GrimMIDIAudioProcessor::kParamChordType, 1); // min7
  setParamFloat(proc, GrimMIDIAudioProcessor::kParamStrumMs, 0.0f);
  setParamChoice(proc, GrimMIDIAudioProcessor::kParamVelHumanize, 0);

  // --- one note on -> multi note chord ---
  {
    juce::AudioBuffer<float> audio(2, kBlock);
    juce::MidiBuffer in;
    in.addEvent(juce::MidiMessage::noteOn(1, 60, static_cast<juce::uint8>(100)), 0);

    juce::MidiBuffer out;
    out.swapWith(in);
    proc.processBlock(audio, out);
    const auto msgs = collectMessages(out);
    expect(countNoteOns(msgs) >= 3, "note on produces chord (>=3 notes)");
  }

  // --- balanced note-offs ---
  {
    juce::AudioBuffer<float> audio(2, kBlock);
    juce::MidiBuffer buf;
    buf.addEvent(juce::MidiMessage::noteOn(1, 62, static_cast<juce::uint8>(90)), 0);
    proc.processBlock(audio, buf);
  const int onCount = countNoteOns(collectMessages(buf));

    buf.clear();
    buf.addEvent(juce::MidiMessage::noteOff(1, 62), 0);
    proc.processBlock(audio, buf);
    const int offCount = countNoteOffs(collectMessages(buf));
    expect(offCount == onCount, "note-off count matches prior chord note-ons");
  }

  // --- parameter change issues all-notes-off style cleanup ---
  {
    juce::AudioBuffer<float> audio(2, kBlock);
    juce::MidiBuffer buf;
    buf.addEvent(juce::MidiMessage::noteOn(1, 64, static_cast<juce::uint8>(80)), 0);
    proc.processBlock(audio, buf);
  const int chordSize = countNoteOns(collectMessages(buf));

    setParamFloat(proc, GrimMIDIAudioProcessor::kParamSpread, 80.0f);
    buf.clear();
    proc.processBlock(audio, buf);
    expect(countNoteOffs(collectMessages(buf)) >= chordSize,
           "parameter change releases active chord voices");
  }

  // --- all notes off controller ---
  {
    juce::AudioBuffer<float> audio(2, kBlock);
    juce::MidiBuffer buf;
    buf.addEvent(juce::MidiMessage::noteOn(1, 67, static_cast<juce::uint8>(80)), 0);
    proc.processBlock(audio, buf);

    buf.clear();
    buf.addEvent(juce::MidiMessage::controllerEvent(1, 123, 0), 0);
    proc.processBlock(audio, buf);
    expect(countNoteOffs(collectMessages(buf)) >= 3, "CC123 releases chord");
  }

  // --- progression MIDI export has events ---
  {
    juce::MidiFile file;
    const bool ok = proc.buildProgressionMidiFile(file);
    expect(ok, "progression MIDI export builds");
    if (ok && file.getNumTracks() > 0) {
      expect(file.getTrack(0)->getNumEvents() >= 4, "progression MIDI has multiple events");
    }
  }

  std::printf("Done. failures=%d\n", gFailures);
  return gFailures == 0 ? 0 : 1;
}
