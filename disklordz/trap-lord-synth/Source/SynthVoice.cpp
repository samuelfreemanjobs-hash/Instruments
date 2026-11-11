#include "SynthVoice.h"

namespace
{
float hashNoise (double& seed)
{
    seed = seed * 1103515245. + 12345.;
    return float (std::fmod (std::abs (std::sin (seed)), 1.)) * 2.f - 1.f;
}

float wavetableMorph (double phase, float morph)
{
    const float m = juce::jlimit (0.f, 1.f, morph);
    const float sine = std::sin (float (juce::MathConstants<double>::twoPi * phase));
    const float saw = float (2. * phase - 1.);
    const float tri = 1.f - 4.f * std::abs (float (phase) - 0.5f);
    const float sq = phase < 0.5 ? 1.f : -1.f;
    const float a = juce::jmap (m, 0.f, 0.33f, sine, saw);
    const float b = juce::jmap (m, 0.33f, 0.66f, saw, tri);
    const float c = juce::jmap (m, 0.66f, 1.f, tri, sq);
    if (m < 0.33f)
        return a;
    if (m < 0.66f)
        return b;
    return c;
}
} // namespace

void TrapVoice::prepare (double sr)
{
    sampleRate = sr;
    env.setSampleRate (sr);
    juce::dsp::ProcessSpec spec { sr, 512, 1 };
    svf.prepare (spec);
    svf.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    filterTypeApplied = 0;
    for (auto& f : formants)
    {
        f.prepare (spec);
        f.setType (juce::dsp::StateVariableTPTFilterType::bandpass);
    }
    combWrite = 0;
    std::fill (std::begin (combBuf), std::end (combBuf), 0.f);
    reset();
}

void TrapVoice::reset()
{
    env.reset();
    phase = phase2 = phase3 = phase4 = subPhase = modPhase = 0.;
    carrierPhase = syncPhase = 0.;
    lfoPhase = 0.;
    vibratoPhase = 0.;
    fastReleaseGain = 1.f;
    pitchEnv = 0.f;
    modWheel01 = 0.f;
    svf.reset();
    ladderZ1 = ladderZ2 = ladderZ3 = ladderZ4 = 0.f;
    for (auto& f : formants)
        f.reset();
}

void TrapVoice::seedPhases()
{
    phase2 = wrap (phase + 0.37);
    phase3 = wrap (phase + 0.71);
    phase4 = wrap (phase + 0.11);
    carrierPhase = phase;
    syncPhase = 0.;
    subPhase = 0.;
    modPhase = 0.;
    voiceDrift = float ((midiNote % 7) * 0.0013 - 0.003);
}

void TrapVoice::setPatch (const TrapPatch& p)
{
    patch = p;
    juce::ADSR::Parameters e;
    e.attack = p.attack;
    e.decay = p.decay;
    e.sustain = p.sustain;
    e.release = p.release;
    env.setParameters (e);
    const int ft = juce::jlimit (0, 5, p.filterType);
    if (ft != filterTypeApplied)
    {
        filterTypeApplied = ft;
        if (ft < 3)
        {
            const auto t = ft == 1 ? juce::dsp::StateVariableTPTFilterType::highpass
                       : ft == 2 ? juce::dsp::StateVariableTPTFilterType::bandpass
                                 : juce::dsp::StateVariableTPTFilterType::lowpass;
            svf.setType (t);
        }
    }
    pitchDecay = patch.pitchEnvDecaySec > 0.001f ? std::exp (-1. / (patch.pitchEnvDecaySec * sampleRate)) : 0.;
}

void TrapVoice::setSamples (std::shared_ptr<const std::vector<TrapSamplePtr>> bank)
{
    samples = std::move (bank);
}

bool TrapVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<TrapSound*> (sound) != nullptr;
}

void TrapVoice::startNote (int note, float vel, juce::SynthesiserSound*, int wheel)
{
    velocity = juce::jlimit (0.f, 1.f, vel);
    const double newFreq = juce::MidiMessage::getMidiNoteInHertz (note);
    midiNote = note;
    frequency = newFreq;
    targetFreq = newFreq;
    glideFreq = newFreq;
    seedPhases();
    fastReleaseGain = 1.f;
    pitchEnv = patch.pitchEnvSemi > 0.01f ? 1.f : 0.f;
    activeSample.reset();
    noiseSeed = note * 13.17 + vel * 991.0;
    pitchWheelMoved (wheel);
    env.noteOn();
}

void TrapVoice::stopNote (float, bool allowTailOff)
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
        setPatch (patch);
    env.noteOff();
}

void TrapVoice::pitchWheelMoved (int v)
{
    bend = std::pow (2., (v - 8192.) / 8192. * 2. / 12.);
}

void TrapVoice::controllerMoved (int cc, int value)
{
    if (cc == 1)
        modWheel01 = juce::jlimit (0.f, 1.f, value / 127.f);
}

double TrapVoice::wrap (double p)
{
    return p - std::floor (p);
}

float TrapVoice::triangle (float p)
{
    return 1.f - 4.f * std::abs (p - .5f);
}

float TrapVoice::polyBlep (double phase, double dt)
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

float TrapVoice::blepSaw (double ph, double dt)
{
    const double naive = 2. * ph - 1.;
    return float (naive - (double) polyBlep (ph, dt) + (double) polyBlep (wrap (ph + dt), dt));
}

float TrapVoice::blepPulse (double ph, double dt, float width)
{
    const float w = juce::jlimit (0.05f, 0.95f, width);
    const double naive = ph < w ? 1. : -1.;
    float out = float (naive);
    out -= polyBlep (ph, dt);
    out += polyBlep (wrap (ph - w), dt);
    return out;
}

float TrapVoice::ladderStep (float in, float& z1, float& z2, float& z3, float& z4, float g)
{
    const float fb = juce::jlimit (0.f, 3.8f, g);
    float x = in - z4 * fb * 0.25f;
    z1 += g * (std::tanh (x) - z1);
    z2 += g * (std::tanh (z1) - z2);
    z3 += g * (std::tanh (z2) - z3);
    z4 += g * (std::tanh (z3) - z4);
    return z4;
}

float TrapVoice::synthesizeEngine (int engine, double basePhase, double step, float morph)
{
    const double ph = basePhase;
    switch (engine)
    {
        case 0: // supersaw
        {
            float sum = 0.f;
            const int voices = juce::jmax (1, patch.unisonVoices);
            for (int u = 0; u < voices; ++u)
            {
                const float det = (float (u) - (voices - 1) * 0.5f) * patch.detune * 0.025f;
                const double dStep = step * std::pow (2., det / 1200.);
                sum += blepSaw (wrap (ph + u * 0.019), dStep);
            }
            return sum / float (voices);
        }
        case 1: // organ drawbars
            return 0.5f * std::sin (tau * ph) + 0.3f * std::sin (tau * ph * 2.02) + 0.15f * std::sin (tau * ph * 3.98)
                 + 0.08f * std::sin (tau * ph * 5.01) + 0.06f * std::sin (tau * ph * 7.97);
        case 2: // string stack
            return 0.38f * blepSaw (ph, step) + 0.32f * blepSaw (wrap (ph + 0.11), step * 1.002)
                 + 0.22f * std::sin (tau * ph) + 0.08f * triangle (float (ph));
        case 3: // FM 2-op bell
        {
            const double mod = std::sin (tau * ph * patch.fmRatio + patch.fmIndex * std::sin (tau * ph));
            return float (mod * std::sin (tau * ph * 2.));
        }
        case 4: // sub / 808
            return 0.82f * std::sin (tau * ph) + 0.12f * blepPulse (ph, step, juce::jlimit (0.08f, 0.22f, morph));
        case 5: // choir vowel stack
            return 0.4f * std::sin (tau * ph) + 0.35f * std::sin (tau * ph * 2.01) + 0.18f * std::sin (tau * ph * 3.03)
                 + 0.07f * std::sin (tau * ph * 4.97);
        case 6: // hard sync lead
        {
            syncPhase = wrap (syncPhase + step * (patch.fmRatio + 1.5));
            if (syncPhase >= 1.)
                syncPhase -= 1.;
            carrierPhase = wrap (carrierPhase + step * (patch.fmRatio * 0.35 + 1.));
            const float syncGate = syncPhase < 0.92 ? 1.f : 0.f;
            return blepSaw (carrierPhase, step) * syncGate;
        }
        case 7: // wavetable morph (synth only)
            return wavetableMorph (ph, morph);
        case 8:
            return blepPulse (ph, step, morph);
        case 9: // EP tine
            return 0.45f * std::sin (tau * ph + 2.2f * std::sin (tau * ph * 3.2)) + 0.35f * std::sin (tau * ph * 2.)
                 + 0.12f * blepSaw (wrap (ph + 0.5), step * 2.);
        case 10: // harsh FM
            return float (std::sin (tau * ph * 1.17 + 5.2f * std::sin (tau * ph * patch.fmRatio))
                        + 0.4f * std::sin (tau * ph * 3.91));
        case 11: // ring mod
        {
            const float car = std::sin (tau * ph);
            const float mod = std::sin (tau * ph * (patch.fmRatio + 2.5));
            return car * mod * (0.2f + patch.ringModDepth);
        }
        case 12: // noise pad body
            return wavetableMorph (ph, morph) * (1.f - patch.noiseMix) + hashNoise (noiseSeed) * patch.noiseMix;
        case 13: // 4-op FM shimmer
        {
            const double m1 = std::sin (tau * ph * 1.0);
            const double m2 = std::sin (tau * ph * patch.fmRatio + patch.fmIndex * m1);
            const double m3 = std::sin (tau * ph * (patch.fmRatio * 1.7) + 1.5 * m2);
            return float (std::sin (tau * ph * 2.9 + m3 + 0.6 * m2));
        }
        case 14: // comb resonant bow
            return 0.55f * std::sin (tau * ph) + 0.35f * triangle (float (ph)) + patch.noiseMix * 0.15f;
        case 15: // airy triangle pad
            return 0.62f * triangle (float (ph)) + 0.28f * std::sin (tau * ph * 0.5f);
        case 16: // brass
            return 0.72f * blepSaw (ph, step) + 0.28f * std::sin (tau * ph);
        case 17: // soft PWM keys
            return 0.5f * blepPulse (ph, step, morph) + 0.35f * std::sin (tau * ph);
        case 18: // digital wave keys
            return wavetableMorph (wrap (ph * 1.003), juce::jlimit (0.15f, 0.85f, morph * 1.1f));
        case 19: // bowed noise
            return 0.35f * hashNoise (noiseSeed) + 0.45f * blepSaw (ph, step) + 0.2f * std::sin (tau * ph);
        case 20: // swell bed (low fundamentals + drift)
            return 0.5f * std::sin (tau * ph * 0.5) + 0.3f * std::sin (tau * ph) + 0.2f * triangle (float (ph * 0.25));
        case 21: // pluck keys
            return 0.55f * std::sin (tau * ph + patch.fmIndex * std::sin (tau * ph * 5.)) + 0.35f * wavetableMorph (ph, morph);
        case 22: // distorted 808
            return 0.9f * std::sin (tau * ph) + 0.08f * hashNoise (noiseSeed) + 0.02f * blepPulse (ph, step, 0.12f);
        case 23: // impact noise burst
            return 0.7f * hashNoise (noiseSeed) + 0.3f * std::sin (tau * ph * (5.f + patch.fmRatio));
        default:
            return std::sin (tau * ph);
    }
}

float TrapVoice::applyFilter (float x, float cutoffHz, float res)
{
    const float sr = float (sampleRate);
    cutoffHz = juce::jlimit (20.f, sr * 0.45f, cutoffHz * patch.keyTrack);
    const float q = juce::jlimit (0.5f, 1.4f, 0.5f + res * 0.95f);

    switch (filterTypeApplied)
    {
        case 0:
        case 1:
        case 2:
            svf.setCutoffFrequency (cutoffHz);
            svf.setResonance (q);
            return svf.processSample (0, x);
        case 3: // ladder
        {
            const float g = juce::jlimit (0.01f, 0.85f, cutoffHz / (sr * 0.5f));
            return ladderStep (x, ladderZ1, ladderZ2, ladderZ3, ladderZ4, g * (1.f + res));
        }
        case 4: // comb
        {
            const int delay = juce::jlimit (8, 4000, int (sr / juce::jmax (40.f, cutoffHz)));
            int idx = combWrite - delay;
            if (idx < 0)
                idx += int (std::size (combBuf));
            const float delayed = combBuf[size_t (idx)];
            const float fb = juce::jlimit (0.1f, 0.88f, res);
            const float y = x + delayed * fb;
            combBuf[size_t (combWrite)] = y;
            combWrite = (combWrite + 1) % int (std::size (combBuf));
            return y;
        }
        case 5: // formant triple
        {
            const float base = cutoffHz;
            const float freqs[3] = { base * 0.75f, base * 1.25f, base * 2.2f };
            float sum = 0.f;
            int fi = 0;
            for (auto& f : formants)
            {
                f.setCutoffFrequency (juce::jlimit (80.f, sr * 0.45f, freqs[fi++]));
                f.setResonance (q);
                sum += f.processSample (0, x);
            }
            return sum / 3.f;
        }
        default:
            return x;
    }
}

void TrapVoice::renderNextBlock (juce::AudioBuffer<float>& out, int start, int n)
{
    if (! isVoiceActive())
        return;

    const float baseCutoff = juce::jlimit (20.f, float (sampleRate * .45f), patch.cutoff);
    if (! std::isfinite (baseCutoff))
        return;

    const double portAlpha = patch.portamentoSec > 0.001f ? std::exp (-1. / (patch.portamentoSec * sampleRate)) : 0.;

    for (int i = 0; i < n; ++i)
    {
        if (portAlpha > 0.)
            glideFreq = glideFreq * portAlpha + targetFreq * (1. - portAlpha);
        else
            glideFreq = targetFreq;

        const double pitchMod = pitchEnv > 0.0001f ? std::pow (2., (patch.pitchEnvSemi * pitchEnv) / 12.) : 1.;
        if (pitchEnv > 0.0001f)
            pitchEnv *= float (pitchDecay);

        const double vibrato = patch.vibratoDepth > 0.001f
                                 ? std::pow (2., patch.vibratoDepth * std::sin (tau * vibratoPhase) / 12.)
                                 : 1.;
        vibratoPhase += patch.vibratoRate / sampleRate;
        if (vibratoPhase > 1.)
            vibratoPhase -= 1.;

        lfoPhase += (0.35 + patch.oscMorph * 1.1) / sampleRate;
        if (lfoPhase > 1.)
            lfoPhase -= 1.;
        const float lfo = std::sin (float (tau * lfoPhase));

        const double baseFreq = glideFreq * bend * pitchMod * vibrato;
        const double step = juce::jlimit (1e-9, 0.5, baseFreq / sampleRate);
        const float morph = juce::jlimit (0.05f, 0.95f, patch.oscMorph + 0.22f * lfo * patch.swellDepth);

        const float swellAmt = patch.swellDepth * (0.15f + 0.85f * modWheel01);
        float dynamicCutoff = juce::jlimit (20.f, float (sampleRate * .45f),
                                            baseCutoff * juce::jmap (swellAmt, 0.f, 1.f, 0.42f, 1.65f));
        dynamicCutoff *= 1.f + 0.42f * lfo * (patch.swellDepth + 0.08f);

        const int eng = juce::jlimit (0, 23, patch.engine);
        float x = synthesizeEngine (eng, phase, step, morph);
        x += voiceDrift * std::sin (float (tau * phase * 1.3));

        phase = wrap (phase + step);
        phase2 = wrap (phase2 + step * 1.0023);
        phase3 = wrap (phase3 + step * 0.9971);
        noiseSeed += 17.31;

        const float filtered = applyFilter (std::isfinite (x) ? x : 0.f, dynamicCutoff, patch.resonance);
        const float normDrive = std::max (.001f, std::tanh (juce::jmax (0.01f, patch.drive)));
        const float envAmp = env.getNextSample();
        const float swellGain = 1.f + swellAmt * (0.35f + 0.55f * envAmp);
        float y = (std::tanh (filtered * patch.drive) / normDrive) * envAmp * velocity * patch.gain * swellGain;
        if (! std::isfinite (y))
            y = 0.f;

        if (fastReleaseGain < 1.f)
        {
            fastReleaseGain *= 0.992f;
            y *= fastReleaseGain;
        }

        const float width = juce::jlimit (0.f, 0.45f, patch.detune * 0.015f + patch.noiseMix * 0.12f);
        const float panL = 1.f - width * (0.5f + 0.5f * std::sin (float (tau * phase)));
        const float panR = 1.f + width * (0.5f + 0.5f * std::cos (float (tau * phase)));

        if (out.getNumChannels() >= 2)
        {
            out.addSample (0, start + i, y * panL);
            out.addSample (1, start + i, y * panR);
        }
        else
            out.addSample (0, start + i, y);
    }

    if (! env.isActive())
        clearCurrentNote();
}
