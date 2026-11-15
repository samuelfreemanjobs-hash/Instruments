#include "DSP/JuicySp1200Engine.h"
#include "DSP/Multi808FusionReactor.h"
#include "DSP/MultiGenreDrumEngine.h"
#include "DSP/ShawtyReddEngine.h"
#include "DSP/TayKeithMemphisEngine.h"
#include "DSP/ToompDrumEngine.h"
#include "DSP/ProceduralAtlantaKit.h"
#include "DSP/Purps808MafiaEngine.h"
#include "Lab/IntelligentDrumLab.h"
#include "Export/WavExporter.h"
#include "Harmony/ScaleEngine.h"
#include "Presets/HitmakerBank.h"
#include "Presets/MelodyMotionBank.h"
#include "ThreeLayerDrum.h"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

static int gFails = 0;

#define CHECK(cond, msg)                                                                 \
    do                                                                                   \
    {                                                                                    \
        if (!(cond))                                                                     \
        {                                                                                \
            std::printf("FAIL: %s\n", msg);                                              \
            ++gFails;                                                                    \
        }                                                                                \
    } while (0)

int main()
{
    constexpr double sr = 48000.0;
    constexpr int frames = 48000;

    {
        disklordz::hitmaker::Multi808FusionReactor fusion;
        fusion.prepare(sr);
        auto p = disklordz::hitmaker::makeFusionFamily(0);
        std::vector<float> buf(static_cast<size_t>(frames * 2), 0.f);
        fusion.renderOneShot(p, buf.data(), frames, p.rootHz, 0.95f);
        const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), frames);
        CHECK(qa.ok, "fusion 808 QA");
        CHECK(qa.peak > 0.05f, "fusion 808 has energy");
        CHECK(!qa.hasNan, "fusion finite");
    }

    {
        auto kick = disklordz::hitmaker::makeKick(disklordz::hitmaker::Bank::Earthquake);
        auto snare = disklordz::hitmaker::makeSnare(disklordz::hitmaker::Bank::AtlantaHeat);
        auto kb = disklordz::ThreeLayerVoice::renderOffline(kick, sr, 0.95f);
        auto sb = disklordz::ThreeLayerVoice::renderOffline(snare, sr, 0.95f);
        CHECK(!kb.empty() && !sb.empty(), "kick/snare buffers");
        float peakK = 0.f, peakS = 0.f;
        for (auto& s : kb)
            peakK = std::max(peakK, std::max(std::fabs(s.left), std::fabs(s.right)));
        for (auto& s : sb)
            peakS = std::max(peakS, std::max(std::fabs(s.left), std::fabs(s.right)));
        CHECK(peakK > 0.02f, "kick peak");
        CHECK(peakS > 0.02f, "snare peak");
    }

    {
        auto kit = disklordz::melodymotion::makeKit(disklordz::melodymotion::Bank::RichBounce);
        CHECK(kit.pads[4].recipe.family == disklordz::InstrumentFamily::Bass808, "MM bass pad");
        auto ctx = disklordz::harmony::makeDefaultTrapProgression(57);
        std::vector<disklordz::harmony::NoteSuggestion> notes;
        disklordz::harmony::suggestBassNotes(ctx, 36, true, notes);
        CHECK(notes.size() >= 2u, "bass suggestions");
    }

    {
        disklordz::juicysp::JuicySp1200Engine eng;
        eng.prepare(sr);
        auto jp = disklordz::juicysp::presetTripleSixCowbell();
        std::vector<float> buf(static_cast<size_t>((frames / 2) * 2), 0.f);
        eng.renderOneShot(disklordz::juicysp::VoiceID::VoiceCowbell, jp, buf.data(), frames / 2, 67);
        const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), frames / 2);
        CHECK(qa.peak > 0.01f, "cowbell energy");
        CHECK(!qa.hasNan, "cowbell finite");
    }

    {
        // Heap-allocate: MultiGenre embeds RoomReverb + 32 voices (too large for default stack).
        auto eng = std::make_unique<disklordz::MultiGenreDrumEngine>();
        eng->prepare(sr);
        eng->setStyle(disklordz::KitStyle::HitmakerEarthquake);
        eng->handleMidiNote(36, 0.9f);
        eng->handleMidiNote(38, 0.85f);
        float peak = 0.f;
        for (int i = 0; i < 8000; ++i)
        {
            float l = 0.f, r = 0.f;
            eng->renderTick(l, r);
            peak = std::max(peak, std::max(std::fabs(l), std::fabs(r)));
            if (!std::isfinite(l) || !std::isfinite(r))
            {
                CHECK(false, "tick finite");
                break;
            }
        }
        CHECK(peak > 0.01f, "multigenre energy");
    }

    {
        disklordz::shawtyredd::ShawtyReddEngine redd;
        redd.prepare(sr);
        auto rp = disklordz::shawtyredd::presetTrapOrDie();
        std::vector<float> hard(static_cast<size_t>(12000 * 2)), soft(static_cast<size_t>(12000 * 2));
        redd.renderOneShot(disklordz::shawtyredd::VoiceID::VoiceReddSnare, rp, hard.data(), 12000, 38);
        // Soft velocity accent path
        redd.reset();
        redd.setParams(rp);
        redd.handleMidiNote(38, 0.4f);
        for (int i = 0; i < 12000; ++i)
        {
            float l = 0.f, r = 0.f;
            redd.renderTick(l, r);
            soft[static_cast<size_t>(i * 2)] = l;
            soft[static_cast<size_t>(i * 2 + 1)] = r;
        }
        const auto qaH = disklordz::export_wav::analyzeInterleavedStereo(hard.data(), 12000);
        const auto qaS = disklordz::export_wav::analyzeInterleavedStereo(soft.data(), 12000);
        CHECK(qaH.peak > 0.02f && qaS.peak > 0.01f, "shawty snare accent energy");
        CHECK(!qaH.hasNan && !qaS.hasNan, "shawty snare finite");

        // Hat choke: open then closed
        redd.reset();
        redd.handleMidiNote(46, 0.9f);
        redd.handleMidiNote(42, 0.9f);
        float peak = 0.f;
        for (int i = 0; i < 4000; ++i)
        {
            float l = 0.f, r = 0.f;
            redd.renderTick(l, r);
            peak = std::max(peak, std::fabs(l));
        }
        CHECK(peak > 0.01f, "shawty hat choke path");
    }

    {
        disklordz::taykeith::TayKeithMemphisEngine tk;
        tk.prepare(sr);
        auto tp = disklordz::taykeith::presetNonstopClub();
        std::vector<float> buf(static_cast<size_t>(24000 * 2), 0.f);
        tk.renderOneShot(disklordz::taykeith::VoiceID::Voice808, tp, buf.data(), 24000);
        const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), 24000);
        CHECK(qa.peak > 0.05f && !qa.hasNan, "tay keith 808");

        tk.reset();
        tk.setParams(tp);
        tk.renderOneShot(disklordz::taykeith::VoiceID::VoiceSnareStack, tp, buf.data(), 12000);
        const auto qs = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), 12000);
        CHECK(qs.peak > 0.02f, "tay keith snare stack");

        std::vector<disklordz::taykeith::PatternHit> hits;
        disklordz::taykeith::makeFourBarBounce(tp, hits);
        CHECK(hits.size() > 40u, "tay keith 4-bar pattern density");
    }

    {
        // Factory pads bake non-silent before any WAV load
        std::array<disklordz::toomp::AtlantaPadBuffer, 8> pads;
        disklordz::toomp::ProceduralAtlantaKit::populateDefaultPads(pads, sr);
        float padPeak = 0.f;
        for (const auto& pad : pads)
        {
            CHECK(pad.numFrames > 64, "atlanta pad length");
            for (int i = 0; i < pad.numFrames; ++i)
                padPeak = std::max(padPeak, std::fabs(pad.sampleL(i)));
        }
        CHECK(padPeak > 0.1f, "procedural atlanta kit energy");

        disklordz::toomp::ToompDrumEngine toomp;
        toomp.prepare(sr);
        auto tp = disklordz::toomp::presetTrapMuzik();
        std::vector<float> buf(static_cast<size_t>(24000 * 2), 0.f);
        toomp.renderOneShot(disklordz::toomp::VoiceID::Voice808, tp, buf.data(), 24000);
        const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), 24000);
        CHECK(qa.peak > 0.05f && !qa.hasNan, "toomp pitch-drop 808");

        toomp.renderOneShot(disklordz::toomp::VoiceID::VoiceSnare, tp, buf.data(), 12000);
        const auto qs = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), 12000);
        CHECK(qs.peak > 0.02f, "toomp crisp snare");

        // Hat choke: open then closed
        toomp.reset();
        toomp.handleMidiNote(46, 0.9f);
        toomp.handleMidiNote(42, 0.9f);
        float peak = 0.f;
        for (int i = 0; i < 4000; ++i)
        {
            float l = 0.f, r = 0.f;
            toomp.renderTick(l, r);
            peak = std::max(peak, std::fabs(l));
        }
        CHECK(peak > 0.01f, "toomp hat choke path");

        // Custom pad overwrite API (fallback kit remains if not used)
        std::vector<float> custom(4800 * 2, 0.f);
        for (int i = 0; i < 4800; ++i)
        {
            const float s = std::sin(static_cast<float>(i) * 0.05f) * std::exp(-i / 800.0f);
            custom[static_cast<size_t>(i * 2)] = s;
            custom[static_cast<size_t>(i * 2 + 1)] = s;
        }
        toomp.loadPadInterleaved(2, custom.data(), 4800);
        CHECK(toomp.getPadBuffer(2).numFrames == 4800, "custom pad overwrite frames");

        std::vector<disklordz::toomp::PatternHit> hits;
        disklordz::toomp::makeFourBarTrapMuzik(tp, hits);
        CHECK(hits.size() > 40u, "toomp 4-bar pattern density");
    }

    {
        disklordz::purps::Purps808MafiaEngine purps;
        purps.prepare(sr);
        auto pp = disklordz::purps::presetGrapeful();
        std::vector<float> buf(static_cast<size_t>(24000 * 2), 0.f);
        purps.renderOneShot(disklordz::purps::VoiceID::Voice808, pp, buf.data(), 24000);
        const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), 24000);
        CHECK(qa.peak > 0.05f && !qa.hasNan, "purps hard-clipped 808");

        purps.renderOneShot(disklordz::purps::VoiceID::VoiceSnare, pp, buf.data(), 12000);
        const auto qs = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), 12000);
        CHECK(qs.peak > 0.02f, "purps hybrid DMX snare");

        std::vector<disklordz::purps::PatternHit> hits;
        disklordz::purps::makeFourBarMafiaBounce(pp, hits);
        CHECK(hits.size() > 40u, "purps 4-bar pattern density");
    }

    {
        const char* prompt =
            "Generate 8 hard, distorted Memphis-inspired kicks, with gritty midrange, "
            "short tails, tuned to C, optimized for MPC playback.";
        auto spec = disklordz::lab::parsePrompt(prompt, 0x4D454D50ULL);
        CHECK(spec.instrument == disklordz::lab::LabInstrument::Kick, "lab prompt→kick");
        CHECK(spec.count == 8, "lab prompt count");
        CHECK(spec.mpcOptimized && spec.shortTails && spec.gritMid, "lab prompt flags");
        CHECK(std::fabs(spec.rootHz - 65.4064f) < 0.01f, "lab prompt C2 tuning");

        auto shots = disklordz::lab::generateFromPrompt(spec, sr);
        CHECK(shots.size() >= 4u, "lab generated ranked kicks");
        CHECK(shots.front().score > -1.0e5f && shots.front().qa.peak > 0.01f, "lab top kick energy");
        CHECK(std::strstr(shots.front().filename, "Kick") != nullptr, "lab auto filename");
    }

    {
        disklordz::hitmaker::Multi808FusionReactor a, b;
        a.prepare(sr);
        b.prepare(sr);
        auto p = disklordz::hitmaker::makeFusionFamily(2);
        std::vector<float> A(4096 * 2), B(4096 * 2);
        a.renderOneShot(p, A.data(), 4096);
        b.renderOneShot(p, B.data(), 4096);
        float maxDiff = 0.f;
        for (size_t i = 0; i < A.size(); ++i)
            maxDiff = std::max(maxDiff, std::fabs(A[i] - B[i]));
        CHECK(maxDiff < 1e-6f, "fusion determinism");
    }

    if (gFails == 0)
    {
        std::printf("PASS: ddl_m1_core_test\n");
        return 0;
    }
    std::printf("FAILED: %d check(s)\n", gFails);
    return 1;
}
