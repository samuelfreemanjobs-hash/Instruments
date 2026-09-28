#pragma once

#include "DiagTone.h"

#include <JuceHeader.h>

namespace junovax::dsp
{
struct RuntimeParams
{
    float masterGainDb = 0.0f;
    float filterCutoff = 0.7f;
    float filterRes = 0.2f;
    bool hpfEnabled = false;
    int chorusMode = 0;
    bool diagTestTone = false;
    float diagToneFreqHz = 440.0f;
};

class SynthEngine
{
public:
    void prepare (double sampleRate, int maxBlockSize) noexcept;
    void reset() noexcept;
    void panic() noexcept;

    void setParams (const RuntimeParams& p) noexcept { params_ = p; }

    void render (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) noexcept;

private:
    void handleMidi (const juce::MidiBuffer& midi) noexcept;
    float renderSample() noexcept;

    RuntimeParams params_{};
    DiagTone diagTone_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> masterGain_;
    double sampleRate_ = 48000.0;
    bool voiceActive_ = false;
    float voiceLevel_ = 0.0f;
};
} // namespace junovax::dsp
