// DiskLordz Kit Sample Factory CLI
// Usage:
//   DiskLordzBatchRenderer.exe [outDir]
//   DiskLordzBatchRenderer.exe --all [--variations N] [outDir]
//   DiskLordzBatchRenderer.exe --formula INDEX [--variations N] [outDir]
//   DiskLordzBatchRenderer.exe --legacy [outDir]   (genre engine one-shots)
#include "DSP/MultiGenreDrumEngine.h"
#include "DSP/Purps808MafiaEngine.h"
#include "DSP/TayKeithMemphisEngine.h"
#include "DSP/ToompDrumEngine.h"
#include "DSP/ProceduralAtlantaKit.h"
#include "Export/WavExporter.h"
#include "Factory/KitFactory.h"
#include "Presets/GenreBankLibrary.h"
#include "Presets/HitmakerBank.h"
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static void printHelp()
{
    std::printf(
        "DiskLordz Drum Sample Kit Factory\n"
        "  --all                 Export all 32 DNA formula kits\n"
        "  --formula INDEX       Export one DNA formula (0..31)\n"
        "  --variations N        Variations per pad (1..16, default 1)\n"
        "  --legacy              Also render genre-engine one-shots\n"
        "  --list                Print formula catalog\n"
        "  --help                This help\n"
        "  [outDir]              Output root (default: renders/kit_factory)\n");
}

static int runLegacy(const std::string& outDir)
{
    fs::create_directories(outDir);
    const std::string tkDir = outDir + "/taykeith_memphis";
    fs::create_directories(tkDir);
    const std::string toompDir = outDir + "/dj_toomp_atlanta";
    fs::create_directories(toompDir);
    const std::string purpsDir = outDir + "/purps_808_mafia";
    fs::create_directories(purpsDir);

    constexpr double sr = 48000.0;
    constexpr int frames = 48000;
    int written = 0;
    int failed = 0;

    auto eng = std::make_unique<disklordz::MultiGenreDrumEngine>();
    eng->prepare(sr);
    eng->setStyle(disklordz::KitStyle::HitmakerEarthquake);

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
    }

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

    std::printf("legacy one-shots: wrote=%d failed=%d -> %s\n", written, failed, outDir.c_str());
    return (written > 0 && failed == 0) ? 0 : 1;
}

int main(int argc, char** argv)
{
    bool doAll = false;
    bool doLegacy = false;
    bool doList = false;
    int formula = -1;
    int variations = 1;
    std::string outDir = "renders/kit_factory";

    for (int i = 1; i < argc; ++i)
    {
        const char* a = argv[i];
        if (std::strcmp(a, "--help") == 0 || std::strcmp(a, "-h") == 0)
        {
            printHelp();
            return 0;
        }
        if (std::strcmp(a, "--list") == 0)
        {
            doList = true;
            continue;
        }
        if (std::strcmp(a, "--all") == 0)
        {
            doAll = true;
            continue;
        }
        if (std::strcmp(a, "--legacy") == 0)
        {
            doLegacy = true;
            continue;
        }
        if (std::strcmp(a, "--formula") == 0 && i + 1 < argc)
        {
            formula = std::atoi(argv[++i]);
            continue;
        }
        if (std::strcmp(a, "--variations") == 0 && i + 1 < argc)
        {
            variations = std::atoi(argv[++i]);
            continue;
        }
        if (a[0] != '-')
            outDir = a;
    }

    if (doList)
    {
        for (int i = 0; i < disklordz::genre_bank::kFormulaCount; ++i)
        {
            const auto& f = disklordz::genre_bank::formulaAt(i);
            std::printf("%2d  %-28s  %s\n", i, f.id, f.displayName);
        }
        return 0;
    }

    // Default: export all DNA kits (kit factory mode)
    if (!doAll && formula < 0 && !doLegacy)
        doAll = true;

    disklordz::kit_factory::ExportOptions opt;
    opt.variations = variations;
    opt.sampleRate = 48000.0;
    opt.maxFrames = 48000;
    opt.familyFolders = true;

    int rc = 0;
    if (doAll)
    {
        std::printf("Kit Factory: exporting %d DNA formulas (variations=%d) -> %s\n",
                    disklordz::genre_bank::kFormulaCount, opt.variations, outDir.c_str());
        const auto r = disklordz::kit_factory::exportAllDnaFormulas(outDir, opt);
        std::printf("Kit Factory: wrote=%d failed=%d skipped_qa=%d\n",
                    r.written, r.failed, r.skippedQa);
        if (r.written <= 0 || r.failed > 0)
            rc = 1;
    }
    else if (formula >= 0)
    {
        if (formula >= disklordz::genre_bank::kFormulaCount)
        {
            std::printf("formula index out of range (0..%d)\n",
                        disklordz::genre_bank::kFormulaCount - 1);
            return 1;
        }
        const auto& f = disklordz::genre_bank::formulaAt(formula);
        std::printf("Kit Factory: exporting %s -> %s\n", f.displayName, outDir.c_str());
        const auto r = disklordz::kit_factory::exportDnaFormula(formula, outDir, opt);
        std::printf("Kit Factory: wrote=%d failed=%d skipped_qa=%d -> %s\n",
                    r.written, r.failed, r.skippedQa, r.rootPath.c_str());
        if (r.written <= 0 || r.failed > 0)
            rc = 1;
    }

    if (doLegacy)
    {
        const int lrc = runLegacy(outDir + "/_legacy_engines");
        if (lrc != 0)
            rc = lrc;
    }

    return rc;
}
