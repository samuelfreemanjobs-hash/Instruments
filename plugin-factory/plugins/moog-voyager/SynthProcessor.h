#pragma once

#include "SynthParameterIds.h"
#include <JuceHeader.h>
#include <array>

/** Monophonic Voyager-style VSTi: triple oscillator, noise, Moog ladder LPF, dual ADSR. */
class MoogVoyagerAudioProcessor : public juce::AudioProcessor
{
public:
    MoogVoyagerAudioProcessor();
    ~MoogVoyagerAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getApvts() noexcept { return apvts; }

    struct VoiceParams
    {
        float oscLevel[3] {};
        float oscOctave[3] {};
        float oscFine[3] {};
        int oscWave[3] {};

        float noiseLevel = 0.0f;
        float filterCutoffHz = 8000.0f;
        float filterResonance = 0.35f;
        float filterDrive = 1.2f;
        float filterEnvAmount = 0.65f;
        float filterKeyTrack = 0.35f;

        juce::ADSR::Parameters filterEnv {};
        juce::ADSR::Parameters ampEnv {};

        float lfoRateHz = 0.5f;
        float lfoToPitch = 0.0f;
        float lfoToFilter = 0.0f;
        float glideSeconds = 0.0f;
    };

    VoiceParams readVoiceParams() const noexcept;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    class SynthSound : public juce::SynthesiserSound
    {
    public:
        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };

    class SynthVoice : public juce::SynthesiserVoice
    {
    public:
        explicit SynthVoice(MoogVoyagerAudioProcessor& ownerProcessor);

        void prepare(double sampleRate, int maxBlockSize);
        bool canPlaySound(juce::SynthesiserSound* sound) override;
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}
        void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    private:
        static float waveFromPhase(float phase01, int waveIndex) noexcept;
        static float noteToHz(int midiNote, float octaveOffset, float fineCents) noexcept;
        float nextNoiseSample() noexcept;

        MoogVoyagerAudioProcessor& owner;
        juce::dsp::LadderFilter<float> ladder;
        juce::dsp::ProcessSpec ladderSpec {};
        juce::ADSR filterAdsr;
        juce::ADSR ampAdsr;

        float phase[3] { 0.0f, 0.0f, 0.0f };
        float currentHz = 440.0f;
        float targetHz = 440.0f;
        float glideCoeff = 1.0f;
        float lfoPhase = 0.0f;
        float velocityGain = 1.0f;
        int currentMidiNote = 60;
        uint32_t rngState = 0xC0FFEEu;
        bool prepared = false;
    };

    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synthesiser;

    juce::LinearSmoothedValue<float> outputGainLinear;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MoogVoyagerAudioProcessor)
};
