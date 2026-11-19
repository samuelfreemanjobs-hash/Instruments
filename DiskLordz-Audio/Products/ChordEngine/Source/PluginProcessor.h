#pragma once

#include "../../../DPF-MIDI/MusicTheory/MusicTheory.h"
#include "PreviewSynth.h"
#include "ProgressionLibrary.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <array>
#include <vector>

class ChordEngineAudioProcessor final : public juce::AudioProcessor
{
public:
    static constexpr int kProgressionLength = 4;

    ChordEngineAudioProcessor();
    ~ChordEngineAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    bool supportsMPE() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getApvts() { return apvts_; }

    void triggerAudition();
    std::array<juce::String, kProgressionLength> getChordSymbolLabels() const;
    int getPlayheadStep() const { return activeStep_; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    using ScaleType = disklordz::theory::ScaleType;

    juce::AudioProcessorValueTreeState apvts_;
    disklordz::chordengine::PreviewSynth previewSynth_;

    int currentProgram_ = 0;
    int activeStep_ = -1;
    double sampleRate_ = 44100.0;

    bool auditionActive_ = false;
    double auditionPpq_ = 0.0;
    int lastTriggeredStep_ = -1;

    std::vector<int> heldNotes_;

    int getRootPc() const;
    ScaleType getScale() const;
    const disklordz::chordengine::StarterProgression& currentPreset() const;

    std::vector<int> notesForStep(int stepIndex) const;
    void startChord(int stepIndex, int sampleOffset, juce::MidiBuffer& midi);
    void stopHeldNotes(int sampleOffset, juce::MidiBuffer& midi);
    void advanceSequencer(double ppqPosition, int numSamples, double bpm, juce::MidiBuffer& midi);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordEngineAudioProcessor)
};