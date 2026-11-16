#pragma once
// Kit Style Selector facade — routes MIDI to the active genre engine.
#include "DrakeFutureEngine.h"
#include "JuicySp1200Engine.h"
#include "Multi808FusionReactor.h"
#include "OpiumCartiEngine.h"
#include "SgpRaiderEngine.h"
#include "ShawtyReddEngine.h"
#include "TayKeithMemphisEngine.h"
#include "ToompDrumEngine.h"
#include "Purps808MafiaEngine.h"
#include "../DNA/HyperDNAMorph.h"
#include "../Factory/SoundEditBridge.h"
#include "../Presets/GenreBankLibrary.h"
#include "../Presets/HitmakerBank.h"
#include "../Presets/MelodyMotionBank.h"
#include "../ThreeLayerDrum.h"
#include <cstdint>
#include <string>

namespace disklordz
{

enum class KitStyle : uint8_t
{
    CinematicShadows = 0,
    HitmakerEarthquake,
    MelodyMotionRichBounce,
    OpiumCarti,
    DrakeFutureOvo,
    SgpRaider,
    JuicySp1200,
    ShawtyRedd,
    TayKeithMemphis,
    DjToompAtlanta,
    Purps808Mafia,
    Count
};

inline const char* kitStyleName(KitStyle s) noexcept
{
    switch (s)
    {
    case KitStyle::CinematicShadows: return "Cinematic Shadows";
    case KitStyle::HitmakerEarthquake: return "Hitmaker — Earthquake";
    case KitStyle::MelodyMotionRichBounce: return "Melody & Motion — Rich Bounce";
    case KitStyle::OpiumCarti: return "Opium / Carti";
    case KitStyle::DrakeFutureOvo: return "Drake / Future OVO";
    case KitStyle::SgpRaider: return "SGP / Raider Klan";
    case KitStyle::JuicySp1200: return "Juicy SP-1200 Memphis";
    case KitStyle::ShawtyRedd: return "Shawty Redd / 2005 ATL";
    case KitStyle::TayKeithMemphis: return "Tay Keith Memphis Club";
    case KitStyle::DjToompAtlanta: return "DJ Toomp / Trap Muzik ATL";
    case KitStyle::Purps808Mafia: return "Purps / 808 Mafia";
    default: return "Unknown";
    }
}

/**
 * Unified real-time/offline router.
 * Hitmaker 808 (note 36/48–72) uses Multi808FusionReactor; other Hitmaker pads use ThreeLayer.
 */
class MultiGenreDrumEngine
{
public:
    void prepare(double sampleRate) noexcept
    {
        sr_ = sampleRate;
        threeLayer_.prepare(sr_);
        fusion_.prepare(sr_);
        opium_.prepare(sr_);
        ovo_.prepare(sr_);
        sgp_.prepare(sr_);
        juicy_.prepare(sr_);
        shawty_.prepare(sr_);
        taykeith_.prepare(sr_);
        toomp_.prepare(sr_);
        purps_.prepare(sr_);
        setStyle(KitStyle::HitmakerEarthquake);
    }

    void reset() noexcept
    {
        threeLayer_.reset();
        fusion_.reset();
        opium_.reset();
        ovo_.reset();
        sgp_.reset();
        juicy_.reset();
        shawty_.reset();
        taykeith_.reset();
        toomp_.reset();
        purps_.reset();
    }

    void setStyle(KitStyle style) noexcept
    {
        style_ = style;
        reset();
        switch (style_)
        {
        case KitStyle::CinematicShadows:
            kit_ = makeDefault16PadKit(cinematic::Mood::Dark);
            useFusionBass_ = false;
            break;
        case KitStyle::HitmakerEarthquake:
            kit_ = hitmaker::makeKit(hitmaker::Bank::Earthquake);
            fusion_.setParams(hitmaker::fusionFor(hitmaker::Bank::Earthquake));
            useFusionBass_ = true;
            break;
        case KitStyle::MelodyMotionRichBounce:
            kit_ = melodymotion::makeKit(melodymotion::Bank::RichBounce);
            useFusionBass_ = false;
            break;
        case KitStyle::OpiumCarti:
            opiumParams_ = opium::presetStopBreathing();
            opium_.setParams(opiumParams_);
            useFusionBass_ = false;
            break;
        case KitStyle::DrakeFutureOvo:
            ovoParams_ = ovofreebandz::makePreset("40 Underwater");
            ovo_.setParams(ovoParams_);
            useFusionBass_ = false;
            break;
        case KitStyle::SgpRaider:
            sgpParams_ = sgp::presetMysteriousPhonk();
            sgp_.setParams(sgpParams_);
            useFusionBass_ = false;
            break;
        case KitStyle::JuicySp1200:
            juicyParams_ = juicysp::presetBlacklandRadio();
            juicy_.setParams(juicyParams_);
            useFusionBass_ = false;
            break;
        case KitStyle::ShawtyRedd:
            shawtyParams_ = shawtyredd::presetTrapOrDie();
            shawty_.setParams(shawtyParams_);
            useFusionBass_ = false;
            break;
        case KitStyle::TayKeithMemphis:
            taykeithParams_ = taykeith::presetNonstopClub();
            taykeith_.setParams(taykeithParams_);
            useFusionBass_ = false;
            break;
        case KitStyle::DjToompAtlanta:
            toompParams_ = toomp::presetTrapMuzik();
            toomp_.setParams(toompParams_);
            useFusionBass_ = false;
            break;
        case KitStyle::Purps808Mafia:
            purpsParams_ = purps::presetGrapeful();
            purps_.setParams(purpsParams_);
            useFusionBass_ = false;
            break;
        default:
            break;
        }
    }

    KitStyle style() const noexcept { return style_; }
    const KitProgram& kit() const noexcept { return kit_; }
    int dnaFormulaIndex() const noexcept { return dnaFormulaIndex_; }

    bool usesThreeLayerPads() const noexcept
    {
        switch (style_)
        {
        case KitStyle::CinematicShadows:
        case KitStyle::HitmakerEarthquake:
        case KitStyle::MelodyMotionRichBounce:
            return true;
        default:
            return false;
        }
    }

    sound_edit::Values readSoundEdit(int padIndex) const noexcept
    {
        const int p = (padIndex < 0) ? 0 : (padIndex % static_cast<int>(KitProgram::kPadCount));
        if (usesThreeLayerPads())
            return sound_edit::fromRecipe(kit_.pads[static_cast<size_t>(p)].recipe,
                                          kit_.pads[static_cast<size_t>(p)].padGain);
        // Specialty engines: synthesize Values from active engine params for the pad family
        sound_edit::Values v;
        switch (style_)
        {
        case KitStyle::TayKeithMemphis:
            v.subHz = taykeithParams_.subFreq;
            v.punchHz = taykeithParams_.knockHz;
            v.sweepMs = taykeithParams_.knockMs;
            v.decayMs = taykeithParams_.subDecayMs;
            v.drive01 = sound_edit::clamp((taykeithParams_.drive - 0.5f) / 5.5f, 0.f, 1.f);
            v.width01 = taykeithParams_.stereoWidth;
            v.padGain = taykeithParams_.outputGain;
            break;
        case KitStyle::DjToompAtlanta:
            v.subHz = toompParams_.subFreq;
            v.decayMs = toompParams_.subDecayMs;
            v.sweepMs = toompParams_.pitchEnvDecayMs;
            v.drive01 = toompParams_.masterKnockDrive;
            v.dirt01 = toompParams_.mpcDrive;
            v.width01 = toompParams_.stereoWidth;
            v.padGain = toompParams_.outputGain;
            break;
        case KitStyle::Purps808Mafia:
            v.subHz = purpsParams_.subFreq;
            v.punchHz = purpsParams_.knockHz;
            v.sweepMs = purpsParams_.knockMs;
            v.decayMs = purpsParams_.subDecayMs;
            v.drive01 = sound_edit::clamp((purpsParams_.preDrive - 0.5f) / 5.5f, 0.f, 1.f);
            v.dirt01 = purpsParams_.hardClip;
            v.width01 = purpsParams_.stereoWidth;
            v.padGain = purpsParams_.outputGain;
            break;
        case KitStyle::OpiumCarti:
            v.subHz = opiumParams_.subFreq;
            v.punchHz = opiumParams_.knockFreq;
            v.decayMs = opiumParams_.subDecayMs;
            v.drive01 = sound_edit::clamp((opiumParams_.blowOutDrive - 0.5f) / 5.5f, 0.f, 1.f);
            v.dirt01 = opiumParams_.wavefoldAmt;
            v.padGain = opiumParams_.outputGain;
            break;
        case KitStyle::DrakeFutureOvo:
            v.subHz = ovoParams_.subFreq;
            v.punchHz = ovoParams_.punchFreq;
            v.sweepMs = ovoParams_.punchDecayMs;
            v.decayMs = ovoParams_.subDecayMs;
            v.drive01 = sound_edit::clamp((ovoParams_.gritDrive - 0.5f) / 5.5f, 0.f, 1.f);
            v.grit01 = ovoParams_.ovoUnderwaterMacro;
            v.width01 = ovoParams_.stereoWidth;
            v.padGain = ovoParams_.outputGain;
            break;
        case KitStyle::SgpRaider:
            v.subHz = sgpParams_.subFreq;
            v.punchHz = sgpParams_.knockFreq;
            v.decayMs = sgpParams_.subDecayMs;
            v.drive01 = sound_edit::clamp((sgpParams_.subWarmth - 0.5f) / 5.5f, 0.f, 1.f);
            v.grit01 = sgpParams_.cassetteGrit;
            v.reverb01 = sgpParams_.darkReverbMix;
            v.padGain = sgpParams_.outputGain;
            break;
        case KitStyle::JuicySp1200:
            v.subHz = juicyParams_.subFreq;
            v.punchHz = juicyParams_.kickPunchHz;
            v.decayMs = juicyParams_.subDecayMs;
            v.drive01 = sound_edit::clamp((juicyParams_.tapeSaturation - 0.5f) / 5.5f, 0.f, 1.f);
            v.cutoffHz = juicyParams_.ssm2044CutoffHz;
            v.padGain = juicyParams_.outputGain;
            break;
        case KitStyle::ShawtyRedd:
            v.subHz = shawtyParams_.subFreq;
            v.punchHz = shawtyParams_.kickAttackHz;
            v.decayMs = shawtyParams_.subDecayMs;
            v.drive01 = sound_edit::clamp((shawtyParams_.tubeDrive - 0.5f) / 5.5f, 0.f, 1.f);
            v.width01 = shawtyParams_.stereoWidth;
            v.padGain = shawtyParams_.outputGain;
            break;
        default:
            break;
        }
        return v;
    }

    DrumRecipe& padRecipe(int padIndex) noexcept
    {
        const int p = (padIndex < 0) ? 0 : (padIndex % static_cast<int>(KitProgram::kPadCount));
        return kit_.pads[static_cast<size_t>(p)].recipe;
    }

    const DrumRecipe& padRecipe(int padIndex) const noexcept
    {
        const int p = (padIndex < 0) ? 0 : (padIndex % static_cast<int>(KitProgram::kPadCount));
        return kit_.pads[static_cast<size_t>(p)].recipe;
    }

    void captureDnaBase(int padIndex) noexcept
    {
        dnaBase_ = padRecipe(padIndex);
        hasDnaBase_ = true;
    }

    void applyHyperDna(int padIndex, float x, float y, float influence01,
                       dna::MatrixMode mode = dna::MatrixMode::HyperDNA) noexcept
    {
        if (! usesThreeLayerPads())
            return;
        if (! hasDnaBase_)
            captureDnaBase(padIndex);
        const auto w = dna::HyperDNAMorph::calculateWeights(x, y, mode);
        padRecipe(padIndex) = dna::HyperDNAMorph::morphRecipe(dnaBase_, w, influence01, {}, mode);
    }

    void setDestruction(int padIndex, DestructionTier tier) noexcept
    {
        if (usesThreeLayerPads())
            padRecipe(padIndex).destruction = tier;
    }

    void mutatePad(int padIndex, float strength01, uint64_t seed) noexcept
    {
        auto& r = padRecipe(padIndex);
        const uint64_t s = seed ^ (static_cast<uint64_t>(padIndex) * 0x9E3779B97F4A7C15ULL);
        const float t = sound_edit::clamp(strength01, 0.f, 1.f);
        auto u = [&](int k) {
            uint64_t x = s + static_cast<uint64_t>(k) * 0x10001ULL;
            x ^= x >> 33;
            x *= 0xff51afd7ed558ccdULL;
            return static_cast<float>((x >> 40) & 0xffff) / 65535.f;
        };
        r.masterSeed ^= s;
        r.drive = sound_edit::clamp(r.drive * (0.85f + 0.35f * u(1) * t), 0.5f, 6.f);
        r.dirtBlend = sound_edit::clamp(r.dirtBlend + (u(2) - 0.5f) * 0.4f * t, 0.f, 1.f);
        r.samplerGrit12Bit = sound_edit::clamp(r.samplerGrit12Bit + (u(3) - 0.3f) * 0.5f * t, 0.f, 1.f);
        r.body.decayMs = sound_edit::clamp(r.body.decayMs * (0.75f + 0.5f * u(4) * t), 15.f, 2500.f);
        r.body.pitchStart = sound_edit::clamp(r.body.pitchStart * (0.85f + 0.4f * u(5) * t), 60.f, 500.f);
        r.attack.level = sound_edit::clamp(r.attack.level + (u(6) - 0.5f) * 0.35f * t, 0.f, 1.25f);
        r.texture.level = sound_edit::clamp(r.texture.level + (u(7) - 0.5f) * 0.35f * t, 0.f, 1.25f);
        captureDnaBase(padIndex);
    }

    /** Message-thread / non-RT: edit focused pad (ThreeLayer) or voice family (specialty engines). */
    void applySoundEdit(int padIndex, const sound_edit::Values& values) noexcept
    {
        const int p = (padIndex < 0) ? 0 : (padIndex % static_cast<int>(KitProgram::kPadCount));
        if (usesThreeLayerPads())
        {
            sound_edit::applyToRecipe(kit_.pads[static_cast<size_t>(p)].recipe, values);
            kit_.pads[static_cast<size_t>(p)].padGain = sound_edit::clamp(values.padGain, 0.1f, 2.f);
            captureDnaBase(p);
            return;
        }
        switch (style_)
        {
        case KitStyle::TayKeithMemphis:
            sound_edit::applyToTayKeith(taykeithParams_, p, values);
            taykeith_.setParams(taykeithParams_);
            break;
        case KitStyle::DjToompAtlanta:
            sound_edit::applyToToomp(toompParams_, p, values);
            toomp_.setParams(toompParams_);
            break;
        case KitStyle::Purps808Mafia:
            sound_edit::applyToPurps(purpsParams_, p, values);
            purps_.setParams(purpsParams_);
            break;
        case KitStyle::OpiumCarti:
            sound_edit::applyToOpium(opiumParams_, p, values);
            opium_.setParams(opiumParams_);
            break;
        case KitStyle::DrakeFutureOvo:
            sound_edit::applyToOvo(ovoParams_, p, values);
            ovo_.setParams(ovoParams_);
            break;
        case KitStyle::SgpRaider:
            sound_edit::applyToSgp(sgpParams_, p, values);
            sgp_.setParams(sgpParams_);
            break;
        case KitStyle::JuicySp1200:
            sound_edit::applyToJuicy(juicyParams_, p, values);
            juicy_.setParams(juicyParams_);
            break;
        case KitStyle::ShawtyRedd:
            sound_edit::applyToShawty(shawtyParams_, p, values);
            shawty_.setParams(shawtyParams_);
            break;
        default:
            break;
        }
    }

    /** Load any of the 32 DNA formula kits into the ThreeLayer pad map (live + offline). */
    void loadDnaFormula(int formulaIndex) noexcept
    {
        dnaFormulaIndex_ = formulaIndex % genre_bank::kFormulaCount;
        if (dnaFormulaIndex_ < 0)
            dnaFormulaIndex_ = 0;
        kit_ = genre_bank::makeFormulaKit(dnaFormulaIndex_);
        useFusionBass_ = genre_bank::usesFusionBass(dnaFormulaIndex_);
        if (useFusionBass_)
            fusion_.setParams(genre_bank::fusionForFormula(dnaFormulaIndex_));
        // Prefer ThreeLayer/Fusion path for DNA kits
        switch (genre_bank::formulaAt(dnaFormulaIndex_).line)
        {
        case genre_bank::Line::Hitmaker:
            style_ = KitStyle::HitmakerEarthquake;
            break;
        case genre_bank::Line::MelodyMotion:
            style_ = KitStyle::MelodyMotionRichBounce;
            break;
        default:
            style_ = KitStyle::CinematicShadows;
            break;
        }
        reset();
        kit_ = genre_bank::makeFormulaKit(dnaFormulaIndex_);
        if (useFusionBass_)
            fusion_.setParams(genre_bank::fusionForFormula(dnaFormulaIndex_));
        hasDnaBase_ = false;
        captureDnaBase(0);
    }

    void handleMidiNote(int midiNote, float velocity01) noexcept
    {
        switch (style_)
        {
        case KitStyle::OpiumCarti:
            opium_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::DrakeFutureOvo:
            ovo_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::SgpRaider:
            sgp_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::JuicySp1200:
            juicy_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::ShawtyRedd:
            shawty_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::TayKeithMemphis:
            taykeith_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::DjToompAtlanta:
            toomp_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::Purps808Mafia:
            purps_.handleMidiNote(midiNote, velocity01);
            return;
        case KitStyle::HitmakerEarthquake:
            if (useFusionBass_ && (midiNote == 36 || (midiNote >= 48 && midiNote <= 72)))
            {
                const auto fp = hitmaker::fusionFor(hitmaker::Bank::Earthquake);
                fusion_.noteOnMidi(midiNote, velocity01, fp.rootHz);
                return;
            }
            // fall through to pad map for snare/hats/etc.
            break;
        default:
            break;
        }

        // Three-layer pad map: MIDI 36–51 → pads 0–15
        if (midiNote >= 36 && midiNote <= 51)
            threeLayer_.triggerPad(kit_, midiNote - 36, velocity01);
    }

    void renderTick(float& outL, float& outR) noexcept
    {
        outL = outR = 0.0f;
        switch (style_)
        {
        case KitStyle::OpiumCarti:
            opium_.renderTick(outL, outR);
            return;
        case KitStyle::DrakeFutureOvo:
            ovo_.renderTick(outL, outR);
            return;
        case KitStyle::SgpRaider:
            sgp_.renderTick(outL, outR);
            return;
        case KitStyle::JuicySp1200:
            juicy_.renderTick(outL, outR);
            return;
        case KitStyle::ShawtyRedd:
            shawty_.renderTick(outL, outR);
            return;
        case KitStyle::TayKeithMemphis:
            taykeith_.renderTick(outL, outR);
            return;
        case KitStyle::DjToompAtlanta:
            toomp_.renderTick(outL, outR);
            return;
        case KitStyle::Purps808Mafia:
            purps_.renderTick(outL, outR);
            return;
        default:
            break;
        }

        float fusL = 0.f, fusR = 0.f;
        if (useFusionBass_)
            fusion_.renderTick(fusL, fusR);

        float tlL = 0.f, tlR = 0.f;
        // Render one sample via voice manager
        float blockL[1] = { 0.f };
        float blockR[1] = { 0.f };
        threeLayer_.renderSubBlock(blockL, blockR, 0, 1, 0.06f);
        tlL = blockL[0];
        tlR = blockR[0];

        outL = std::tanh(fusL + tlL);
        outR = std::tanh(fusR + tlR);
    }

    /** Offline one-shot for pad index (ThreeLayer) or fusion bass. */
    void renderPadOneShot(int padIndex, float* interleavedLR, int frames) noexcept
    {
        if (useFusionBass_ && padIndex == 4)
        {
            fusion_.renderOneShot(hitmaker::fusionFor(hitmaker::Bank::Earthquake), interleavedLR, frames);
            return;
        }
        if (padIndex < 0 || padIndex >= static_cast<int>(KitProgram::kPadCount))
            return;
        auto buf = ThreeLayerVoice::renderOffline(kit_.pads[static_cast<size_t>(padIndex)].recipe, sr_, 0.95f);
        const int n = std::min(frames, static_cast<int>(buf.size()));
        for (int i = 0; i < n; ++i)
        {
            interleavedLR[i * 2] = buf[static_cast<size_t>(i)].left;
            interleavedLR[i * 2 + 1] = buf[static_cast<size_t>(i)].right;
        }
        for (int i = n; i < frames; ++i)
        {
            interleavedLR[i * 2] = 0.f;
            interleavedLR[i * 2 + 1] = 0.f;
        }
    }

private:
    double sr_ = 48000.0;
    KitStyle style_ = KitStyle::HitmakerEarthquake;
    KitProgram kit_ {};
    bool useFusionBass_ = true;
    int dnaFormulaIndex_ = 8; // hitmaker_earthquake

    DrumVoiceManager threeLayer_;
    hitmaker::Multi808FusionReactor fusion_;
    opium::OpiumCartiEngine opium_;
    ovofreebandz::DrakeFutureEngine ovo_;
    sgp::SgpRaiderEngine sgp_;
    juicysp::JuicySp1200Engine juicy_;
    shawtyredd::ShawtyReddEngine shawty_;
    taykeith::TayKeithMemphisEngine taykeith_;
    toomp::ToompDrumEngine toomp_;
    purps::Purps808MafiaEngine purps_;

    opium::OpiumParams opiumParams_ {};
    ovofreebandz::DrakeFutureParams ovoParams_ {};
    sgp::SgpParams sgpParams_ {};
    juicysp::JuicySp1200Params juicyParams_ {};
    shawtyredd::ShawtyReddParams shawtyParams_ {};
    taykeith::TayKeithParams taykeithParams_ {};
    toomp::ToompParams toompParams_ {};
    purps::PurpsParams purpsParams_ {};

    DrumRecipe dnaBase_ {};
    bool hasDnaBase_ = false;
};

} // namespace disklordz
