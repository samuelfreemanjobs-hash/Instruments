#include "PluginProcessor.h"

#include "PluginEditor.h"

namespace {

constexpr const char* kScaleChoices[] = {"Natural Minor", "Harmonic Minor", "Phrygian",
                                         "Phrygian Dominant"};
constexpr const char* kChordChoices[] = {"Minor Triad",   "Minor 7",       "Minor 9",
                                         "Minor 11",      "Sus2 Minor",    "Minor Add9",
                                         "Diminished 7",  "Power 5th"};

uint64_t hashCombine(uint64_t h, uint64_t v) noexcept {
  return h ^ (v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
}

} // namespace

GrimMIDIAudioProcessor::GrimMIDIAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {
  apvts_.addParameterListener(kParamRootKey, this);
  apvts_.addParameterListener(kParamScale, this);
  apvts_.addParameterListener(kParamChordType, this);
  apvts_.addParameterListener(kParamInversion, this);
  apvts_.addParameterListener(kParamSpread, this);
  apvts_.addParameterListener(kParamStrumMs, this);
  apvts_.addParameterListener(kParamVelHumanize, this);
  apvts_.addParameterListener(kParamOctave, this);
  setCurrentProgram(0);
}

GrimMIDIAudioProcessor::~GrimMIDIAudioProcessor() {
  apvts_.removeParameterListener(kParamRootKey, this);
  apvts_.removeParameterListener(kParamScale, this);
  apvts_.removeParameterListener(kParamChordType, this);
  apvts_.removeParameterListener(kParamInversion, this);
  apvts_.removeParameterListener(kParamSpread, this);
  apvts_.removeParameterListener(kParamStrumMs, this);
  apvts_.removeParameterListener(kParamVelHumanize, this);
  apvts_.removeParameterListener(kParamOctave, this);
}

juce::AudioProcessorValueTreeState::ParameterLayout GrimMIDIAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  juce::StringArray scaleNames;
  for (const auto* s : kScaleChoices) {
    scaleNames.add(s);
  }
  juce::StringArray chordNames;
  for (const auto* c : kChordChoices) {
    chordNames.add(c);
  }

  params.push_back(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID{kParamRootKey, 1}, "Key", 0, 11, 8,
      juce::AudioParameterIntAttributes().withStringFromValueFunction(
          [](int v, int) { return juce::MidiMessage::getMidiNoteName(v + 60, true, true, 4); })));

  params.push_back(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{kParamScale, 1}, "Scale", scaleNames, 0));

  params.push_back(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{kParamChordType, 1}, "Chord", chordNames, 1));

  params.push_back(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID{kParamInversion, 1}, "Inversion", 0, 3, 0));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{kParamSpread, 1}, "Spread",
      juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 45.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{kParamStrumMs, 1}, "Strum",
      juce::NormalisableRange<float>(0.0f, 120.0f, 0.1f), 22.0f,
      juce::AudioParameterFloatAttributes().withLabel("ms")));

  params.push_back(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID{kParamVelHumanize, 1}, "Vel Humanize", 0, 40, 12));

  params.push_back(std::make_unique<juce::AudioParameterInt>(
      juce::ParameterID{kParamOctave, 1}, "Octave", -2, 2, 0));

  return {params.begin(), params.end()};
}

void GrimMIDIAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  sampleRate_ = (sampleRate > 0.0) ? sampleRate : 48000.0;
  maxBlockSize_ = std::max(1, samplesPerBlock);
  scratchOut_.clear();
  scratchOut_.ensureSize(maxBlockSize_ * 64);
  pendingCount_ = 0;
  for (auto& h : heldByTrigger_) {
    h = {};
  }
  readRuntimeParams();
  paramFingerprint_ = computeParamFingerprint();
  setLatencySamples(0);
}

void GrimMIDIAudioProcessor::releaseResources() {
  pendingCount_ = 0;
  for (auto& h : heldByTrigger_) {
    h = {};
  }
}

bool GrimMIDIAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  const auto& in = layouts.getMainInputChannelSet();
  const auto& out = layouts.getMainOutputChannelSet();
  if (in != out) {
    return false;
  }
  return in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo()
      || in == juce::AudioChannelSet::disabled();
}

void GrimMIDIAudioProcessor::readRuntimeParams() noexcept {
  rootKeyPc_ = apvts_.getRawParameterValue(kParamRootKey)->load();
  scale_ = static_cast<GrimChordEngine::ScaleType>(
      static_cast<int>(std::lround(apvts_.getRawParameterValue(kParamScale)->load())));
  chordQuality_ = static_cast<GrimChordEngine::ChordQuality>(
      static_cast<int>(std::lround(apvts_.getRawParameterValue(kParamChordType)->load())));
  inversion_ = static_cast<int>(std::lround(apvts_.getRawParameterValue(kParamInversion)->load()));
  spread_ = static_cast<int>(std::lround(apvts_.getRawParameterValue(kParamSpread)->load()));
  const float strumMs = apvts_.getRawParameterValue(kParamStrumMs)->load();
  strumSamples_ = (strumMs * 0.001) * sampleRate_;
  velHumanize_ = static_cast<int>(std::lround(apvts_.getRawParameterValue(kParamVelHumanize)->load()));
  octaveShift_ = static_cast<int>(std::lround(apvts_.getRawParameterValue(kParamOctave)->load()));
}

uint64_t GrimMIDIAudioProcessor::computeParamFingerprint() const noexcept {
  uint64_t h = 0;
  h = hashCombine(h, static_cast<uint64_t>(rootKeyPc_));
  h = hashCombine(h, static_cast<uint64_t>(static_cast<int>(scale_)));
  h = hashCombine(h, static_cast<uint64_t>(static_cast<int>(chordQuality_)));
  h = hashCombine(h, static_cast<uint64_t>(inversion_));
  h = hashCombine(h, static_cast<uint64_t>(spread_));
  h = hashCombine(h, static_cast<uint64_t>(static_cast<int>(strumSamples_)));
  h = hashCombine(h, static_cast<uint64_t>(velHumanize_));
  h = hashCombine(h, static_cast<uint64_t>(octaveShift_));
  h = hashCombine(h, static_cast<uint64_t>(currentProgram_));
  return h;
}

void GrimMIDIAudioProcessor::parameterChanged(const juce::String&, float) {
  releaseAllRequested_.store(true, std::memory_order_release);
}

void GrimMIDIAudioProcessor::requestReleaseAllVoices() noexcept {
  releaseAllRequested_.store(true, std::memory_order_release);
}

int GrimMIDIAudioProcessor::mapTriggerToRoot(int triggerNote) const noexcept {
  return GrimChordEngine::rootMidiFromTrigger(triggerNote, rootKeyPc_, scale_, octaveShift_);
}

GrimChordEngine::VoicingRequest GrimMIDIAudioProcessor::makeVoicingRequest(int triggerNote,
                                                                            int velocity) const noexcept {
  GrimChordEngine::VoicingRequest req;
  req.rootMidi = mapTriggerToRoot(triggerNote);
  req.scale = scale_;
  req.quality = chordQuality_;
  req.inversion = inversion_;
  req.spreadPct = spread_;
  req.octaveShift = 0;
  req.velocity = velocity;
  req.velocityHumanize = velHumanize_;
  req.addPhrygianColor = true;
  return req;
}

void GrimMIDIAudioProcessor::releaseAllVoices(juce::MidiBuffer& out, int sampleOffset) noexcept {
  for (auto& hold : heldByTrigger_) {
    if (!hold.active) {
      continue;
    }
    for (int i = 0; i < hold.numNotes; ++i) {
      out.addEvent(juce::MidiMessage::noteOff(hold.channel, hold.notes[static_cast<size_t>(i)]), sampleOffset);
    }
    hold = {};
  }
}

void GrimMIDIAudioProcessor::handleAllNotesOff(juce::MidiBuffer& out, int midiChannel1to16,
                                               int sampleOffset) noexcept {
  for (auto& hold : heldByTrigger_) {
    if (!hold.active) {
      continue;
    }
    if (midiChannel1to16 < 1 || hold.channel == static_cast<uint8_t>(midiChannel1to16)) {
      for (int i = 0; i < hold.numNotes; ++i) {
        out.addEvent(
            juce::MidiMessage::noteOff(hold.channel, hold.notes[static_cast<size_t>(i)]), sampleOffset);
      }
      hold = {};
    }
  }
  out.addEvent(
      juce::MidiMessage::controllerEvent(midiChannel1to16 < 1 ? 1 : midiChannel1to16, 123, 0),
      sampleOffset);
}

void GrimMIDIAudioProcessor::scheduleStrummedChord(const GrimChordEngine::VoicingResult& chord,
                                                 uint8_t channel, int startSample,
                                                 juce::MidiBuffer& out) noexcept {
  if (chord.numNotes <= 0) {
    return;
  }

  const double step = (chord.numNotes > 1 && strumSamples_ > 0.0)
                          ? strumSamples_ / static_cast<double>(chord.numNotes - 1)
                          : 0.0;

  for (int i = 0; i < chord.numNotes; ++i) {
    const int offset = startSample + static_cast<int>(std::lround(step * static_cast<double>(i)));
    const auto msg =
        juce::MidiMessage::noteOn(channel, chord.notes[static_cast<size_t>(i)],
                                  static_cast<juce::uint8>(chord.velocities[static_cast<size_t>(i)]));
    out.addEvent(msg, std::max(0, offset));
  }
}

void GrimMIDIAudioProcessor::appendPendingToBuffer(juce::MidiBuffer& out, int /*numSamples*/) noexcept {
  for (int i = 0; i < pendingCount_; ++i) {
    const auto& p = pending_[static_cast<size_t>(i)];
    if (p.isNoteOn) {
      out.addEvent(juce::MidiMessage::noteOn(p.channel, p.note, p.velocity), p.sampleOffset);
    } else {
      out.addEvent(juce::MidiMessage::noteOff(p.channel, p.note), p.sampleOffset);
    }
  }
  pendingCount_ = 0;
}

void GrimMIDIAudioProcessor::copyPassThroughAudio(juce::AudioBuffer<float>& buffer) noexcept {
  const int numSamples = buffer.getNumSamples();
  const int inCh = getTotalNumInputChannels();
  const int outCh = getTotalNumOutputChannels();
  if (inCh <= 0 || outCh <= 0) {
    buffer.clear();
    return;
  }
  const int copyCh = std::min(inCh, outCh);
  for (int ch = 0; ch < copyCh; ++ch) {
    if (ch != 0 && inCh == 1) {
      buffer.copyFrom(ch, 0, buffer, 0, 0, numSamples);
    }
  }
  for (int ch = copyCh; ch < outCh; ++ch) {
    buffer.clear(ch, 0, numSamples);
  }
}

void GrimMIDIAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals noDenormals;

  readRuntimeParams();
  const uint64_t fp = computeParamFingerprint();
  scratchOut_.clear();

  if (releaseAllRequested_.exchange(false, std::memory_order_acq_rel) || fp != paramFingerprint_) {
    releaseAllVoices(scratchOut_, 0);
    paramFingerprint_ = fp;
  }

  if (auto* head = getPlayHead()) {
    if (auto pos = head->getPosition()) {
      if (!pos->getIsPlaying()) {
        releaseAllVoices(scratchOut_, 0);
      }
    }
  }

  constexpr int kMetaAllNotesOff = 123;

  juce::MidiBuffer inputCopy;
  inputCopy.swapWith(midi);
  inputCopy.ensureSize(maxBlockSize_ * 32);

  const int numSamples = buffer.getNumSamples();

  for (const auto metadata : inputCopy) {
    const auto& msg = metadata.getMessage();
    const int sample = metadata.samplePosition;

    if (msg.isAllNotesOff() || msg.isAllSoundOff()
        || (msg.isController() && msg.getControllerNumber() == kMetaAllNotesOff)) {
      handleAllNotesOff(scratchOut_, msg.getChannel(), sample);
      continue;
    }

    if (!msg.isNoteOnOrOff()) {
      scratchOut_.addEvent(msg, sample);
      continue;
    }

    const int note = msg.getNoteNumber();
    const int ch = msg.getChannel();
    const int slot = note % kMaxHeld;

    if (msg.isNoteOn() && msg.getVelocity() > 0) {
      auto& hold = heldByTrigger_[static_cast<size_t>(slot)];
      if (hold.active) {
        for (int i = 0; i < hold.numNotes; ++i) {
          scratchOut_.addEvent(
              juce::MidiMessage::noteOff(hold.channel, hold.notes[static_cast<size_t>(i)]), sample);
        }
        hold = {};
      }

      GrimChordEngine::VoicingResult chord;
      GrimChordEngine::buildVoicing(makeVoicingRequest(note, msg.getVelocity()), chord);

      hold.active = true;
      hold.channel = static_cast<uint8_t>(ch);
      hold.numNotes = static_cast<uint8_t>(std::min(chord.numNotes, GrimChordEngine::kMaxChordTones));
      for (int i = 0; i < hold.numNotes; ++i) {
        hold.notes[static_cast<size_t>(i)] = static_cast<uint8_t>(chord.notes[static_cast<size_t>(i)]);
      }

      scheduleStrummedChord(chord, static_cast<uint8_t>(ch), sample, scratchOut_);
    } else {
      auto& hold = heldByTrigger_[static_cast<size_t>(slot)];
      if (hold.active) {
        for (int i = 0; i < hold.numNotes; ++i) {
          scratchOut_.addEvent(
              juce::MidiMessage::noteOff(hold.channel, hold.notes[static_cast<size_t>(i)]), sample);
        }
        hold = {};
      }
    }
  }

  appendPendingToBuffer(scratchOut_, numSamples);
  midi.swapWith(scratchOut_);
  copyPassThroughAudio(buffer);
}

bool GrimMIDIAudioProcessor::buildProgressionMidiFile(juce::MidiFile& file) const {
  if (currentProgram_ < 0 || currentProgram_ >= kProgressionPresetCount) {
    return false;
  }
  const auto& preset = kProgressionPresets[static_cast<size_t>(currentProgram_)];

  file.setTicksPerQuarterNote(480);
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

    const int root = GrimChordEngine::degreeToRootMidi(rootKeyPc_, scale_, step.degree, octaveShift_);

    GrimChordEngine::VoicingRequest req;
    req.rootMidi = root;
    req.scale = scale_;
    req.quality = chordQuality_;
    req.inversion = inversion_;
    req.spreadPct = spread_;
    req.octaveShift = 0;
    req.velocity = 100;
    req.velocityHumanize = 0;
    req.addPhrygianColor = true;

    GrimChordEngine::VoicingResult chord;
    GrimChordEngine::buildVoicing(req, chord);

    const double stepDur = secondsPerBeat * static_cast<double>(step.beats);
    const double strumStep =
        (chord.numNotes > 1) ? (strumSamples_ / sampleRate_) / static_cast<double>(chord.numNotes - 1) : 0.0;

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

  file.addTrack(seq);
  return true;
}

void GrimMIDIAudioProcessor::setCurrentProgram(int index) {
  currentProgram_ = juce::jlimit(0, kProgressionPresetCount - 1, index);
  releaseAllRequested_.store(true, std::memory_order_release);
}

const juce::String GrimMIDIAudioProcessor::getProgramName(int index) {
  if (index < 0 || index >= kProgressionPresetCount) {
    return {};
  }
  return kProgressionPresets[static_cast<size_t>(index)].name;
}

void GrimMIDIAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  auto state = apvts_.copyState();
  state.setProperty("currentProgram", currentProgram_, nullptr);
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void GrimMIDIAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml == nullptr || !xml->hasTagName(apvts_.state.getType())) {
    return;
  }
  auto vt = juce::ValueTree::fromXml(*xml);
  apvts_.replaceState(vt);
  currentProgram_ = vt.getProperty("currentProgram", 0);
  releaseAllRequested_.store(true, std::memory_order_release);
}

juce::AudioProcessorEditor* GrimMIDIAudioProcessor::createEditor() {
  return new GrimMIDIAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new GrimMIDIAudioProcessor();
}
