#pragma once
// Apply focused-pad / focused-voice sound edits onto recipes or genre-engine params.
#include "../DSP/DrakeFutureEngine.h"
#include "../DSP/JuicySp1200Engine.h"
#include "../DSP/OpiumCartiEngine.h"
#include "../DSP/Purps808MafiaEngine.h"
#include "../DSP/SgpRaiderEngine.h"
#include "../DSP/ShawtyReddEngine.h"
#include "../DSP/TayKeithMemphisEngine.h"
#include "../DSP/ToompDrumEngine.h"
#include "../ThreeLayerDrum.h"
#include <algorithm>
#include <cmath>

namespace disklordz::sound_edit
{

struct Values
{
    float subHz = 49.f;
    float punchHz = 165.f;
    float sweepMs = 24.f;
    float decayMs = 340.f;
    float bodyLevel = 0.85f;
    float attackLevel = 0.70f;
    float textureLevel = 0.50f;
    float drive01 = 0.35f;      // 0..1 UI → recipe drive ~0.5..6
    float dirt01 = 0.55f;
    float grit01 = 0.f;
    float width01 = 0.30f;
    float reverb01 = 0.10f;
    float cutoffHz = 9500.f;
    float fmRatio = 2.5f;
    float fmIndex = 2.2f;
    float padGain = 1.0f;
};

inline float clamp(float v, float lo, float hi) noexcept
{
    return std::max(lo, std::min(hi, v));
}

inline Values fromRecipe(const DrumRecipe& r, float padGain = 1.f) noexcept
{
    Values v;
    v.subHz = r.rootHz > 1.f ? r.rootHz : r.body.tuneHz;
    v.punchHz = r.body.pitchStart > 1.f ? r.body.pitchStart : 165.f;
    v.sweepMs = r.body.pitchMs > 1.f ? r.body.pitchMs : 24.f;
    v.decayMs = r.body.decayMs > 1.f ? r.body.decayMs : 340.f;
    v.bodyLevel = r.body.level;
    v.attackLevel = r.attack.level;
    v.textureLevel = r.texture.level;
    v.drive01 = clamp((r.drive - 0.5f) / 5.5f, 0.f, 1.f);
    v.dirt01 = clamp(r.dirtBlend, 0.f, 1.f);
    v.grit01 = clamp(r.samplerGrit12Bit, 0.f, 1.f);
    v.width01 = clamp(r.stereoWidth, 0.f, 1.f);
    v.reverb01 = clamp(r.reverbMix, 0.f, 1.f);
    v.cutoffHz = r.body.cutoffHz > 40.f ? r.body.cutoffHz : 9500.f;
    v.fmRatio = r.body.fmRatio;
    v.fmIndex = r.body.fmIndex;
    v.padGain = padGain;
    return v;
}

inline void applyToRecipe(DrumRecipe& r, const Values& v) noexcept
{
    r.rootHz = clamp(v.subHz, 20.f, 200.f);
    r.body.tuneHz = r.rootHz;
    r.body.pitchStart = clamp(v.punchHz, 40.f, 800.f);
    r.body.pitchMs = clamp(v.sweepMs, 1.f, 200.f);
    r.body.decayMs = clamp(v.decayMs, 10.f, 3000.f);
    r.attack.decayMs = clamp(v.decayMs * 0.35f, 5.f, 800.f);
    r.texture.decayMs = clamp(v.decayMs * 0.55f, 5.f, 1200.f);
    r.body.level = clamp(v.bodyLevel, 0.f, 1.25f);
    r.attack.level = clamp(v.attackLevel, 0.f, 1.25f);
    r.texture.level = clamp(v.textureLevel, 0.f, 1.25f);
    r.drive = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
    r.dirtBlend = clamp(v.dirt01, 0.f, 1.f);
    r.samplerGrit12Bit = clamp(v.grit01, 0.f, 1.f);
    r.stereoWidth = clamp(v.width01, 0.f, 1.f);
    r.reverbMix = clamp(v.reverb01, 0.f, 1.f);
    r.body.cutoffHz = clamp(v.cutoffHz, 40.f, 18000.f);
    r.attack.cutoffHz = clamp(v.cutoffHz * 1.15f, 40.f, 18000.f);
    r.body.fmRatio = clamp(v.fmRatio, 0.5f, 8.f);
    r.body.fmIndex = clamp(v.fmIndex, 0.f, 6.f);
}

/** Map pad index → sound family for specialty engines (Kick/808/Snare/Hat/Perc). */
enum class VoiceFamily : uint8_t { Kick808 = 0, Snare, Hat, Perc, Global };

inline VoiceFamily familyForPad(int pad) noexcept
{
    switch (pad)
    {
    case 0: case 4: case 7: case 12: return VoiceFamily::Kick808;
    case 2: case 3: case 14: return VoiceFamily::Snare;
    case 6: case 8: case 10: return VoiceFamily::Hat;
    default: return VoiceFamily::Perc;
    }
}

inline void applyToTayKeith(taykeith::TayKeithParams& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.knockHz = clamp(v.punchHz, 80.f, 400.f);
        p.knockMs = clamp(v.sweepMs, 5.f, 80.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.drive = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
        break;
    case VoiceFamily::Snare:
        p.snareBodyHz = clamp(v.subHz * 4.f, 120.f, 400.f);
        p.snareStartHz = clamp(v.punchHz, 150.f, 600.f);
        p.snareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        p.snareClip = 0.8f + 2.0f * clamp(v.drive01, 0.f, 1.f);
        break;
    case VoiceFamily::Hat:
        p.hatDecayMs = clamp(v.decayMs * 0.12f, 8.f, 80.f);
        p.openHatDecayMs = clamp(v.decayMs * 0.55f, 40.f, 400.f);
        break;
    default:
        p.rimHz = clamp(v.subHz * 12.f, 200.f, 2000.f);
        p.rimDecayMs = clamp(v.decayMs * 0.2f, 10.f, 120.f);
        break;
    }
    p.stereoWidth = clamp(v.width01, 0.f, 1.f);
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
}

inline void applyToToomp(toomp::ToompParams& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.pitchEnvDecayMs = clamp(v.sweepMs, 5.f, 120.f);
        p.masterKnockDrive = clamp(v.drive01, 0.f, 1.f);
        break;
    case VoiceFamily::Snare:
        p.snareBodyHz = clamp(v.subHz * 4.f, 120.f, 400.f);
        p.snareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        break;
    case VoiceFamily::Hat:
        p.hatDecayMs = clamp(v.decayMs * 0.12f, 8.f, 80.f);
        p.openHatDecayMs = clamp(v.decayMs * 0.55f, 40.f, 400.f);
        break;
    default:
        p.rimHz = clamp(v.subHz * 12.f, 200.f, 2000.f);
        break;
    }
    p.stereoWidth = clamp(v.width01, 0.f, 1.f);
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
    p.mpcDrive = clamp(v.dirt01, 0.f, 1.f);
}

inline void applyToPurps(purps::PurpsParams& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.knockHz = clamp(v.punchHz, 80.f, 400.f);
        p.knockMs = clamp(v.sweepMs, 5.f, 80.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.preDrive = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
        p.hardClip = clamp(v.dirt01, 0.f, 1.f);
        break;
    case VoiceFamily::Snare:
        p.snareBodyHz = clamp(v.subHz * 4.f, 120.f, 400.f);
        p.snareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        break;
    case VoiceFamily::Hat:
        p.hatDecayMs = clamp(v.decayMs * 0.12f, 8.f, 80.f);
        p.openHatDecayMs = clamp(v.decayMs * 0.55f, 40.f, 400.f);
        break;
    default:
        p.rimHz = clamp(v.subHz * 12.f, 200.f, 2000.f);
        p.darkLpHz = clamp(v.cutoffHz, 400.f, 8000.f);
        break;
    }
    p.stereoWidth = clamp(v.width01, 0.f, 1.f);
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
}

inline void applyToShawty(shawtyredd::ShawtyReddParams& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.kickAttackHz = clamp(v.punchHz, 80.f, 400.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.tubeDrive = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
        break;
    case VoiceFamily::Snare:
        p.snareBodyHz = clamp(v.subHz * 4.f, 120.f, 400.f);
        p.snareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        p.snareSnapCutoff = clamp(v.cutoffHz, 800.f, 10000.f);
        break;
    case VoiceFamily::Hat:
        p.hatDecayMs = clamp(v.decayMs * 0.12f, 8.f, 80.f);
        p.openHatDecayMs = clamp(v.decayMs * 0.55f, 40.f, 500.f);
        break;
    default:
        break;
    }
    p.stereoWidth = clamp(v.width01, 0.f, 1.f);
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
}

inline void applyToOpium(opium::OpiumParams& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.knockFreq = clamp(v.punchHz, 80.f, 400.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.blowOutDrive = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
        p.wavefoldAmt = clamp(v.dirt01, 0.f, 1.f);
        p.glideMs = clamp(v.sweepMs * 1.5f, 0.f, 250.f);
        break;
    case VoiceFamily::Snare:
        p.snareBodyHz = clamp(v.subHz * 4.f, 120.f, 400.f);
        p.snareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        break;
    case VoiceFamily::Hat:
        p.hatDecayMs = clamp(v.decayMs * 0.12f, 8.f, 80.f);
        p.hatCrushAmt = clamp(v.grit01, 0.f, 1.f);
        break;
    default:
        p.perkFreqHz = clamp(v.subHz * 12.f, 200.f, 2000.f);
        p.perkDecayMs = clamp(v.decayMs * 0.25f, 10.f, 200.f);
        break;
    }
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
}

inline void applyToOvo(ovofreebandz::DrakeFutureParams& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.punchFreq = clamp(v.punchHz, 80.f, 400.f);
        p.punchDecayMs = clamp(v.sweepMs, 5.f, 80.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.gritDrive = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
        break;
    case VoiceFamily::Snare:
        p.trapSnareBodyHz = clamp(v.subHz * 4.f, 120.f, 400.f);
        p.trapSnareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        break;
    case VoiceFamily::Hat:
        p.hatDecayMs = clamp(v.decayMs * 0.12f, 8.f, 100.f);
        break;
    default:
        p.rimToneHz = clamp(v.subHz * 8.f, 200.f, 1200.f);
        p.rimDecayMs = clamp(v.decayMs * 0.2f, 10.f, 120.f);
        break;
    }
    p.stereoWidth = clamp(v.width01, 0.f, 1.f);
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
    p.ovoUnderwaterMacro = clamp(v.grit01, 0.f, 1.f);
}

inline void applyToSgp(sgp::SgpParams& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.knockFreq = clamp(v.punchHz, 80.f, 400.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.subWarmth = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
        break;
    case VoiceFamily::Snare:
        p.snareBodyHz = clamp(v.subHz * 3.f, 100.f, 350.f);
        p.snareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        p.darkReverbMix = clamp(v.reverb01, 0.f, 1.f);
        break;
    case VoiceFamily::Hat:
        p.hatDecayMs = clamp(v.decayMs * 0.12f, 8.f, 100.f);
        break;
    default:
        p.triangleFreqHz = clamp(v.subHz * 30.f, 400.f, 4000.f);
        p.triangleDecayMs = clamp(v.decayMs * 0.3f, 10.f, 200.f);
        break;
    }
    p.cassetteGrit = clamp(v.grit01, 0.f, 1.f);
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
}

inline void applyToJuicy(juicysp::JuicySp1200Params& p, int pad, const Values& v) noexcept
{
    switch (familyForPad(pad))
    {
    case VoiceFamily::Kick808:
        p.subFreq = clamp(v.subHz, 30.f, 80.f);
        p.kickPunchHz = clamp(v.punchHz, 80.f, 400.f);
        p.subDecayMs = clamp(v.decayMs, 80.f, 2500.f);
        p.tapeSaturation = 0.5f + 5.5f * clamp(v.drive01, 0.f, 1.f);
        break;
    case VoiceFamily::Snare:
        p.snareBodyHz = clamp(v.subHz * 4.f, 120.f, 400.f);
        p.snareDecayMs = clamp(v.decayMs, 20.f, 400.f);
        break;
    default:
        p.cowbellBaseHz = clamp(v.subHz * 10.f, 200.f, 1200.f);
        p.cowbellDecayMs = clamp(v.decayMs * 0.5f, 40.f, 400.f);
        break;
    }
    p.ssm2044CutoffHz = clamp(v.cutoffHz, 400.f, 12000.f);
    p.outputGain = clamp(v.padGain, 0.2f, 1.5f);
}

} // namespace disklordz::sound_edit
