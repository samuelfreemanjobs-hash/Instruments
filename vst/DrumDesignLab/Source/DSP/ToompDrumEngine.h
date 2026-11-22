#pragma once
// DiskLordz DJ Toomp / Trap Muzik Atlanta — portable C++20 (no JUCE).
// prepare() bakes ProceduralAtlantaKit pads; process/renderTick only plays buffers.
#include "CoreMathAndPRNG.h"
#include "ProceduralAtlantaKit.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz::toomp
{

struct ToompVoiceParams
{
    float gain = 1.0f;
    float pitchSemitones = 0.0f;
    float pitchEnvAmount = 0.0f;   // +semitones knock into fundamental
    float pitchEnvDecayMs = 45.0f;
    float drive = 0.35f;
    int chokeGroup = 0; // 1 = hi-hat choke
};

struct ToompParams
{
    float mpcDrive = 0.55f;
    float mpcBias = 0.08f;
    float transientPunch = 0.55f;
    float subShelfDb = 3.5f;
    float masterKnockDrive = 0.35f;
    float stereoWidth = 0.30f;
    float outputGain = 0.90f;
    float bpm = 140.0f;
    // Legacy aliases kept so existing call sites / presets compile
    float subFreq = 42.0f;
    float pitchEnvSemitones = 14.0f;
    float pitchEnvDecayMs = 52.0f;
    float subDecayMs = 780.0f;
    float topKickHz = 95.0f;
    float topKickDecayMs = 22.0f;
    float topKickMix = 0.55f;
    float snareBodyHz = 210.0f;
    float snareStartHz = 340.0f;
    float snareNoiseHz = 4200.0f;
    float snareDecayMs = 105.0f;
    float clapSpreadMs = 9.0f;
    float hatDecayMs = 26.0f;
    float openHatDecayMs = 210.0f;
    float rimHz = 720.0f;
    float rimDecayMs = 35.0f;
    float crashHpHz = 5000.0f;
    float crashDecayMs = 380.0f;
};

enum class VoiceID : uint8_t
{
    Voice808 = 0,
    VoiceTopKick = 1,
    VoiceSnare = 2,
    VoiceClap = 3,
    VoiceClosedHat = 4,
    VoiceOpenHat = 5,
    VoiceRim = 6,
    VoiceCrash = 7,
    Count = 8
};

inline float midiToHz(int note) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

inline float mpcSaturate(float s, float drive, float bias) noexcept
{
    if (drive < 0.001f)
        return s;
    const float pre = 1.0f + drive * 4.5f;
    const float offset = bias * drive;
    const float driven = s * pre + offset;
    const float den = std::tanh(pre);
    if (den < 1.0e-6f)
        return s;
    return (std::tanh(driven) - std::tanh(offset)) / den;
}

inline ToompParams presetTrapMuzik() noexcept
{
    ToompParams p {};
    p.pitchEnvSemitones = 14.0f;
    p.pitchEnvDecayMs = 52.0f;
    p.mpcDrive = 0.58f;
    p.bpm = 140.0f;
    return p;
}

inline ToompParams presetKing() noexcept
{
    ToompParams p {};
    p.pitchEnvSemitones = 12.0f;
    p.pitchEnvDecayMs = 48.0f;
    p.mpcDrive = 0.48f;
    p.transientPunch = 0.62f;
    p.bpm = 138.0f;
    return p;
}

struct PatternHit
{
    int step = 0;
    int midiNote = 36;
    float velocity = 0.9f;
};

inline void makeFourBarTrapMuzik(const ToompParams& p, std::vector<PatternHit>& out) noexcept
{
    out.clear();
    auto add = [&](int step, int note, float vel) {
        PatternHit h;
        h.step = step;
        h.midiNote = note;
        h.velocity = vel;
        out.push_back(h);
    };

    for (int bar = 0; bar < 4; ++bar)
    {
        const int off = bar * 16;
        for (int s : { 0, 6, 10, 14 })
            add(off + s, 36, 0.98f);
        add(off + 0, 37, 0.85f);
        add(off + 10, 37, 0.70f);
        add(off + 4, 38, 0.95f);
        add(off + 12, 38, 0.95f);
        add(off + 12, 39, 0.55f);
        for (int s = 0; s < 16; s += 2)
            add(off + s, 42, 0.55f);
        add(off + 10, 42, 0.45f);
        add(off + 11, 42, 0.50f);
        add(off + 2, 46, 0.50f);
        add(off + 8, 46, 0.42f);
        add(off + 7, 47, 0.48f);
        if (bar == 0)
            add(off + 0, 49, 0.65f);
    }
    (void)p;
}

/** Poly sample player voice — reads pre-baked AtlantaPadBuffer. */
class ToompPadVoice
{
public:
    void prepare(double sr) noexcept
    {
        sampleRate_ = sr > 0.0 ? sr : 44100.0;
        stop();
    }

    void stop() noexcept
    {
        active_ = false;
        pad_ = nullptr;
        readPos_ = 0.0;
    }

    bool isActive() const noexcept { return active_; }
    int getChokeGroup() const noexcept { return params_.chokeGroup; }

    void trigger(const AtlantaPadBuffer* sampleData, float velocity, const ToompVoiceParams& p) noexcept
    {
        if (sampleData == nullptr || sampleData->numFrames <= 1)
            return;
        pad_ = sampleData;
        params_ = p;
        velGain_ = std::clamp(velocity, 0.0f, 1.0f) * p.gain;
        readPos_ = 0.0;
        pitchEnvValue_ = p.pitchEnvAmount;
        const double decaySamples =
            std::max(1.0, (p.pitchEnvDecayMs * 0.001) * sampleRate_);
        pitchEnvCoeff_ = static_cast<float>(std::exp(-1.0 / decaySamples));
        active_ = true;
    }

    void renderTick(float& outL, float& outR, float mpcDrive, float mpcBias) noexcept
    {
        if (!active_ || pad_ == nullptr)
            return;

        const int total = pad_->numFrames;
        const int idx0 = static_cast<int>(readPos_);
        if (idx0 >= total - 1)
        {
            active_ = false;
            return;
        }

        const float frac = static_cast<float>(readPos_ - static_cast<double>(idx0));
        const int idx1 = idx0 + 1;
        const float currentSemis = params_.pitchSemitones + pitchEnvValue_;
        const double playbackRatio = std::pow(2.0, static_cast<double>(currentSemis) / 12.0);
        pitchEnvValue_ *= pitchEnvCoeff_;

        float l = pad_->sampleL(idx0) + frac * (pad_->sampleL(idx1) - pad_->sampleL(idx0));
        float r = pad_->sampleR(idx0) + frac * (pad_->sampleR(idx1) - pad_->sampleR(idx0));
        const float drive = std::max(params_.drive, mpcDrive);
        l = mpcSaturate(l, drive, mpcBias) * velGain_;
        r = mpcSaturate(r, drive, mpcBias) * velGain_;
        outL += l;
        outR += r;
        readPos_ += playbackRatio;
    }

private:
    const AtlantaPadBuffer* pad_ = nullptr;
    ToompVoiceParams params_ {};
    double sampleRate_ = 44100.0;
    double readPos_ = 0.0;
    float velGain_ = 1.0f;
    float pitchEnvValue_ = 0.0f;
    float pitchEnvCoeff_ = 0.99f;
    bool active_ = false;
};

class ToompDrumEngine
{
public:
    static constexpr int maxVoices = 16;
    static constexpr int numPads = ProceduralAtlantaKit::kPadCount;

    void prepare(double sampleRate) noexcept
    {
        fs_ = std::max(8000.0, sampleRate);
        for (auto& v : voices_)
            v.prepare(fs_);

        ProceduralAtlantaKit::populateDefaultPads(padBuffers_, fs_);
        configureDefaultAtlantaKit();

        subShelf_.setParams(fs_, 62.0, 0.85, dsp::TptSvf::Mode::LowPass);
        transientHp_.setParams(fs_, 2200.0, 0.707, dsp::TptSvf::Mode::HighPass);
        dc_.prepare(fs_, 22.0);
        tube_.prepare(fs_);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v.stop();
        subShelf_.reset();
        transientHp_.reset();
        dc_.reset();
        tube_.reset();
        pattern_.clear();
        patternActive_ = false;
        sampleCursor_ = 0;
        nextHit_ = 0;
    }

    void setParams(const ToompParams& p) noexcept
    {
        params_ = p;
        // Refresh pad 0 / 1 pitch-env defaults from global preset
        padParams_[0].pitchEnvAmount = p.pitchEnvSemitones;
        padParams_[0].pitchEnvDecayMs = p.pitchEnvDecayMs;
        padParams_[0].drive = p.mpcDrive;
        padParams_[1].drive = p.mpcDrive * 0.75f;
        padParams_[2].drive = p.mpcDrive * 0.8f;
    }

    void configureDefaultAtlantaKit() noexcept
    {
        // 808 Sub — classic Toomp pitch-drop
        padParams_[0] = { 1.05f, 0.0f, 14.0f, 52.0f, 0.55f, 0 };
        // Top punch kick
        padParams_[1] = { 0.95f, 0.0f, 4.0f, 22.0f, 0.40f, 0 };
        // Crisp Atlanta snare
        padParams_[2] = { 1.00f, 0.5f, 0.0f, 10.0f, 0.45f, 0 };
        // Layered clap
        padParams_[3] = { 0.90f, 0.0f, 0.0f, 10.0f, 0.30f, 0 };
        // Closed hat (choke group 1)
        padParams_[4] = { 0.80f, 0.0f, 0.0f, 10.0f, 0.20f, 1 };
        // Open hat (choke group 1)
        padParams_[5] = { 0.82f, -0.5f, 0.0f, 10.0f, 0.25f, 1 };
        // Rim
        padParams_[6] = { 0.85f, 0.0f, 0.0f, 10.0f, 0.25f, 0 };
        // Crash
        padParams_[7] = { 0.78f, 0.0f, 0.0f, 10.0f, 0.20f, 0 };
    }

    /** Non-RT: overwrite a factory pad with custom interleaved stereo (LRLR). */
    void loadPadInterleaved(int padIndex, const float* interleavedLR, int numFrames) noexcept
    {
        if (padIndex < 0 || padIndex >= numPads || interleavedLR == nullptr || numFrames <= 0)
            return;
        auto& buf = padBuffers_[static_cast<size_t>(padIndex)];
        buf.clearAndResize(numFrames);
        for (int i = 0; i < numFrames * 2; ++i)
            buf.interleaved[static_cast<size_t>(i)] = interleavedLR[i];
    }

    AtlantaPadBuffer& getPadBuffer(int padIndex) noexcept
    {
        const int i = std::clamp(padIndex, 0, numPads - 1);
        return padBuffers_[static_cast<size_t>(i)];
    }

    const AtlantaPadBuffer& getPadBuffer(int padIndex) const noexcept
    {
        const int i = std::clamp(padIndex, 0, numPads - 1);
        return padBuffers_[static_cast<size_t>(i)];
    }

    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        // Melodic 808: map 48–72 onto pad 0 with pitch offset
        if (midiNote >= 48 && midiNote <= 72)
        {
            ToompVoiceParams p = padParams_[0];
            p.pitchSemitones = static_cast<float>(midiNote - 60); // C3 = root
            triggerPad(0, velocity01, p);
            return;
        }

        const int pad = ProceduralAtlantaKit::midiToPad(midiNote);
        if (pad < 0)
            return;
        triggerPad(pad, velocity01, padParams_[static_cast<size_t>(pad)]);
    }

    void startTrapMuzikPattern() noexcept
    {
        makeFourBarTrapMuzik(params_, pattern_);
        patternActive_ = !pattern_.empty();
        sampleCursor_ = 0;
        nextHit_ = 0;
        const double stepSec = (60.0 / static_cast<double>(params_.bpm)) / 4.0;
        stepSamples_ = static_cast<int>(stepSec * fs_);
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        if (patternActive_ && nextHit_ < pattern_.size())
        {
            while (nextHit_ < pattern_.size())
            {
                const int due = pattern_[nextHit_].step * stepSamples_;
                if (due > sampleCursor_)
                    break;
                handleMidiNote(pattern_[nextHit_].midiNote, pattern_[nextHit_].velocity);
                ++nextHit_;
            }
            if (nextHit_ >= pattern_.size())
                patternActive_ = false;
        }
        ++sampleCursor_;

        float busL = 0.f, busR = 0.f;
        for (auto& v : voices_)
            v.renderTick(busL, busR, params_.mpcDrive, params_.mpcBias);

        // Parallel transient punch
        const float punchMono = (busL + busR) * 0.5f;
        const float punch =
            std::tanh(transientHp_.process(punchMono) * (1.0f + params_.transientPunch * 2.0f))
            * params_.transientPunch;

        // Soft 62 Hz chest shelf on mono sum blended back
        const float mono = dc_.process((busL + busR) * 0.5f);
        const float shelfGain = std::pow(10.0f, params_.subShelfDb / 20.0f);
        const float shelved = subShelf_.process(mono);
        const float lowBlend = mono * 0.65f + shelved * (0.35f * shelfGain);
        const float width = params_.stereoWidth;
        float l = (busL - mono) * (1.0f + width) + lowBlend + punch;
        float r = (busR - mono) * (1.0f + width) + lowBlend + punch;

        l *= params_.outputGain;
        r *= params_.outputGain;
        const float ceiling = 1.0f + std::clamp(params_.masterKnockDrive, 0.0f, 1.0f) * 2.5f;
        outL = tube_.process(std::tanh(l * ceiling), 1.0f + params_.mpcDrive * 0.2f,
                             dsp::OversampledWaveshaper4x::Curve::AsymmetricClip, 0.04f);
        outR = tube_.process(std::tanh(r * ceiling), 1.0f + params_.mpcDrive * 0.2f,
                             dsp::OversampledWaveshaper4x::Curve::AsymmetricClip, 0.04f);
    }

    void renderOneShot(VoiceID id, const ToompParams& p, float* interleavedLR, int numFrames,
                       int midiNote = -1) noexcept
    {
        setParams(p);
        // Do not clear baked pads — only stop voices / filters
        for (auto& v : voices_)
            v.stop();
        subShelf_.reset();
        transientHp_.reset();
        dc_.reset();
        tube_.reset();
        patternActive_ = false;

        int note = midiNote;
        if (note < 0)
        {
            switch (id)
            {
            case VoiceID::Voice808: note = 36; break;
            case VoiceID::VoiceTopKick: note = 37; break;
            case VoiceID::VoiceSnare: note = 38; break;
            case VoiceID::VoiceClap: note = 39; break;
            case VoiceID::VoiceClosedHat: note = 42; break;
            case VoiceID::VoiceOpenHat: note = 46; break;
            case VoiceID::VoiceRim: note = 47; break;
            case VoiceID::VoiceCrash: note = 49; break;
            default: note = 36; break;
            }
        }
        handleMidiNote(note, 0.95f);
        for (int i = 0; i < numFrames; ++i)
        {
            float l = 0.f, r = 0.f;
            renderTick(l, r);
            interleavedLR[i * 2] = l;
            interleavedLR[i * 2 + 1] = r;
        }
    }

private:
    void triggerPad(int padIndex, float velocity01, const ToompVoiceParams& params) noexcept
    {
        if (padIndex < 0 || padIndex >= numPads)
            return;
        const float vel = std::clamp(velocity01, 0.05f, 1.0f);

        if (params.chokeGroup > 0)
        {
            for (auto& v : voices_)
                if (v.isActive() && v.getChokeGroup() == params.chokeGroup)
                    v.stop();
        }

        ToompPadVoice* chosen = &voices_[0];
        for (auto& v : voices_)
        {
            if (!v.isActive())
            {
                chosen = &v;
                break;
            }
        }
        chosen->trigger(&padBuffers_[static_cast<size_t>(padIndex)], vel, params);
    }

    double fs_ = 48000.0;
    ToompParams params_ {};
    std::array<AtlantaPadBuffer, numPads> padBuffers_ {};
    std::array<ToompVoiceParams, numPads> padParams_ {};
    std::array<ToompPadVoice, maxVoices> voices_ {};
    dsp::TptSvf subShelf_, transientHp_;
    dsp::OnePoleDcBlocker dc_;
    dsp::OversampledWaveshaper4x tube_;
    std::vector<PatternHit> pattern_;
    bool patternActive_ = false;
    int sampleCursor_ = 0;
    int nextHit_ = 0;
    int stepSamples_ = 1;
};

} // namespace disklordz::toomp
