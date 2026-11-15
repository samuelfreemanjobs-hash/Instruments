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
            opium_.setParams(opium::presetStopBreathing());
            useFusionBass_ = false;
            break;
        case KitStyle::DrakeFutureOvo:
            ovo_.setParams(ovofreebandz::makePreset("40 Underwater"));
            useFusionBass_ = false;
            break;
        case KitStyle::SgpRaider:
            sgp_.setParams(sgp::presetMysteriousPhonk());
            useFusionBass_ = false;
            break;
        case KitStyle::JuicySp1200:
            juicy_.setParams(juicysp::presetBlacklandRadio());
            useFusionBass_ = false;
            break;
        case KitStyle::ShawtyRedd:
            shawty_.setParams(shawtyredd::presetTrapOrDie());
            useFusionBass_ = false;
            break;
        case KitStyle::TayKeithMemphis:
            taykeith_.setParams(taykeith::presetNonstopClub());
            useFusionBass_ = false;
            break;
        case KitStyle::DjToompAtlanta:
            toomp_.setParams(toomp::presetTrapMuzik());
            useFusionBass_ = false;
            break;
        case KitStyle::Purps808Mafia:
            purps_.setParams(purps::presetGrapeful());
            useFusionBass_ = false;
            break;
        default:
            break;
        }
    }

    KitStyle style() const noexcept { return style_; }
    const KitProgram& kit() const noexcept { return kit_; }

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
};

} // namespace disklordz
