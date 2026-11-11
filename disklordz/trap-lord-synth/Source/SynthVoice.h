#pragma once
#include <JuceHeader.h>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

/** Realtime patch — macros and movement are resolved in buildPatch(). */
struct TrapPatch
{
    float attack = .01f, decay = .3f, sustain = .7f, release = .4f;
    float cutoff = 7000.f, gain = .2f, drive = 1.f, detune = 7.f;
    float resonance = 0.707f;
    float pitchEnvSemi = 0.f;
    float pitchEnvDecaySec = 0.045f;
    float pulseWidth = .5f;
    float portamentoSec = 0.f;
    float vibratoDepth = 0.f;
    float vibratoRate = 5.5f;
    float swellDepth = 0.f;
    float oscMorph = 0.5f;
    float noiseMix = 0.f;
    float ringModDepth = 0.f;
    float fmIndex = 1.f;
    float fmRatio = 2.f;
    float keyTrack = 1.f;
    int engine = 0;
    /** 0–2 SVF LP/HP/BP, 3 ladder LP, 4 comb, 5 formant. */
    int filterType = 0;
    int unisonVoices = 3;
};

struct TrapSample
{
    juce::AudioBuffer<float> audio;
    int root = 60, low = 0, high = 127, velLow = 0, velHigh = 127;
    double sourceRate = 44100.;
};
using TrapSamplePtr = std::shared_ptr<const TrapSample>;

class TrapSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

class TrapVoice final : public juce::SynthesiserVoice
{
public:
    void prepare (double sr);
    void reset();
    void setPatch (const TrapPatch& p);
    void setSamples (std::shared_ptr<const std::vector<TrapSamplePtr>> bank);
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int note, float vel, juce::SynthesiserSound*, int wheel) override;
    void stopNote (float, bool allowTailOff) override;
    void pitchWheelMoved (int v) override;
    void controllerMoved (int cc, int value) override;
    void renderNextBlock (juce::AudioBuffer<float>& out, int start, int n) override;

private:
    float synthesizeEngine (int engine, double basePhase, double step, float morph);
    float applyFilter (float x, float cutoffHz, float res);
    void seedPhases();
    static float polyBlep (double phase, double dt);
    static float blepSaw (double phase, double dt);
    static float blepPulse (double phase, double dt, float width);
    static double wrap (double p);
    static float triangle (float p);
    static float ladderStep (float in, float& z1, float& z2, float& z3, float& z4, float g);

    juce::ADSR env;
    juce::dsp::StateVariableTPTFilter<float> svf;
    int filterTypeApplied = 0;
    float ladderZ1 = 0.f, ladderZ2 = 0.f, ladderZ3 = 0.f, ladderZ4 = 0.f;
    float combBuf[4096]{};
    int combWrite = 0;
    std::array<juce::dsp::StateVariableTPTFilter<float>, 3> formants;
    TrapPatch patch;
    TrapSamplePtr activeSample;
    std::shared_ptr<const std::vector<TrapSamplePtr>> samples;
    double sampleRate = 44100., frequency = 440., targetFreq = 440., glideFreq = 440., bend = 1.;
    double phase = 0., phase2 = 0., phase3 = 0., phase4 = 0., subPhase = 0., modPhase = 0.;
    double carrierPhase = 0., syncPhase = 0.;
    double lfoPhase = 0.;
    double vibratoPhase = 0.;
    double noiseSeed = 12345.678;
    float velocity = 0.f, fastReleaseGain = 1.f, pitchEnv = 0.f, modWheel01 = 0.f;
    double pitchDecay = 0.;
    int midiNote = 60;
    float voiceDrift = 0.f;
    float voiceHpL = 0.f, voiceHpR = 0.f, voiceHpInL = 0.f, voiceHpInR = 0.f;
    static constexpr double tau = juce::MathConstants<double>::twoPi;
};
