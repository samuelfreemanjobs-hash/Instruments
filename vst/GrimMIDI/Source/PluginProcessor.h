#pragma once

#include <array>
#include <atomic>
#include <cstdint>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include "GrimChordEngine.h"
#include "ProgressionPresets.h"

class GrimMIDIAudioProcessor : public juce::AudioProcessor,
                               private juce::AudioProcessorValueTreeState::Listener {
public:
  GrimMIDIAudioProcessor();
  ~GrimMIDIAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return true; }
  bool isMidiEffect() const override { return true; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return kProgressionPresetCount; }
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int, const juce::String&) override {}

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  bool buildProgressionMidiFile(juce::MidiFile& file) const;

  static constexpr const char* kParamRootKey = "root_key";
  static constexpr const char* kParamScale = "scale";
  static constexpr const char* kParamChordType = "chord_type";
  static constexpr const char* kParamInversion = "inversion";
  static constexpr const char* kParamSpread = "spread";
  static constexpr const char* kParamStrumMs = "strum_ms";
  static constexpr const char* kParamVelHumanize = "velocity_humanize";
  static constexpr const char* kParamOctave = "octave";

  void requestReleaseAllVoices() noexcept;

private:
  struct HeldChord {
    bool active = false;
    uint8_t channel = 0;
    uint8_t numNotes = 0;
    std::array<uint8_t, GrimChordEngine::kMaxChordTones> notes {};
  };

  struct PendingMidi {
    int sampleOffset = 0;
    uint8_t channel = 0;
    uint8_t note = 0;
    uint8_t velocity = 0;
    bool isNoteOn = true;
  };

  static constexpr int kMaxPending = 512;
  static constexpr int kMaxHeld = 128;

  void parameterChanged(const juce::String& parameterID, float newValue) override;
  void readRuntimeParams() noexcept;
  void releaseAllVoices(juce::MidiBuffer& out, int sampleOffset) noexcept;
  void handleAllNotesOff(juce::MidiBuffer& out, int channel, int sampleOffset) noexcept;
  GrimChordEngine::VoicingRequest makeVoicingRequest(int triggerNote, int velocity) const noexcept;
  int mapTriggerToRoot(int triggerNote) const noexcept;
  void scheduleStrummedChord(const GrimChordEngine::VoicingResult& chord, uint8_t channel,
                             int startSample, juce::MidiBuffer& out) noexcept;
  void appendPendingToBuffer(juce::MidiBuffer& out, int numSamples) noexcept;
  void copyPassThroughAudio(juce::AudioBuffer<float>& buffer) noexcept;

  juce::AudioProcessorValueTreeState apvts_;
  int currentProgram_ = 0;

  double sampleRate_ = 48000.0;
  int maxBlockSize_ = 512;

  std::array<HeldChord, kMaxHeld> heldByTrigger_ {};
  std::array<PendingMidi, kMaxPending> pending_ {};
  int pendingCount_ = 0;

  juce::MidiBuffer scratchOut_;

  std::atomic<bool> releaseAllRequested_{false};

  int rootKeyPc_ = 0;
  GrimChordEngine::ScaleType scale_ = GrimChordEngine::ScaleType::NaturalMinor;
  GrimChordEngine::ChordQuality chordQuality_ = GrimChordEngine::ChordQuality::Minor7;
  int inversion_ = 0;
  int spread_ = 45;
  double strumSamples_ = 0.0;
  int velHumanize_ = 12;
  int octaveShift_ = 0;

  uint64_t paramFingerprint_ = 0;
  uint64_t computeParamFingerprint() const noexcept;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrimMIDIAudioProcessor)
};
