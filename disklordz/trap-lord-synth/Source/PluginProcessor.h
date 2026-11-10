#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"
#include <array>

class TrapLordSynthAudioProcessor final : public juce::AudioProcessor
{
public:
    TrapLordSynthAudioProcessor();
    void prepareToPlay (double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 6.; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return "User Patch"; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;
    bool loadSampleMap (const juce::File&);
    bool exportKeygroup (const juce::File& folder, int root = 60);
    void loadFactoryPreset (int index);
    juce::AudioProcessorValueTreeState state;
    static const juce::StringArray& factoryNames();

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    juce::Synthesiser synth;
    std::shared_ptr<const std::vector<TrapSamplePtr>> bank;
    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay { 96000 };
    double currentRate = 44100., arpPhase = 0.;
    int arpStep = 0;
    std::array<bool, 128> held {};
    double hostBpm = 120.;
    void generateArp (juce::MidiBuffer&, int);
    double arpPeriodSeconds() const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrapLordSynthAudioProcessor)
};
