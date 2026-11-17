#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <atomic>

class OvernightDrumAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int kMaxVoices = 16;
    static constexpr int kPads = 8;

    OvernightDrumAudioProcessor();
    ~OvernightDrumAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DiskLordz Overnight Drum"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void loadKitFromDirectory(const juce::File& dir);
    juce::File getKitDirectory() const { return kitDir_; }
    juce::MidiKeyboardState& getKeyboardState() noexcept { return keyboardState_; }
    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts_; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    struct Voice
    {
        bool active = false;
        int pad = 0;
        double pos = 0.0;
        float gain = 1.0f;
        double ratio = 1.0;
    };

    void noteOn(int midiNote, float velocity);
    int midiToPad(int note) const noexcept;

    juce::AudioProcessorValueTreeState apvts_;
    juce::MidiKeyboardState keyboardState_;
    std::array<juce::AudioBuffer<float>, kPads> pads_ {};
    std::array<Voice, kMaxVoices> voices_ {};
    juce::File kitDir_;
    double sampleRate_ = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OvernightDrumAudioProcessor)
};
