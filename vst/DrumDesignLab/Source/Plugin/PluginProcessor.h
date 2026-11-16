#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <memory>
#include "../DSP/MultiGenreDrumEngine.h"
#include "../Factory/SoundEditBridge.h"

class DrumDesignLabAudioProcessor : public juce::AudioProcessor
{
public:
    DrumDesignLabAudioProcessor();
    ~DrumDesignLabAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DiskLordz Drum Design Lab"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return static_cast<int>(disklordz::KitStyle::Count); }
    int getCurrentProgram() override { return static_cast<int>(currentStyle_); }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setKitStyle(disklordz::KitStyle style);
    disklordz::KitStyle getKitStyle() const noexcept { return currentStyle_; }

    /** Message-thread: select pad 0..15, sync knobs from engine, optionally preview. */
    void selectPad(int padIndex, bool preview = true);
    int getSelectedPad() const noexcept { return selectedPad_; }

    /** Message-thread: push current APVTS sound knobs into the active engine/pad. */
    void applySoundEditsFromApvts();

    /** Safe UI note preview (consumed on audio thread). */
    void queueUiNote(int midiNote, float velocity01) noexcept;

    /** Editor polls this after style/pad changes that happen on the audio thread. */
    bool consumeKnobsNeedPull() noexcept
    {
        return knobsNeedPull_.exchange(false, std::memory_order_acq_rel);
    }

    void pullKnobsFromEngine();

    bool engineUsesThreeLayerPads() const noexcept
    {
        return engine_ != nullptr && engine_->usesThreeLayerPads();
    }

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    disklordz::sound_edit::Values readEditValuesFromApvts() const;
    void writeEditValuesToApvts(const disklordz::sound_edit::Values& v);
    void syncStyleFromApvts();

    std::unique_ptr<disklordz::MultiGenreDrumEngine> engine_;
    disklordz::KitStyle currentStyle_ = disklordz::KitStyle::HitmakerEarthquake;
    int selectedPad_ = 0;
    bool suppressEditPush_ = false;
    disklordz::sound_edit::Values lastApplied_ {};
    bool hasLastApplied_ = false;

    std::atomic<int> uiNote_ { -1 };
    std::atomic<float> uiVel_ { 0.f };
    std::atomic<bool> knobsNeedPull_ { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumDesignLabAudioProcessor)
};
