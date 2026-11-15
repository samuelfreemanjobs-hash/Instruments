#include "DSP/Multi808FusionReactor.h"
#include "DSP/MultiGenreDrumEngine.h"
#include "DSP/TayKeithMemphisEngine.h"
#include "DSP/ToompDrumEngine.h"
#include "DSP/ProceduralAtlantaKit.h"
#include "DSP/Purps808MafiaEngine.h"
#include "Export/WavExporter.h"
#include "Presets/HitmakerBank.h"
#include <filesystem>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    const std::string outDir = (argc > 1) ? argv[1] : "renders/hitmaker_m1";
    fs::create_directories(outDir);
    const std::string tkDir = outDir + "/taykeith_memphis";
    fs::create_directories(tkDir);
    const std::string toompDir = outDir + "/dj_toomp_atlanta";
    fs::create_directories(toompDir);
    const std::string purpsDir = outDir + "/purps_808_mafia";
    fs::create_directories(purpsDir);

    constexpr double sr = 48000.0;
    constexpr int frames = 48000;

    auto eng = std::make_unique<disklordz::MultiGenreDrumEngine>();
    eng->prepare(sr);
    eng->setStyle(disklordz::KitStyle::HitmakerEarthquake);

    int written = 0;
    int failed = 0;

    // Fusion 808 families 0..3
    for (int i = 0; i < 4; ++i)
    {
        auto p = disklordz::hitmaker::makeFusionFamily(i);
        std::vector<float> buf(static_cast<size_t>(frames * 2), 0.f);
        disklordz::hitmaker::Multi808FusionReactor fusion;
        fusion.prepare(sr);
        fusion.renderOneShot(p, buf.data(), frames);
        const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), frames);
        const std::string path = outDir + "/808_fusion_" + std::to_string(i) + ".wav";
        if (qa.ok && disklordz::export_wav::writeWav16Stereo(path, buf.data(), frames, 48000))
            ++written;
        else
            ++failed;
    }

    // Kit pads 0..7 one-shots via MultiGenre
    for (int pad = 0; pad < 8; ++pad)
    {
        std::vector<float> buf(static_cast<size_t>(frames * 2), 0.f);
        eng->renderPadOneShot(pad, buf.data(), frames);
        const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), frames);
        const std::string path = outDir + "/pad_" + std::to_string(pad) + ".wav";
        if (!qa.silent && !qa.hasNan
            && disklordz::export_wav::writeWav16Stereo(path, buf.data(), frames, 48000))
            ++written;
        else
            ++failed;
    }

    // Tay Keith Memphis club one-shots
    {
        disklordz::taykeith::TayKeithMemphisEngine tk;
        tk.prepare(sr);
        const auto tp = disklordz::taykeith::presetNonstopClub();
        using VID = disklordz::taykeith::VoiceID;
        const VID voices[] = {
            VID::Voice808, VID::VoiceSnareStack, VID::VoiceClosedHat,
            VID::VoiceOpenHat, VID::VoiceRim, VID::VoiceCrash
        };
        const char* names[] = {
            "808_boomy", "snare_clap_stack", "closed_hat",
            "open_hat", "rim_woodblock", "crash_hp"
        };
        for (int i = 0; i < 6; ++i)
        {
            std::vector<float> buf(static_cast<size_t>(frames * 2), 0.f);
            tk.renderOneShot(voices[i], tp, buf.data(), frames);
            const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), frames);
            const std::string path = tkDir + "/" + names[i] + ".wav";
            if (!qa.silent && !qa.hasNan
                && disklordz::export_wav::writeWav16Stereo(path, buf.data(), frames, 48000))
                ++written;
            else
                ++failed;
        }
    }

    // DJ Toomp: export baked factory pads + engine one-shots
    {
        std::array<disklordz::toomp::AtlantaPadBuffer, 8> pads;
        disklordz::toomp::ProceduralAtlantaKit::populateDefaultPads(pads, sr);
        const char* padNames[] = {
            "factory_808_sub", "factory_top_kick", "factory_snare", "factory_clap",
            "factory_closed_hat", "factory_open_hat", "factory_rim", "factory_crash"
        };
        for (int i = 0; i < 8; ++i)
        {
            const auto& pad = pads[static_cast<size_t>(i)];
            const auto qa = disklordz::export_wav::analyzeInterleavedStereo(
                pad.interleaved.data(), pad.numFrames);
            const std::string path = toompDir + "/" + padNames[i] + ".wav";
            if (!qa.silent && !qa.hasNan
                && disklordz::export_wav::writeWav16Stereo(
                       path, pad.interleaved.data(), pad.numFrames, 48000))
                ++written;
            else
                ++failed;
        }

        disklordz::toomp::ToompDrumEngine toomp;
        toomp.prepare(sr);
        const auto tp = disklordz::toomp::presetTrapMuzik();
        using VID = disklordz::toomp::VoiceID;
        const VID voices[] = {
            VID::Voice808, VID::VoiceTopKick, VID::VoiceSnare, VID::VoiceClap,
            VID::VoiceClosedHat, VID::VoiceOpenHat, VID::VoiceRim, VID::VoiceCrash
        };
        const char* names[] = {
            "808_pitchdrop", "top_kick", "snare_crisp", "clap_layered",
            "closed_hat", "open_hat", "rim_perc", "crash"
        };
        for (int i = 0; i < 8; ++i)
        {
            std::vector<float> buf(static_cast<size_t>(frames * 2), 0.f);
            toomp.renderOneShot(voices[i], tp, buf.data(), frames);
            const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), frames);
            const std::string path = toompDir + "/" + names[i] + ".wav";
            if (!qa.silent && !qa.hasNan
                && disklordz::export_wav::writeWav16Stereo(path, buf.data(), frames, 48000))
                ++written;
            else
                ++failed;
        }
    }

    // Purps / 808 Mafia one-shots
    {
        disklordz::purps::Purps808MafiaEngine purps;
        purps.prepare(sr);
        const auto pp = disklordz::purps::presetGrapeful();
        using VID = disklordz::purps::VoiceID;
        const VID voices[] = {
            VID::Voice808, VID::VoiceSnare, VID::VoiceClosedHat,
            VID::VoiceOpenHat, VID::VoiceRim, VID::VoiceCrash
        };
        const char* names[] = {
            "808_hardclip", "snare_dmx_hybrid", "closed_hat",
            "open_hat_dark", "rim_dark_lp", "crash"
        };
        for (int i = 0; i < 6; ++i)
        {
            std::vector<float> buf(static_cast<size_t>(frames * 2), 0.f);
            purps.renderOneShot(voices[i], pp, buf.data(), frames);
            const auto qa = disklordz::export_wav::analyzeInterleavedStereo(buf.data(), frames);
            const std::string path = purpsDir + "/" + names[i] + ".wav";
            if (!qa.silent && !qa.hasNan
                && disklordz::export_wav::writeWav16Stereo(path, buf.data(), frames, 48000))
                ++written;
            else
                ++failed;
        }
    }

    std::printf("DiskLordzBatchRenderer: wrote=%d failed=%d -> %s\n", written, failed, outDir.c_str());
    return (written > 0 && failed == 0) ? 0 : 1;
}
