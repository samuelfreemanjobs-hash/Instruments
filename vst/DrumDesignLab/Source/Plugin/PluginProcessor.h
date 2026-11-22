#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <memory>
#include "../DSP/MultiGenreDrumEngine.h"
#include "../Factory/KitFactory.h"
#include "../Factory/SoundEditBridge.h"
#include "../Sequencer/LoopSequencer.h"
#include "../Sequencer/RhythmicOddity.h"

class DrumDesignLabAudioProcessor : public juce::AudioProcessor
{
public:
    enum class WorkspaceMode : int { Synth = 0, Design, Dna, Groove, Factory, Count };

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

    void selectPad(int padIndex, bool preview = true);
    int getSelectedPad() const noexcept { return selectedPad_; }

    void applySoundEditsFromApvts();
    void queueUiNote(int midiNote, float velocity01) noexcept;

    bool consumeKnobsNeedPull() noexcept
    {
        return knobsNeedPull_.exchange(false, std::memory_order_acq_rel);
    }
    void pullKnobsFromEngine();

    bool engineUsesThreeLayerPads() const noexcept
    {
        return engine_ != nullptr && engine_->usesThreeLayerPads();
    }

    void loadDnaFormula(int formulaIndex);
    int getDnaFormula() const noexcept { return dnaFormula_; }

    void mutateSelectedPad();
    void applyHyperDnaFromApvts();
    juce::String exportKitPack(const juce::File& folder, bool allFormulas, int variations);

    WorkspaceMode getWorkspaceMode() const;
    juce::String getStatusLine() const;
    int getGrooveStep() const noexcept { return grooveStep_; }
    bool isSequencerPlaying() const noexcept
    {
        return apvts.getRawParameterValue("ddl_seq_enabled")->load() > 0.5f;
    }
    void setSequencerPlaying(bool on);
    void generateLoop();
    const disklordz::loop_seq::LoopPattern& loopPattern() const noexcept { return loop_; }

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    disklordz::sound_edit::Values readEditValuesFromApvts() const;
    void writeEditValuesToApvts(const disklordz::sound_edit::Values& v);
    void syncFromApvts();
    void tickGroove(int numSamples);

    std::unique_ptr<disklordz::MultiGenreDrumEngine> engine_;
    disklordz::KitStyle currentStyle_ = disklordz::KitStyle::HitmakerEarthquake;
    int selectedPad_ = 0;
    int dnaFormula_ = 8;
    bool suppressEditPush_ = false;
    disklordz::sound_edit::Values lastApplied_ {};
    bool hasLastApplied_ = false;

    disklordz::bang::OddityPattern groove_ {};
    disklordz::loop_seq::LoopPattern loop_ {};
    bool useGeneratedLoop_ = false;
    double sampleRate_ = 48000.0;
    double samplesPerStep_ = 1.0;
    double groovePhase_ = 0.0;
    int grooveStep_ = 0;
    bool grooveWasOn_ = false;

    std::atomic<int> uiNote_ { -1 };
    std::atomic<float> uiVel_ { 0.f };
    std::atomic<bool> knobsNeedPull_ { false };
    juce::String lastExportStatus_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumDesignLabAudioProcessor)
};
