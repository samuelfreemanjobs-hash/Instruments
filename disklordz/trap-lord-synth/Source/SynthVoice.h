#pragma once
#include <JuceHeader.h>
#include <cmath>
#include <memory>
#include <vector>

struct TrapPatch
{
    float attack = .01f, decay = .3f, sustain = .7f, release = .4f;
    float cutoff = 7000.f, gain = .2f, drive = 1.f, detune = 7.f, mix = .0f;
    int engine = 0;
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
    void prepare (double sr)
    {
        sampleRate = sr;
        env.setSampleRate (sr);
        juce::dsp::ProcessSpec spec { sr, 512, 1 };
        filter.prepare (spec);
        filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        filter.setResonance (1.f / std::sqrt (2.f));
        reset();
    }

    void reset()
    {
        env.reset();
        phase = phase2 = phase3 = 0;
        samplePosition = 0;
        fastReleaseGain = 1.f;
        filter.reset();
    }

    void setPatch (const TrapPatch& p)
    {
        patch = p;
        juce::ADSR::Parameters e;
        e.attack = p.attack;
        e.decay = p.decay;
        e.sustain = p.sustain;
        e.release = p.release;
        env.setParameters (e);
        filter.setCutoffFrequency (juce::jlimit (20.f, float (sampleRate * .45f), patch.cutoff));
    }

    void setSamples (std::shared_ptr<const std::vector<TrapSamplePtr>> bank) { samples = std::move (bank); }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<TrapSound*> (sound) != nullptr;
    }

    void startNote (int note, float vel, juce::SynthesiserSound*, int wheel) override
    {
        midiNote = note;
        frequency = juce::MidiMessage::getMidiNoteInHertz (note);
        phase = phase2 = phase3 = 0;
        samplePosition = 0;
        fastReleaseGain = 1.f;
        activeSample.reset();

        if (samples != nullptr && patch.engine == 7)
        {
            const int v = juce::jlimit (0, 127, int (vel * 127.f));
            for (auto& item : *samples)
                if (item != nullptr && note >= item->low && note <= item->high
                    && v >= item->velLow && v <= item->velHigh)
                {
                    activeSample = item;
                    break;
                }
        }

        pitchWheelMoved (wheel);
        env.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (! allowTailOff)
        {
            juce::ADSR::Parameters quick;
            quick.attack = patch.attack;
            quick.decay = patch.decay;
            quick.sustain = patch.sustain;
            quick.release = juce::jmin (patch.release, 0.006f);
            env.setParameters (quick);
            fastReleaseGain = 1.f;
        }
        else
        {
            setPatch (patch);
        }

        env.noteOff();
    }

    void pitchWheelMoved (int v) override { bend = std::pow (2., (v - 8192.) / 8192. * 2. / 12.); }

    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& out, int start, int n) override
    {
        if (! isVoiceActive())
            return;

        const float safeCutoff = juce::jlimit (20.f, float (sampleRate * .45f), patch.cutoff);
        if (! std::isfinite (safeCutoff))
            return;

        filter.setCutoffFrequency (safeCutoff);
        const double step = juce::jlimit (1e-9, 0.5, frequency * bend / sampleRate);
        const double step2 = step * std::pow (2., patch.detune / 1200.);
        const double step3 = step * 0.5;

        for (int i = 0; i < n; ++i)
        {
            float x = 0.f;

            if (patch.engine == 7)
            {
                x = readSample();
            }
            else
            {
                const float a = float (phase);
                const float b = float (phase2);
                switch (patch.engine)
                {
                    case 0: x = 0.55f * blepSaw (phase, step) + 0.45f * blepSaw (phase2, step2); break;
                    case 1:
                        x = 0.6f * std::sin (tau * phase) + 0.22f * std::sin (tau * phase * 2.)
                          + 0.12f * std::sin (tau * phase * 3.) + 0.06f * std::sin (tau * phase * 4.);
                        break;
                    case 2:
                        x = 0.45f * blepSaw (phase, step) + 0.35f * blepSaw (phase2, step2)
                          + 0.2f * triangle (float (phase3));
                        break;
                    case 3: x = std::sin (tau * phase + 2.5f * std::sin (tau * phase * 2.71)); break;
                    case 4: x = 0.65f * std::sin (tau * phase) + 0.35f * triangle (a); break;
                    case 5:
                        x = 0.5f * std::sin (tau * phase) + 0.3f * std::sin (tau * phase * 2.02)
                          + 0.2f * std::sin (tau * phase * 3.97);
                        break;
                    case 6: x = 0.65f * blepSaw (phase, step) + 0.35f * triangle (b); break;
                    default: x = std::sin (tau * phase); break;
                }

                phase = wrap (phase + step);
                phase2 = wrap (phase2 + step2);
                phase3 = wrap (phase3 + step3);
            }

            const float filtered = filter.processSample (0, std::isfinite (x) ? x : 0.f);
            const float normDrive = std::max (.001f, std::tanh (juce::jmax (0.01f, patch.drive)));
            const float driven = std::tanh (filtered * patch.drive) / normDrive;
            float y = driven * env.getNextSample() * velocity * patch.gain;
            if (! std::isfinite (y))
                y = 0.f;

            if (fastReleaseGain < 1.f)
            {
                fastReleaseGain *= 0.992f;
                y *= fastReleaseGain;
            }

            for (int ch = 0; ch < out.getNumChannels(); ++ch)
                out.addSample (ch, start + i, y);
        }

        if (! env.isActive())
            clearCurrentNote();
    }

private:
    static constexpr double tau = juce::MathConstants<double>::twoPi;

    static double wrap (double p) { return p - std::floor (p); }

    static float triangle (float p) { return 1.f - 4.f * std::abs (p - .5f); }

    static float polyBlep (double phase, double dt)
    {
        if (dt <= 0.)
            return 0.f;

        if (phase < dt)
        {
            const double x = phase / dt;
            return float (x + x - x * x - 1.);
        }

        if (phase > 1. - dt)
        {
            const double x = (phase - 1.) / dt;
            return float (x * x + x + x + 1.);
        }

        return 0.f;
    }

    static float blepSaw (double phase, double dt)
    {
        const double naive = 2. * phase - 1.;
        return float (naive - (double) polyBlep (phase, dt) + (double) polyBlep (wrap (phase + dt), dt));
    }

    float readSample()
    {
        if (activeSample == nullptr || activeSample->audio.getNumSamples() < 2)
            return 0.f;

        const auto& b = activeSample->audio;
        const int ix = int (samplePosition);
        if (ix >= b.getNumSamples() - 1)
            return 0.f;

        const float frac = float (samplePosition - ix);
        const float x = juce::jmap (frac, b.getSample (0, ix), b.getSample (0, ix + 1));
        samplePosition += activeSample->sourceRate / sampleRate
                        * std::pow (2., (midiNote - activeSample->root) / 12.) * bend;
        return x;
    }

    juce::ADSR env;
    juce::dsp::StateVariableTPTFilter<float> filter;
    TrapPatch patch;
    TrapSamplePtr activeSample;
    std::shared_ptr<const std::vector<TrapSamplePtr>> samples;
    double sampleRate = 44100., frequency = 440., bend = 1., phase = 0., phase2 = 0., phase3 = 0.,
           samplePosition = 0.;
    float velocity = 0.f, fastReleaseGain = 1.f;
    int midiNote = 60;
};
