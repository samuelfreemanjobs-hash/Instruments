#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Assets/CleanroomWaveLibrary.h"
#include "Assets/RomLoader.h"
#include "DSP/Effects/GroupADistortion.h"
#include "DSP/Effects/GroupBSpatial.h"
#include "DSP/VoicePool.h"

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

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts_; }
    const jdupgraded::assets::RomBank& getRomBank() const noexcept { return romLoader_.getBank(); }

private:
    void refreshCachedParameters() noexcept;
    void applyPatchesFromParameters() noexcept;
    void handleMidi (const juce::MidiBuffer& midi) noexcept;
    const jdupgraded::dsp::PcmWaveform& resolveWaveform (std::size_t waveIndex) const noexcept;

    juce::AudioProcessorValueTreeState apvts_;
    jdupgraded::dsp::VoicePool voicePool_;
    jdupgraded::assets::RomLoader romLoader_;
    jdupgraded::assets::CleanroomWaveLibrary fallbackWaves_;
    jdupgraded::dsp::GroupADistortion groupA_;
    jdupgraded::dsp::GroupBSpatial groupB_;

    std::array<float, 8192> outputScratch_{};

    std::atomic<float>* masterGainPtr_ = nullptr;
    std::atomic<float>* tone1LevelPtr_ = nullptr;
    std::atomic<float>* tone2LevelPtr_ = nullptr;
    std::atomic<float>* tone3LevelPtr_ = nullptr;
    std::atomic<float>* tone4LevelPtr_ = nullptr;
    std::atomic<float>* tone1WavePtr_ = nullptr;
    std::atomic<float>* tone2WavePtr_ = nullptr;
    std::atomic<float>* tone3WavePtr_ = nullptr;
    std::atomic<float>* tone4WavePtr_ = nullptr;
    std::atomic<float>* filterResonancePtr_ = nullptr;
    std::atomic<float>* couplingModePtr_ = nullptr;
    std::atomic<float>* groupADrivePtr_ = nullptr;
    std::atomic<float>* groupBMixPtr_ = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JDUpgradedAudioProcessor)
};
