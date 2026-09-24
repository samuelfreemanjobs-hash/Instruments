#pragma once

#include "Presets/FactoryPresets.h"

#include <array>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class ProphetRev2TrapAudioProcessor : public juce::AudioProcessor
{
public:
    ProphetRev2TrapAudioProcessor();
    ~ProphetRev2TrapAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getApvts() noexcept { return apvts_; }

    void applyFactoryPreset (int index);
    void applySynthParams (const prophetrev2::SynthParams& params);
    bool applyUserPreset (const juce::String& name);
    bool saveUserPreset (const juce::String& name);
    juce::String getActiveUserPresetName() const noexcept { return activeUserPresetName_; }

    struct RuntimeParams
    {
        float osc1Level = 0.85f;
        float osc2Level = 0.65f;
        float osc2DetuneRatio = 1.0f;
        float oscMix = 0.45f;
        float filterCutoffNorm = 0.55f;
        float filterRes = 0.22f;
        float filtEnvAmt = 0.5f;
        float keyTrack = 0.45f;
        float circuitDrive = 0.15f;
        float filterDrive = 0.2f;
        int unisonVoices = 1;
        float unisonSpreadCents = 12.0f;
        bool monoMode = false;
        bool legatoMode = true;
        float glideMs = 80.0f;
        float glideCoeff = 0.0f;
        float outputGain = 1.0f;
        juce::ADSR::Parameters ampEnv {};
        juce::ADSR::Parameters filtEnv {};
    };

    const RuntimeParams& getRuntimeParams() const noexcept { return runtime_; }

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void refreshRuntimeParams() noexcept;
    void processMidiMono (juce::MidiBuffer& midi);

    class SynthSound : public juce::SynthesiserSound
    {
    public:
        bool appliesToNote (int) override { return true; }
        bool appliesToChannel (int) override { return true; }
    };

    class SynthVoice : public juce::SynthesiserVoice
    {
    public:
        explicit SynthVoice (ProphetRev2TrapAudioProcessor& owner);

        bool canPlaySound (juce::SynthesiserSound* sound) override;
        void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
        void stopNote (float velocity, bool allowTailOff) override;
        void pitchWheelMoved (int) override {}
        void controllerMoved (int, int) override {}
        void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

        void prepareVoice (const juce::dsp::ProcessSpec& spec);
        void setOsc2DetuneRatio (float ratio) noexcept { osc2Ratio_ = ratio; }
        void glideToNote (int midiNote, float velocity, bool retriggerEnvelopes);

    private:
        float renderOscMix() noexcept;
        float computeCutoffHz (float baseNorm, float filtEnv) noexcept;

        ProphetRev2TrapAudioProcessor& owner_;
        juce::ADSR ampAdsr_;
        juce::ADSR filtAdsr_;
        juce::dsp::Oscillator<float> osc1_;
        juce::dsp::Oscillator<float> osc2_;
        juce::dsp::LadderFilter<float> ladder_;
        double osc2Ratio_ = 1.0;
        double currentFreq_ = 440.0;
        double targetFreq_ = 440.0;
        int midiNote_ = 60;
        float level_ = 0.0f;
        bool active_ = false;
    };

    juce::AudioProcessorValueTreeState apvts_;
    juce::Synthesiser synthesiser_;
    std::array<bool, 128> monoKeysDown_ {};
    int countMonoKeysDown() const noexcept;
    int highestMonoKeyDown() const noexcept;
    RuntimeParams runtime_;
    int currentProgram_ = 0;
    juce::String activeUserPresetName_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProphetRev2TrapAudioProcessor)
};
