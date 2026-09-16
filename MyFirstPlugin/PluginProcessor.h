#pragma once

#include "ParameterIds.h"
#include <JuceHeader.h>
#include <array>

//==============================================================================
/** Stereo effect: state-variable lowpass, tube-style saturation, LFO -> cutoff.
    Processor-only phase: use GenericAudioProcessorEditor or a custom editor later.
*/
class MyFirstPluginAudioProcessor : public juce::AudioProcessor
{
public:
    MyFirstPluginAudioProcessor();
    ~MyFirstPluginAudioProcessor() override;

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

    void updateSmootherTargets() noexcept;

    static float tubeSaturation(float sample, float driveAmount) noexcept;

    juce::AudioProcessorValueTreeState apvts;

    // Cached atomic parameter pointers — never look up by string on the audio thread.
    std::atomic<float>* cutoffHzParam = nullptr;
    std::atomic<float>* resonanceParam = nullptr;
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* lfoRateHzParam = nullptr;
    std::atomic<float>* lfoDepthParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* outputGainDbParam = nullptr;

    juce::LinearSmoothedValue<float> cutoffSmoothed;
    juce::LinearSmoothedValue<float> resonanceSmoothed;
    juce::LinearSmoothedValue<float> driveSmoothed;
    juce::LinearSmoothedValue<float> lfoRateSmoothed;
    juce::LinearSmoothedValue<float> lfoDepthSmoothed;
    juce::LinearSmoothedValue<float> mixSmoothed;
    juce::LinearSmoothedValue<float> outputGainLinearSmoothed;

    struct ChannelDSP
    {
        juce::dsp::StateVariableTPTFilter<float> svf;
        float lfoPhaseRadians = 0.0f;
    };

    static constexpr int maxChannels = 2;
    std::array<ChannelDSP, maxChannels> channelDSP;

    juce::dsp::ProcessSpec processSpec {};
    double currentSampleRate = 44100.0;
    float lfoPhaseIncrementPerSample = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyFirstPluginAudioProcessor)
};
