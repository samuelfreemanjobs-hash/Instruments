#pragma once

#include "SynthParameterIds.h"
#include <JuceHeader.h>

class MyFirstSynthAudioProcessor : public juce::AudioProcessor
{
public:
    MyFirstSynthAudioProcessor();
    ~MyFirstSynthAudioProcessor() override;

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

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void refreshEnvelopeParameters() noexcept;
    const juce::ADSR::Parameters& getEnvelopeParameters() const noexcept { return envelopeParams; }

    class SynthSound : public juce::SynthesiserSound
    {
    public:
        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };

    class SynthVoice : public juce::SynthesiserVoice
    {
    public:
        explicit SynthVoice(MyFirstSynthAudioProcessor& ownerProcessor);

        bool canPlaySound(juce::SynthesiserSound* sound) override;
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}
        void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    private:
        MyFirstSynthAudioProcessor& owner;
        juce::ADSR adsr;
        double currentAngle = 0.0;
        double angleDelta = 0.0;
        float currentLevel = 0.0f;
    };

    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synthesiser;

    std::atomic<float>* outputGainDbParam = nullptr;
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* decayParam = nullptr;
    std::atomic<float>* sustainParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    juce::ADSR::Parameters envelopeParams;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyFirstSynthAudioProcessor)
};
