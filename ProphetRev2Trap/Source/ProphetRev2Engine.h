#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace AudioUtils {

inline float killDenormal(float val) noexcept
{
    static const float antiDenormal = 1e-18f;
    if (std::abs(val) < 1e-15f)
        return val + antiDenormal;
    return val;
}

inline float fastTanh(float x) noexcept
{
    if (x <= -3.0f)
        return -1.0f;
    if (x >= 3.0f)
        return 1.0f;
    return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
}

} // namespace AudioUtils

class SmoothParam
{
public:
    void reset(double sampleRate, double timeMs) noexcept
    {
        const double c = timeMs <= 0.0 ? 0.0 : std::exp(-1.0 / (sampleRate * (timeMs * 0.001)));
        coeff = static_cast<float>(c);
    }

    float process(float target) noexcept
    {
        current = target + coeff * (current - target);
        return current;
    }

private:
    float current = 0.0f;
    float coeff = 0.0f;
};

class ADSR
{
public:
    void setSampleRate(double sr) noexcept { sampleRate = sr; }

    void setParameters(float a, float d, float s, float r) noexcept
    {
        attack = a;
        decay = d;
        sustain = s;
        release = r;
    }

    void noteOn() noexcept
    {
        state = State::Attack;
        currentOutput = 0.0f;
    }

    void noteOff() noexcept
    {
        if (state != State::Idle)
        {
            state = State::Release;
            releaseStartVal = currentOutput;
            releaseProgress = 0.0f;
        }
    }

    float process() noexcept
    {
        switch (state)
        {
        case State::Idle:
            currentOutput = 0.0f;
            break;
        case State::Attack: {
            const float samples = std::max(1.0f, attack * static_cast<float>(sampleRate));
            currentOutput += (1.0f / samples);
            if (currentOutput >= 1.0f)
            {
                currentOutput = 1.0f;
                state = State::Decay;
            }
            break;
        }
        case State::Decay: {
            const float samples = std::max(1.0f, decay * static_cast<float>(sampleRate));
            currentOutput -= ((1.0f - sustain) / samples);
            if (currentOutput <= sustain)
            {
                currentOutput = sustain;
                state = State::Sustain;
            }
            break;
        }
        case State::Sustain:
            currentOutput = sustain;
            break;
        case State::Release: {
            const float samples = std::max(1.0f, release * static_cast<float>(sampleRate));
            releaseProgress += (1.0f / samples);
            currentOutput = releaseStartVal * (1.0f - releaseProgress);
            if (releaseProgress >= 1.0f || currentOutput <= 0.0f)
            {
                currentOutput = 0.0f;
                state = State::Idle;
            }
            break;
        }
        }
        return currentOutput;
    }

    bool isActive() const noexcept { return state != State::Idle; }

private:
    enum class State
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };
    State state = State::Idle;
    double sampleRate = 44100.0;
    float attack = 0.01f;
    float decay = 0.1f;
    float sustain = 0.7f;
    float release = 0.2f;
    float currentOutput = 0.0f;
    float releaseStartVal = 0.0f;
    float releaseProgress = 0.0f;
};

class Rev2Voice
{
public:
    explicit Rev2Voice(int voiceIndex = 0) noexcept
    {
        const float seed = 0.137f * static_cast<float>(voiceIndex + 1);
        phase1 = seed;
        phase2 = seed * 1.61f;
    }

    void init(double sr) noexcept
    {
        sampleRate = sr;
        ampEnv.setSampleRate(sr);
        filterEnv.setSampleRate(sr);
        modEnv.setSampleRate(sr);
    }

    void triggerVoice(int midiNote, float velocity, float initialGlideFreq) noexcept
    {
        noteNumber = midiNote;
        noteVelocity = velocity;
        isActive = true;
        targetFreq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
        currentGlideFreq = (initialGlideFreq > 0.0f) ? initialGlideFreq : targetFreq;
        ampEnv.noteOn();
        filterEnv.noteOn();
        modEnv.noteOn();
    }

    void releaseVoice() noexcept
    {
        ampEnv.noteOff();
        filterEnv.noteOff();
        modEnv.noteOff();
    }

    void kill() noexcept
    {
        isActive = false;
        noteNumber = -1;
        s1 = s2 = s3 = s4 = 0.0f;
    }

    void processSample(
        float& left,
        float& right,
        float glideRate,
        float slop,
        int filterMode,
        float cutoffParam,
        float resonanceParam,
        float bassComp,
        int wave1,
        int wave2,
        float pwmWidth,
        float dcoMix,
        float subLevel,
        int saturationOn,
        float crunch,
        float aA,
        float aD,
        float aS,
        float aR,
        float fA,
        float fD,
        float fS,
        float fR) noexcept
    {
        if (!isActive)
        {
            left = right = 0.0f;
            return;
        }

        if (currentGlideFreq != targetFreq)
        {
            currentGlideFreq += glideRate * (targetFreq - currentGlideFreq);
            if (std::abs(currentGlideFreq - targetFreq) < 0.1f)
                currentGlideFreq = targetFreq;
        }

        ampEnv.setParameters(aA, aD, aS, aR);
        filterEnv.setParameters(fA, fD, fS, fR);
        const float ampValue = ampEnv.process();
        const float filterEnvValue = filterEnv.process();
        modEnv.process();

        if (!ampEnv.isActive())
        {
            kill();
            left = right = 0.0f;
            return;
        }

        const float slopMod1 = 1.0f + (std::sin(phase1 * 13.37f) * 0.005f * slop);
        const float slopMod2 = 1.0f + (std::cos(phase2 * 19.84f) * 0.006f * slop);

        const float f1 = currentGlideFreq * slopMod1;
        const float f2 = currentGlideFreq * slopMod2 * 1.01f;
        const float fSub = f1 * 0.5f;

        phase1 += f1 / static_cast<float>(sampleRate);
        phase2 += f2 / static_cast<float>(sampleRate);
        phaseSub += fSub / static_cast<float>(sampleRate);
        if (phase1 >= 1.0f) phase1 -= 1.0f;
        if (phase2 >= 1.0f) phase2 -= 1.0f;
        if (phaseSub >= 1.0f) phaseSub -= 1.0f;

        const float osc1 = renderWave(phase1, wave1, pwmWidth);
        const float osc2 = renderWave(phase2, wave2, 0.5f);
        const float subOsc = (phaseSub < 0.5f) ? 1.0f : -1.0f;

        const float mixedSource = (1.0f - dcoMix) * osc1 + dcoMix * osc2 + subLevel * subOsc;

        float envCutoffHz = cutoffParam * std::pow(2.0f, filterEnvValue * 5.0f);
        envCutoffHz = std::clamp(envCutoffHz, 20.0f, 20000.0f);

        const float g = static_cast<float>(std::tan(M_PI * envCutoffHz / sampleRate));
        float r = 1.0f - resonanceParam;
        r = std::clamp(r, 0.05f, 1.0f);

        const float inputScaler = 1.0f + (resonanceParam * bassComp * 1.5f);
        const float filterInput = mixedSource * inputScaler;

        float fOut = 0.0f;
        if (filterMode == 1)
        {
            const float v1 = (filterInput - s1) * g / (1.0f + g);
            const float y1 = s1 + v1;
            s1 = y1 + v1;
            const float v2 = (y1 - s2) * g / (1.0f + g);
            fOut = s2 + v2;
            s2 = fOut + v2;
        }
        else
        {
            float y1 = (filterInput * g + s1) / (1.0f + g);
            s1 = 2.0f * y1 - s1;
            float y2 = (y1 * g + s2) / (1.0f + g);
            s2 = 2.0f * y2 - s2;
            float y3 = (y2 * g + s3) / (1.0f + g);
            s3 = 2.0f * y3 - s3;
            fOut = (y3 * g + s4) / (1.0f + g);
            s4 = 2.0f * fOut - s4;
        }

        float vcaOut = fOut * ampValue * noteVelocity;

        if (saturationOn == 1)
        {
            const float driveScaler = 1.0f + (crunch * 4.0f);
            vcaOut = AudioUtils::fastTanh(vcaOut * driveScaler) / (1.0f + crunch * 0.5f);
        }

        const float finalSample = AudioUtils::killDenormal(vcaOut);
        left = right = finalSample;
    }

    bool isVoiceActive() const noexcept { return isActive; }
    int getNoteNumber() const noexcept { return noteNumber; }
    float getGlideHz() const noexcept { return currentGlideFreq; }

private:
    static float renderWave(float phase, int type, float pwm) noexcept
    {
        switch (type)
        {
        case 0:
            return 2.0f * phase - 1.0f;
        case 1:
            return (phase < 0.5f) ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);
        default:
            return (phase < pwm) ? 1.0f : -1.0f;
        }
    }

    bool isActive = false;
    int noteNumber = -1;
    float noteVelocity = 0.0f;
    double sampleRate = 44100.0;
    float phase1 = 0.0f;
    float phase2 = 0.0f;
    float phaseSub = 0.0f;
    float targetFreq = 0.0f;
    float currentGlideFreq = 0.0f;
    float s1 = 0.0f;
    float s2 = 0.0f;
    float s3 = 0.0f;
    float s4 = 0.0f;
    ADSR ampEnv;
    ADSR filterEnv;
    ADSR modEnv;
};

/** Static 8-voice stack — zero heap on the audio thread. */
class ProphetRev2SynthEngine
{
public:
    static constexpr int kMaxVoices = 8;

    ProphetRev2SynthEngine() noexcept { loadPreset(0); }

    void prepareToPlay(double sampleRate) noexcept
    {
        currentSampleRate = sampleRate;
        glideSmooth.reset(sampleRate, 20.0);
        cutoffSmooth.reset(sampleRate, 10.0);
        resonanceSmooth.reset(sampleRate, 10.0);
        slopSmooth.reset(sampleRate, 50.0);
        crunchSmooth.reset(sampleRate, 15.0);
        for (int i = 0; i < kMaxVoices; ++i)
            voiceStack[static_cast<std::size_t>(i)].init(sampleRate);
    }

    void loadPreset(int presetIndex) noexcept
    {
        switch (presetIndex)
        {
        case 0:
            param_wave1.store(0);
            param_wave2.store(0);
            param_dcoMix.store(0.5f);
            param_slop.store(0.75f);
            param_filterMode.store(0);
            param_cutoff.store(350.0f);
            param_resonance.store(0.45f);
            param_bassComp.store(0.8f);
            param_satOn.store(1);
            param_crunch.store(0.6f);
            param_glide.store(0.02f);
            param_pwmWidth.store(0.5f);
            param_subLevel.store(0.35f);
            a_A.store(0.005f);
            a_D.store(0.25f);
            a_S.store(0.6f);
            a_R.store(0.15f);
            f_A.store(0.01f);
            f_D.store(0.35f);
            f_S.store(0.1f);
            f_R.store(0.2f);
            break;
        case 1:
            param_wave1.store(2);
            param_wave2.store(0);
            param_pwmWidth.store(0.15f);
            param_dcoMix.store(0.3f);
            param_slop.store(0.2f);
            param_filterMode.store(0);
            param_cutoff.store(180.0f);
            param_resonance.store(0.65f);
            param_bassComp.store(0.9f);
            param_satOn.store(0);
            param_crunch.store(0.0f);
            param_glide.store(0.0f);
            param_subLevel.store(0.35f);
            a_A.store(0.001f);
            a_D.store(0.12f);
            a_S.store(0.0f);
            a_R.store(0.1f);
            f_A.store(0.001f);
            f_D.store(0.09f);
            f_S.store(0.0f);
            f_R.store(0.1f);
            break;
        default:
            param_wave1.store(1);
            param_wave2.store(0);
            param_dcoMix.store(0.4f);
            param_slop.store(0.8f);
            param_filterMode.store(1);
            param_cutoff.store(800.0f);
            param_resonance.store(0.15f);
            param_bassComp.store(0.2f);
            param_satOn.store(1);
            param_crunch.store(0.2f);
            param_glide.store(0.15f);
            param_subLevel.store(0.3f);
            a_A.store(0.45f);
            a_D.store(0.5f);
            a_S.store(0.85f);
            a_R.store(0.6f);
            f_A.store(0.6f);
            f_D.store(0.7f);
            f_S.store(0.5f);
            f_R.store(0.5f);
            break;
        }
    }

    void noteOn(int midiNote, float velocity) noexcept
    {
        int targetIndex = -1;
        for (int i = 0; i < kMaxVoices; ++i)
        {
            if (voiceStack[static_cast<std::size_t>(i)].isVoiceActive()
                && voiceStack[static_cast<std::size_t>(i)].getNoteNumber() == midiNote)
            {
                targetIndex = i;
                break;
            }
        }
        if (targetIndex == -1)
        {
            for (int i = 0; i < kMaxVoices; ++i)
            {
                if (!voiceStack[static_cast<std::size_t>(i)].isVoiceActive())
                {
                    targetIndex = i;
                    break;
                }
            }
        }
        if (targetIndex == -1)
        {
            targetIndex = allocationTracker;
            allocationTracker = (allocationTracker + 1) % kMaxVoices;
            voiceStack[static_cast<std::size_t>(targetIndex)].kill();
        }

        float initialGlideFreq = 0.0f;
        if (lastActiveNoteNumber != -1 && param_glide.load() > 0.0f)
        {
            initialGlideFreq =
                440.0f * std::pow(2.0f, (lastActiveNoteNumber - 69) / 12.0f);
        }

        voiceStack[static_cast<std::size_t>(targetIndex)].triggerVoice(
            midiNote, velocity, initialGlideFreq);
        lastActiveNoteNumber = midiNote;
    }

    void noteOff(int midiNote) noexcept
    {
        for (int i = 0; i < kMaxVoices; ++i)
        {
            if (voiceStack[static_cast<std::size_t>(i)].getNoteNumber() == midiNote)
                voiceStack[static_cast<std::size_t>(i)].releaseVoice();
        }
        if (lastActiveNoteNumber == midiNote)
            lastActiveNoteNumber = -1;
    }

    void processBlock(float* left, float* right, int numSamples) noexcept
    {
        if (currentSampleRate <= 0.0)
            return;

        const float smoothGlide = glideSmooth.process(param_glide.load());
        const float smoothCutoff = cutoffSmooth.process(param_cutoff.load());
        const float smoothRes = resonanceSmooth.process(param_resonance.load());
        const float smoothSlop = slopSmooth.process(param_slop.load());
        const float smoothCrunch = crunchSmooth.process(param_crunch.load());

        const float glideCoefficient =
            (smoothGlide <= 0.0f) ? 1.0f : 1.0f / (smoothGlide * static_cast<float>(currentSampleRate) + 1.0f);

        const int w1 = param_wave1.load();
        const int w2 = param_wave2.load();
        const int fMode = param_filterMode.load();
        const int satOn = param_satOn.load();
        const float sub = param_subLevel.load();
        const float mix = param_dcoMix.load();
        const float pwm = param_pwmWidth.load();
        const float bass = param_bassComp.load();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float accumL = 0.0f;
            float accumR = 0.0f;

            for (int i = 0; i < kMaxVoices; ++i)
            {
                if (!voiceStack[static_cast<std::size_t>(i)].isVoiceActive())
                    continue;

                float vL = 0.0f;
                float vR = 0.0f;
                voiceStack[static_cast<std::size_t>(i)].processSample(
                    vL, vR, glideCoefficient, smoothSlop, fMode, smoothCutoff, smoothRes, bass, w1,
                    w2, pwm, mix, sub, satOn, smoothCrunch, a_A.load(), a_D.load(), a_S.load(),
                    a_R.load(), f_A.load(), f_D.load(), f_S.load(), f_R.load());
                accumL += vL;
                accumR += vR;
            }

            const float gain = 0.35f;
            left[sample] = accumL * gain;
            if (right != nullptr)
                right[sample] = accumR * gain;
        }
    }

    void setCutoff(float hz) noexcept { param_cutoff.store(hz); }
    void setResonance(float res) noexcept { param_resonance.store(res); }
    void setBassComp(float v) noexcept { param_bassComp.store(v); }
    void setSlop(float v) noexcept { param_slop.store(v); }
    void setSaturation(int state) noexcept { param_satOn.store(state); }
    void setCrunch(float val) noexcept { param_crunch.store(val); }
    void setGlide(float timeSec) noexcept { param_glide.store(timeSec); }
    void setDcoMix(float v) noexcept { param_dcoMix.store(v); }
    void setPwm(float v) noexcept { param_pwmWidth.store(v); }
    void setSubLevel(float v) noexcept { param_subLevel.store(v); }
    void setFilterMode(int m) noexcept { param_filterMode.store(m); }
    void setWave1(int w) noexcept { param_wave1.store(w); }
    void setWave2(int w) noexcept { param_wave2.store(w); }
    void setAmpAdsr(float a, float d, float s, float r) noexcept
    {
        a_A.store(a);
        a_D.store(d);
        a_S.store(s);
        a_R.store(r);
    }
    void setFilterAdsr(float a, float d, float s, float r) noexcept
    {
        f_A.store(a);
        f_D.store(d);
        f_S.store(s);
        f_R.store(r);
    }

private:
    double currentSampleRate = 44100.0;
    std::array<Rev2Voice, kMaxVoices> voiceStack {};
    int allocationTracker = 0;
    int lastActiveNoteNumber = -1;

    std::atomic<float> param_cutoff { 400.0f };
    std::atomic<float> param_resonance { 0.2f };
    std::atomic<float> param_bassComp { 0.7f };
    std::atomic<float> param_slop { 0.25f };
    std::atomic<float> param_glide { 0.0f };
    std::atomic<float> param_dcoMix { 0.5f };
    std::atomic<float> param_pwmWidth { 0.5f };
    std::atomic<float> param_subLevel { 0.35f };
    std::atomic<int> param_filterMode { 0 };
    std::atomic<int> param_wave1 { 0 };
    std::atomic<int> param_wave2 { 0 };
    std::atomic<int> param_satOn { 1 };
    std::atomic<float> param_crunch { 0.1f };

    std::atomic<float> a_A { 0.01f };
    std::atomic<float> a_D { 0.1f };
    std::atomic<float> a_S { 1.0f };
    std::atomic<float> a_R { 0.2f };
    std::atomic<float> f_A { 0.01f };
    std::atomic<float> f_D { 0.1f };
    std::atomic<float> f_S { 1.0f };
    std::atomic<float> f_R { 0.2f };

    SmoothParam glideSmooth;
    SmoothParam cutoffSmooth;
    SmoothParam resonanceSmooth;
    SmoothParam slopSmooth;
    SmoothParam crunchSmooth;
};
