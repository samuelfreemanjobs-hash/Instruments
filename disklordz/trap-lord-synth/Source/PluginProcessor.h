#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"
#include "KeygroupExportOptions.h"
#include <array>
#include <atomic>
#include <functional>

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
    /** kind 0 = init saw, 1 = sine-heavy, 2 = middle-C check (same as 0). */
    void loadDiagnosticPatch (int kind);
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    void setCurrentPresetIndex (int i) { currentPresetIndex = i; }
    bool exportMpcKeygroupOffline (const juce::File& parentFolder,
                                   const KeygroupExportOptions& options,
                                   std::atomic<bool>& abortFlag,
                                   const std::function<void (float, const juce::String&)>& onProgress);
    bool renderOfflineTimeline (int presetIndex,
                                const std::vector<std::pair<int, juce::MidiMessage>>& timeline,
                                double sampleRate,
                                double durationSeconds,
                                double bpm,
                                juce::AudioBuffer<float>& buffer);
    bool renderNoteOffline (int midiNote,
                            int velocity,
                            juce::AudioBuffer<float>& buffer,
                            double sampleRate,
                            float noteHoldSeconds = -1.f);
    juce::AudioProcessorValueTreeState state;
    static const juce::StringArray& factoryNames();
    static int factoryPresetCount();
    static int presetBank (int index);
    static juce::String presetCategory (int index);

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    TrapPatch buildPatch() const;
    void applyRotary (juce::AudioBuffer<float>& buffer, float depth);
    juce::Synthesiser synth;
    std::shared_ptr<const std::vector<TrapSamplePtr>> bank;
    juce::dsp::Reverb reverb;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay { 96000 };
    float hpStateL = 0.f, hpStateR = 0.f, hpPrevInL = 0.f, hpPrevInR = 0.f;
    double currentRate = 44100., arpPhase = 0., rotaryPhase = 0.;
    int arpStep = 0;
    std::array<bool, 128> held {};
    double hostBpm = 120.;
    void generateArp (juce::MidiBuffer&, int);
    double arpPeriodSeconds() const;
    void prepareOfflineRender (double sampleRate, bool preservePatchParams = false);
    void finishOfflineRender();
    int currentPresetIndex = 0;
    double offlineSavedRate = 44100.;
    bool offlineActive = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrapLordSynthAudioProcessor)
};
