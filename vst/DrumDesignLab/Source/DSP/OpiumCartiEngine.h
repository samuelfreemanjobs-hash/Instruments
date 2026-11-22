#pragma once
// DiskLordz Opium / Carti / Ken Carson engine — portable C++20 (no JUCE in DSP core).
// Blown-out 808 · laser snare · bitcrushed hats · FM glitch perk · trap hat-roll helper.
#include "CoreMathAndPRNG.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz::opium
{

struct OpiumParams
{
    float subFreq = 46.0f;
    float knockFreq = 210.0f;
    float knockDecayMs = 20.0f;
    float subDecayMs = 540.0f;
    float blowOutDrive = 4.2f;
    float wavefoldAmt = 0.35f;
    float glideMs = 35.0f;

    float snareBodyHz = 235.0f;
    float snareDecayMs = 95.0f;
    float snareClip = 2.8f;

    float hatDecayMs = 26.0f;
    float hatPitchScale = 1.12f;
    float hatCrushAmt = 0.45f;

    float perkFreqHz = 680.0f;
    float perkModRatio = 3.14159f;
    float perkDecayMs = 55.0f;

    float outputGain = 0.92f;
};

struct OpiumVoiceState
{
    bool active = false;
    float timeSec = 0.0f;
    float velocity = 1.0f;
    double phase = 0.0;
    double modPhase = 0.0;
    std::array<double, 6> hatPhases {};
    float currentNoteFreq = 46.0f;
    float targetNoteFreq = 46.0f;
};

enum class VoiceID : uint8_t
{
    Voice808 = 0,
    VoiceOpiumSnare = 1,
    VoiceDigitalHat = 2,
    VoiceGlitchPerk = 3,
    Count = 4
};

inline float midiToHz(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline float lerp(float a, float b, float t) noexcept
{
    return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
}

/** Named factory patches. */
inline OpiumParams makePreset(const char* name) noexcept
{
    OpiumParams p {};
    if (name == nullptr)
        return p;

    // "Stop Breathing / F1lthy"
    if (name[0] == 'S' || name[0] == 's' || name[0] == 'F' || name[0] == 'f')
    {
        if (name[0] == 'F' || name[0] == 'f' || (name[1] == 't' || name[1] == 'T'))
        {
            // Prefer F1lthy / Stop Breathing when F or St...
        }
        p.blowOutDrive = 5.8f;
        p.wavefoldAmt = 0.45f;
        p.subFreq = 44.0f;
        p.snareClip = 3.2f;
        p.knockFreq = 220.0f;
        p.hatCrushAmt = 0.50f;
        if (name[0] == 'S' || name[0] == 's')
        {
            // Stop Breathing
            p.blowOutDrive = 5.8f;
            p.wavefoldAmt = 0.45f;
            p.subFreq = 44.0f;
            p.snareClip = 3.2f;
        }
        return p;
    }

    // Fighting My Demons / Ken
    if (name[0] == 'K' || name[0] == 'k' || (name[0] == 'F' && name[1] == 'i'))
    {
        p.blowOutDrive = 6.2f;
        p.hatCrushAmt = 0.65f;
        p.hatDecayMs = 22.0f;
        p.wavefoldAmt = 0.50f;
        p.subFreq = 46.0f;
        p.perkDecayMs = 48.0f;
        return p;
    }

    // Sky / Star Boy Melodic Rage
    if (name[0] == 'Y' || name[0] == 'y' || (name[0] == 'S' && name[1] == 'k'))
    {
        p.blowOutDrive = 2.6f;
        p.wavefoldAmt = 0.15f;
        p.glideMs = 60.0f;
        p.subDecayMs = 620.0f;
        p.hatCrushAmt = 0.25f;
        p.hatPitchScale = 1.05f;
        return p;
    }

    // Default WLR punch
    p.blowOutDrive = 4.2f;
    p.wavefoldAmt = 0.35f;
    return p;
}

inline OpiumParams presetStopBreathing() noexcept
{
    OpiumParams p {};
    p.blowOutDrive = 5.8f;
    p.wavefoldAmt = 0.45f;
    p.subFreq = 44.0f;
    p.snareClip = 3.2f;
    p.knockFreq = 220.0f;
    p.hatCrushAmt = 0.50f;
    return p;
}

inline OpiumParams presetFightingMyDemons() noexcept
{
    OpiumParams p {};
    p.blowOutDrive = 6.2f;
    p.hatCrushAmt = 0.65f;
    p.hatDecayMs = 22.0f;
    p.wavefoldAmt = 0.50f;
    p.subFreq = 46.0f;
    p.perkDecayMs = 48.0f;
    return p;
}

inline OpiumParams presetSkyStarBoy() noexcept
{
    OpiumParams p {};
    p.blowOutDrive = 2.6f;
    p.wavefoldAmt = 0.15f;
    p.glideMs = 60.0f;
    p.subDecayMs = 620.0f;
    p.hatCrushAmt = 0.25f;
    p.hatPitchScale = 1.05f;
    return p;
}

/**
 * Euclidean / Trap hat-roll helper — holding one key expands into 1/16, 1/32, or triplet rolls.
 * Returns relative sample offsets + velocities for offline or sequencer injection.
 */
enum class HatRollDiv : uint8_t
{
    Sixteenth = 0,
    ThirtySecond,
    Triplet,
    Euclidean5_16,
    Euclidean7_16
};

struct HatRollHit
{
    int sampleOffset = 0;
    float velocity = 0.8f;
};

inline void generateHatRoll(HatRollDiv div, double bpm, double sampleRate, float lengthBeats,
                            float accentVel, std::vector<HatRollHit>& out) noexcept
{
    out.clear();
    const double beatSec = 60.0 / std::max(1.0, bpm);
    const double lengthSec = lengthBeats * beatSec;
    const int totalSamples = static_cast<int>(lengthSec * sampleRate);

    auto pushGrid = [&](double stepSec, bool euclid, int pulses, int steps) {
        if (!euclid)
        {
            double t = 0.0;
            int idx = 0;
            while (t < lengthSec - 1e-9)
            {
                HatRollHit h;
                h.sampleOffset = static_cast<int>(t * sampleRate);
                h.velocity = ((idx % 4) == 0) ? accentVel : accentVel * 0.62f;
                out.push_back(h);
                t += stepSec;
                ++idx;
            }
            return;
        }
        // Bjorklund-lite: distribute `pulses` across `steps`
        for (int i = 0; i < steps; ++i)
        {
            const bool hit = ((i * pulses) % steps) < pulses;
            if (!hit)
                continue;
            const double t = (static_cast<double>(i) / static_cast<double>(steps)) * lengthSec;
            HatRollHit h;
            h.sampleOffset = std::min(totalSamples - 1, static_cast<int>(t * sampleRate));
            h.velocity = accentVel * (((i % 2) == 0) ? 1.0f : 0.7f);
            out.push_back(h);
        }
    };

    switch (div)
    {
    case HatRollDiv::Sixteenth:
        pushGrid(beatSec / 4.0, false, 0, 0);
        break;
    case HatRollDiv::ThirtySecond:
        pushGrid(beatSec / 8.0, false, 0, 0);
        break;
    case HatRollDiv::Triplet:
        pushGrid(beatSec / 3.0, false, 0, 0);
        break;
    case HatRollDiv::Euclidean5_16:
        pushGrid(0.0, true, 5, 16);
        break;
    case HatRollDiv::Euclidean7_16:
        pushGrid(0.0, true, 7, 16);
        break;
    }
}

class OpiumCartiEngine
{
public:
    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        punchPeak_.setParams(fs_, 150.0, 1.5, dsp::TptSvf::Mode::BandPass);
        // Low shelf approx: mild LP boost path via soft blend in render
        subLp_.setParams(fs_, 52.0, 0.8, dsp::TptSvf::Mode::LowPass);
        dcBlock_.prepare(fs_, 24.0);
        snareHp_.setParams(fs_, 260.0, 0.707, dsp::TptSvf::Mode::HighPass);
        snareAir_.setParams(fs_, 5500.0, 0.7, dsp::TptSvf::Mode::HighPass);
        hatHp_.setParams(fs_, 8200.0, 0.707, dsp::TptSvf::Mode::HighPass);
        waveshapeL_.prepare(fs_);
        waveshapeR_.prepare(fs_);
        rng_.reseed(0x0F10C47FULL, 3ULL);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = OpiumVoiceState {};
        punchPeak_.reset();
        subLp_.reset();
        dcBlock_.reset();
        snareHp_.reset();
        snareAir_.reset();
        hatHp_.reset();
        hatHold_ = 0.0f;
        hatCrushCounter_ = 0.0f;
    }

    void setParams(const OpiumParams& p) noexcept { params_ = p; }

    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        if (velocity01 <= 0.0f)
            return;
        const float vel = std::clamp(velocity01, 0.05f, 1.0f);

        if (midiNote == 36 || (midiNote >= 48 && midiNote <= 72))
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::Voice808)];
            const float noteHz = (midiNote == 36) ? params_.subFreq : midiToHz(midiNote - 24);
            if (!v.active)
                v.currentNoteFreq = noteHz;
            v.targetNoteFreq = noteHz;
            v.active = true;
            v.timeSec = 0.0f;
            v.velocity = vel;
            return;
        }

        int id = -1;
        if (midiNote == 38 || midiNote == 39 || midiNote == 40)
            id = static_cast<int>(VoiceID::VoiceOpiumSnare);
        else if (midiNote == 42 || midiNote == 44)
            id = static_cast<int>(VoiceID::VoiceDigitalHat);
        else if (midiNote == 46 || midiNote == 49)
            id = static_cast<int>(VoiceID::VoiceGlitchPerk);

        if (id < 0)
            return;

        auto& v = voices_[static_cast<size_t>(id)];
        v.active = true;
        v.timeSec = 0.0f;
        v.velocity = vel;
        v.phase = 0.0;
        v.modPhase = 0.0;
        if (id == static_cast<int>(VoiceID::VoiceDigitalHat))
            v.hatPhases = {};
    }

    /** Trigger a hat roll from a single key (injects VoiceDigitalHat hits at offsets). */
    void triggerHatRoll(HatRollDiv div, double bpm, float lengthBeats, float velocity01) noexcept
    {
        pendingRoll_.clear();
        generateHatRoll(div, bpm, fs_, lengthBeats, std::clamp(velocity01, 0.05f, 1.0f), pendingRoll_);
        rollSampleCursor_ = 0;
        rollActive_ = !pendingRoll_.empty();
        nextRollIndex_ = 0;
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        // Fire scheduled hat-roll hits
        if (rollActive_ && nextRollIndex_ < pendingRoll_.size())
        {
            while (nextRollIndex_ < pendingRoll_.size()
                   && pendingRoll_[nextRollIndex_].sampleOffset <= rollSampleCursor_)
            {
                handleMidiNote(42, pendingRoll_[nextRollIndex_].velocity);
                ++nextRollIndex_;
            }
            if (nextRollIndex_ >= pendingRoll_.size())
                rollActive_ = false;
        }
        ++rollSampleCursor_;

        const auto& p = params_;
        const float dt = 1.0f / static_cast<float>(fs_);
        float subOut = 0.0f;
        float percL = 0.0f;
        float percR = 0.0f;

        // 1. Blown-out 808
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::Voice808)];
            if (v.active)
            {
                const float glideCoeff =
                    std::exp(-1.0f / (std::max(1.0f, p.glideMs) * 0.001f * static_cast<float>(fs_)));
                v.currentNoteFreq = glideCoeff * v.currentNoteFreq + (1.0f - glideCoeff) * v.targetNoteFreq;

                const float freq = v.currentNoteFreq
                    + (p.knockFreq - v.currentNoteFreq) * std::exp(-v.timeSec / (p.knockDecayMs * 0.001f));
                v.phase = std::fmod(v.phase + static_cast<double>(freq) / fs_, 1.0);

                const float twoPi = static_cast<float>(dsp::kTwoPi);
                const float ph = static_cast<float>(v.phase) * twoPi;
                float osc = std::sin(ph) + 0.25f * std::sin(ph * 2.0f);
                const float env = std::exp(-v.timeSec / (p.subDecayMs * 0.001f));

                float shaped = osc + 0.55f * punchPeak_.process(osc); // 150Hz presence
                const float sub = subLp_.process(osc);
                shaped = shaped * 0.75f + sub * 0.40f;
                shaped = dcBlock_.process(shaped * p.blowOutDrive * 0.45f);
                subOut = shaped * env * v.velocity;

                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // 2. Laser snare / clap
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceOpiumSnare)];
            if (v.active)
            {
                const float tau = p.snareDecayMs * 0.001f;
                const float bodyFreq = p.snareBodyHz * (0.55f + 1.1f * std::exp(-v.timeSec / 0.014f));
                v.phase = std::fmod(v.phase + static_cast<double>(bodyFreq) / fs_, 1.0);
                const float body = std::sin(static_cast<float>(v.phase * dsp::kTwoPi))
                    * std::exp(-v.timeSec / (tau * 0.4f));
                const float noise = rng_.nextSigned() * std::exp(-v.timeSec / tau);
                float raw = snareHp_.process(body * 0.45f + noise * 0.75f);
                raw = raw + 0.45f * snareAir_.process(raw);
                const float hit = std::tanh(raw * p.snareClip) * v.velocity;
                percL += hit;
                percR += hit * 0.94f;
                v.timeSec += dt;
                if (v.timeSec > tau * 6.0f)
                    v.active = false;
            }
        }

        // 3. Bitcrushed digital hat
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceDigitalHat)];
            if (v.active)
            {
                static constexpr double ratios[6] = { 205.3, 304.4, 369.6, 522.7, 540.0, 800.0 };
                float sum = 0.0f;
                for (int k = 0; k < 6; ++k)
                {
                    v.hatPhases[static_cast<size_t>(k)] = std::fmod(
                        v.hatPhases[static_cast<size_t>(k)]
                            + (ratios[k] * static_cast<double>(p.hatPitchScale)) / fs_,
                        1.0);
                    sum += (v.hatPhases[static_cast<size_t>(k)] < 0.5) ? 0.16f : -0.16f;
                }

                const float step = 1.0f + p.hatCrushAmt * 3.5f;
                hatCrushCounter_ += 1.0f;
                if (hatCrushCounter_ >= step)
                {
                    hatCrushCounter_ -= step;
                    hatHold_ = sum;
                }

                const float env = std::exp(-v.timeSec / (p.hatDecayMs * 0.001f));
                const float hit = hatHp_.process(hatHold_) * env * v.velocity * 1.3f;
                percL += hit * 1.1f;
                percR += hit * 0.9f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // 4. FM glitch perk
        {
            auto& v = voices_[static_cast<size_t>(VoiceID::VoiceGlitchPerk)];
            if (v.active)
            {
                const float tau = p.perkDecayMs * 0.001f;
                const float env = std::exp(-v.timeSec / tau);
                const float modIndex = 7.5f * std::exp(-v.timeSec / (tau * 0.35f));
                v.modPhase = std::fmod(
                    v.modPhase + static_cast<double>(p.perkFreqHz * p.perkModRatio) / fs_, 1.0);
                const float mod =
                    std::sin(static_cast<float>(v.modPhase * dsp::kTwoPi)) * modIndex;
                v.phase = std::fmod(v.phase + static_cast<double>(p.perkFreqHz) / fs_, 1.0);
                const float hit =
                    std::sin(static_cast<float>(v.phase * dsp::kTwoPi) + mod) * env * v.velocity * 0.85f;
                percL += hit * 0.7f;
                percR += hit * 1.15f;
                v.timeSec += dt;
                if (env < 0.0005f)
                    v.active = false;
            }
        }

        // 4x oversampled wavefold / asymmetric clip on glued bus
        float busL = (subOut + percL) * p.outputGain;
        float busR = (subOut + percR) * p.outputGain;
        const auto curve = (p.wavefoldAmt > 0.2f) ? dsp::OversampledWaveshaper4x::Curve::Wavefold
                                                  : dsp::OversampledWaveshaper4x::Curve::AsymmetricClip;
        outL = waveshapeL_.process(busL, 1.0f + p.blowOutDrive * 0.15f, curve, p.wavefoldAmt);
        outR = waveshapeR_.process(busR, 1.0f + p.blowOutDrive * 0.15f, curve, p.wavefoldAmt);
        outL = std::tanh(outL);
        outR = std::tanh(outR);
    }

    void renderOneShot(VoiceID id, const OpiumParams& p, float* interleavedLR, int numFrames,
                       int midiNote = -1) noexcept
    {
        setParams(p);
        reset();
        int note = midiNote;
        if (note < 0)
        {
            switch (id)
            {
            case VoiceID::Voice808: note = 36; break;
            case VoiceID::VoiceOpiumSnare: note = 38; break;
            case VoiceID::VoiceDigitalHat: note = 42; break;
            case VoiceID::VoiceGlitchPerk: note = 46; break;
            default: note = 36; break;
            }
        }
        handleMidiNote(note, 1.0f);
        for (int i = 0; i < numFrames; ++i)
        {
            float l = 0.0f, r = 0.0f;
            renderTick(l, r);
            interleavedLR[i * 2] = l;
            interleavedLR[i * 2 + 1] = r;
        }
    }

private:
    double fs_ = 44100.0;
    OpiumParams params_ {};
    std::array<OpiumVoiceState, static_cast<size_t>(VoiceID::Count)> voices_ {};
    dsp::TptSvf punchPeak_, subLp_, snareHp_, snareAir_, hatHp_;
    dsp::OnePoleDcBlocker dcBlock_;
    dsp::OversampledWaveshaper4x waveshapeL_, waveshapeR_;
    dsp::DeterministicRng rng_;
    float hatHold_ = 0.0f;
    float hatCrushCounter_ = 0.0f;

    std::vector<HatRollHit> pendingRoll_;
    int rollSampleCursor_ = 0;
    size_t nextRollIndex_ = 0;
    bool rollActive_ = false;
};

} // namespace disklordz::opium
