#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Assets/CleanroomWaveLibrary.h"
#include "Assets/RomLoader.h"
#include "DSP/Effects/GroupADistortion.h"
#include "DSP/Effects/GroupBSpatial.h"
#include "DSP/VoicePool.h"

#include <array>

class JDUpgradedAudioProcessor final : public juce::AudioProcessor
{
public:
    JDUpgradedAudioProcessor();
    ~JDUpgradedAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts_; }
    const jdupgraded::assets::RomBank& getRomBank() const noexcept { return romLoader_.getBank(); }

private:
    void refreshCachedParameters() noexcept;
    void applyPatchesFromParameters() noexcept;
    void applyFactoryPatch (int index);
    void handleMidi (const juce::MidiBuffer& midi) noexcept;
    void setApvtsFloat (const char* paramId, float value);
    void setApvtsInt (const char* paramId, int value);
    void setApvtsChoice (const char* paramId, int index);
    void setApvtsBool (const char* paramId, bool value);

    juce::AudioProcessorValueTreeState apvts_;
    jdupgraded::dsp::VoicePool voicePool_;
    jdupgraded::assets::RomLoader romLoader_;
    jdupgraded::assets::CleanroomWaveLibrary fallbackWaves_;
    jdupgraded::dsp::GroupADistortion groupA_;
    jdupgraded::dsp::GroupBSpatial groupB_;

    std::array<float, 8192> outputScratch_{};
    std::array<float, 4> toneCoarseSemis_{};
    std::array<float, 4> toneFilterCutoff_{ 1.0f, 1.0f, 1.0f, 1.0f };
    std::array<float, 4> toneFilterResonance_{ 0.35f, 0.35f, 0.35f, 0.35f };

    int currentProgram_ = 0;

    std::atomic<float>* masterGainPtr_ = nullptr;
    std::atomic<float>* tone1LevelPtr_ = nullptr;
    std::atomic<float>* tone2LevelPtr_ = nullptr;
    std::atomic<float>* tone3LevelPtr_ = nullptr;
    std::atomic<float>* tone4LevelPtr_ = nullptr;
    std::atomic<float>* tone1WavePtr_ = nullptr;
    std::atomic<float>* tone2WavePtr_ = nullptr;
    std::atomic<float>* tone3WavePtr_ = nullptr;
    std::atomic<float>* tone4WavePtr_ = nullptr;
    std::atomic<float>* tone1MultisamplePtr_ = nullptr;
    std::atomic<float>* tone2MultisamplePtr_ = nullptr;
    std::atomic<float>* tone3MultisamplePtr_ = nullptr;
    std::atomic<float>* tone4MultisamplePtr_ = nullptr;
    std::atomic<float>* filterResonancePtr_ = nullptr;
    std::atomic<float>* couplingModePtr_ = nullptr;
    std::atomic<float>* groupADrivePtr_ = nullptr;
    std::atomic<float>* groupBMixPtr_ = nullptr;
    std::atomic<float>* tone1MutePtr_ = nullptr;
    std::atomic<float>* tone2MutePtr_ = nullptr;
    std::atomic<float>* tone3MutePtr_ = nullptr;
    std::atomic<float>* tone4MutePtr_ = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JDUpgradedAudioProcessor)
};
