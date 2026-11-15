#pragma once

#include "BbdChorus.h"
#include "DiagTone.h"
#include "ExpAdsr.h"
#include "MipBlepOscillators.h"
#include "VoiceAnalogDrift.h"
#include "ZdfMoogLadder.h"

#include <JuceHeader.h>
#include <array>

namespace junovax::dsp
{
struct RuntimeParams
{
    float masterGainDb = 0.0f;

    float ampAttack = 0.01f;
    float ampDecay = 0.2f;
    float ampSustain = 0.75f;
    float ampRelease = 0.4f;

    float filtAttack = 0.005f;
    float filtDecay = 0.35f;
    float filtSustain = 0.4f;
    float filtRelease = 0.5f;

    float filterCutoff = 0.65f;
    float filterRes = 0.15f;
    int hpfMode = 0;

    int chorusMode = 0;
    bool chorusHiss = true;
    int voiceMode = 0;

    int lfoWaveform = 0;
    float lfoRate = 4.2f;
    float lfoDelay = 0.15f;
    bool lfoBpmSync = false;
    float glideMs = 12.0f;

    int dcoOctave = 1;
    bool dcoPulseOn = true;
    bool dcoSawOn = true;
    bool dcoSubOn = true;
    float dcoLfoMod = 30.0f;
    float dcoPwm = 65.0f;
    bool pwmSourceLfo = true;
    float dcoSubLvl = 80.0f;
    int dcoSubOct = 0;
    float dcoNoise = 10.0f;

    float vcfEnv = 55.0f;
    float vcfLfo = 33.0f;
    float vcfKey = 85.0f;
    bool vcfEnvInv = false;
    float vcfDrive = 35.0f;

    bool vcaGateMode = false;
    bool envVintageCurve = true;

    float drift = 14.0f;
    float detune = 6.0f;
    float width = 100.0f;

    int oversampleMode = 1;

    bool arpEnabled = false;
    int arpDirection = 0;
    float arpRange = 2.0f;
    float arpRate = 0.25f;
    bool arpLatch = false;

    float hostBpm = 120.0f;

    bool diagTestTone = false;
    float diagToneFreqHz = 440.0f;

    bool measurementLegacyOsc = false;
    bool measurementBypassFilter = false;
    int measurementOsFactor = 0;
};

class SynthEngine
{
public:
    static constexpr int kMaxVoices = 6;

    void prepare (double sampleRate, int maxBlockSize) noexcept;
    void reset() noexcept;
    void panic() noexcept;

    void setParams (const RuntimeParams& p) noexcept;

    void render (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) noexcept;
    float readAndResetPeak() noexcept;
    std::uint8_t getActiveVoiceMask() const noexcept;

private:
    struct Voice
    {
        int note = -1;
        bool keyDown = false;
        float frequency = 440.0f;
        float phase = 0.0f;
        float subPhase = 0.0f;
        float detuneCents = 0.0f;
        float lastOscSample = 0.0f;
        float noiseLp = 0.0f;
        ExpAdsr ampEnv;
        ExpAdsr filtEnv;
        ZdfMoogLadder ladder1x;
        ZdfMoogLadder ladder2x;
        ZdfMoogLadder ladder4x;
        VoiceAnalogDrift analogDrift;
    };

    void updateEnvelopes() noexcept;
    void initParamSmoothers() noexcept;
    void handleMidi (const juce::MidiBuffer& midi) noexcept;
    int findFreeVoice() noexcept;
    int findVoiceForNote (int note) noexcept;
    void startVoice (Voice& v, int note, float velocity, bool retrigger) noexcept;
    void releaseVoice (Voice& v) noexcept;
    float cutoffHzForVoice (const Voice& v, float filtEnvLevel, float lfo, float smCutoff) const noexcept;
    float synthesizeOsc (const Voice& v, float phase, float subPhase, float blepInc, float subBlepInc, float pwm, float hz) const noexcept;
    float renderVoiceSample (Voice& v, float lfo, float smCutoff, float smRes, float smPwm) noexcept;
    float advanceLfo() noexcept;
    float polyBlep (float phase, float inc) const noexcept;
    float blepPulse (float phase, float inc, float pw) const noexcept;
    float lfoValueAtPhase (float phase) const noexcept;
    int maxPolyVoices() const noexcept;
    int filterOversampleFactor() const noexcept;

    RuntimeParams params_{};
    MipBlepOscillators mipOsc_;
    DiagTone diagTone_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> masterGain_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smFilterCutoff_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smFilterRes_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smPwm_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smSubLvl_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smNoise_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smVcfDrive_;
    std::array<Voice, kMaxVoices> voices_{};
    juce::Random rng_;
    float lfoRandHold_ = 0.0f;
    double sampleRate_ = 48000.0;
    float peakHold_ = 0.0f;
    float lfoPhase_ = 0.0f;
    float lfoDelaySamples_ = 0.0f;
    float monoGlideFreq_ = 440.0f;
    int monoActiveNote_ = -1;

    juce::dsp::IIR::Filter<float> hpfL_;
    juce::dsp::IIR::Filter<float> hpfR_;
    juce::dsp::ProcessSpec spec_{};

    BbdChorus chorus_;
};
} // namespace junovax::dsp