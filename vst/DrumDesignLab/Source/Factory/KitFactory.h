#pragma once
// Offline drum sample kit factory — pack folders, variations, QA, MANIFEST.json.
// Message/worker thread only — never call from the audio thread.
#include "../Export/WavExporter.h"
#include "../Presets/GenreBankLibrary.h"
#include "../ThreeLayerDrum.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

namespace disklordz::kit_factory
{

namespace fs = std::filesystem;

struct ExportOptions
{
    double sampleRate = 48000.0;
    int maxFrames = 48000;
    int variations = 1;       // 1..16
    bool familyFolders = true;
    float peakCeiling = 0.98f;
    bool writeManifest = true;
};

struct ExportResult
{
    int written = 0;
    int failed = 0;
    int skippedQa = 0;
    std::string rootPath;
    std::string lastError;
};

inline const char* familyFolder(InstrumentFamily f) noexcept
{
    switch (f)
    {
    case InstrumentFamily::Bass808: return "808";
    case InstrumentFamily::Kick: return "Kick";
    case InstrumentFamily::Snare: return "Snare";
    case InstrumentFamily::Clap: return "Clap";
    case InstrumentFamily::ClosedHat: return "ClosedHat";
    case InstrumentFamily::OpenHat: return "OpenHat";
    case InstrumentFamily::RimTom: return "RimTom";
    case InstrumentFamily::FmBellPerc: return "Perc";
    case InstrumentFamily::SyntheticFoleyFx: return "FX";
    case InstrumentFamily::TimpaniForge: return "Timpani";
    case InstrumentFamily::WarDrumForge: return "WarDrum";
    case InstrumentFamily::ConcertBassForge: return "ConcertBass";
    case InstrumentFamily::MilitarySnareForge: return "MilitarySnare";
    case InstrumentFamily::TomForge: return "Tom";
    case InstrumentFamily::CymbalForge: return "Cymbal";
    case InstrumentFamily::CinematicImpactForge: return "Impact";
    default: return "Misc";
    }
}

inline const char* padRoleName(int padIndex) noexcept
{
    static constexpr const char* kRoles[16] = {
        "Kick", "Rim", "Snare", "Clap", "808", "Perc", "CH", "808B",
        "CH2", "Tom", "OH", "FX", "Kick2", "Ride", "Snare2", "Impact"
    };
    if (padIndex < 0 || padIndex >= 16)
        return "Pad";
    return kRoles[padIndex];
}

inline std::string sanitizeToken(const char* s)
{
    std::string out;
    for (const char* p = s; p != nullptr && *p != '\0'; ++p)
    {
        const char c = *p;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            out.push_back(c);
        else if (c == ' ' || c == '-' || c == '_' || c == '/')
            out.push_back('_');
    }
    if (out.empty())
        out = "kit";
    return out;
}

inline void normalizePeak(float* interleavedLR, int frames, float ceiling) noexcept
{
    float peak = 0.f;
    for (int i = 0; i < frames * 2; ++i)
        peak = std::max(peak, std::fabs(interleavedLR[i]));
    if (peak < 1e-8f || peak <= ceiling)
        return;
    const float g = ceiling / peak;
    for (int i = 0; i < frames * 2; ++i)
        interleavedLR[i] *= g;
}

inline KitProgram mutateKitSeeds(KitProgram kit, int variation, uint64_t baseSeed) noexcept
{
    if (variation <= 0)
        return kit;
    const uint64_t salt = baseSeed + static_cast<uint64_t>(variation) * 0x9E3779B97F4A7C15ULL;
    for (auto& pad : kit.pads)
        pad.recipe.masterSeed ^= salt + static_cast<uint64_t>(pad.midiNote) * 0x10001ULL;
    return kit;
}

inline bool writeManifestJson(const fs::path& path, const std::string& kitId,
                              const std::string& displayName, int sampleRate,
                              const std::vector<std::string>& files, int variations)
{
    FILE* f = nullptr;
#if defined(_MSC_VER)
    if (fopen_s(&f, path.string().c_str(), "wb") != 0 || f == nullptr)
        return false;
#else
    f = std::fopen(path.string().c_str(), "wb");
    if (f == nullptr)
        return false;
#endif
    std::fprintf(f,
                 "{\n  \"product\": \"DiskLordz Drum Design Lab\",\n"
                 "  \"kit_id\": \"%s\",\n"
                 "  \"display_name\": \"%s\",\n"
                 "  \"sample_rate\": %d,\n"
                 "  \"bit_depth\": 16,\n"
                 "  \"channels\": 2,\n"
                 "  \"variations\": %d,\n"
                 "  \"synthesis\": \"procedural\",\n"
                 "  \"files\": [\n",
                 kitId.c_str(), displayName.c_str(), sampleRate, variations);
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::fprintf(f, "    \"%s\"%s\n", files[i].c_str(),
                     (i + 1 < files.size()) ? "," : "");
    }
    std::fprintf(f, "  ]\n}\n");
    std::fclose(f);
    return true;
}

inline bool writeReadme(const fs::path& path, const std::string& displayName)
{
    FILE* f = nullptr;
#if defined(_MSC_VER)
    if (fopen_s(&f, path.string().c_str(), "wb") != 0 || f == nullptr)
        return false;
#else
    f = std::fopen(path.string().c_str(), "wb");
    if (f == nullptr)
        return false;
#endif
    std::fprintf(f,
                 "DiskLordz Drum Design Lab — Sample Kit Factory\n"
                 "Kit: %s\n\n"
                 "All one-shots are synthesis-only (no third-party WAV libraries).\n"
                 "Folder layout is family-based for MPC / Ableton / FL drag-drop.\n"
                 "See MANIFEST.json for file inventory and sample rate.\n",
                 displayName.c_str());
    std::fclose(f);
    return true;
}

/** Export one DNA formula as a commercial-style kit pack. */
inline ExportResult exportDnaFormula(int formulaIndex, const fs::path& outRoot,
                                     const ExportOptions& opt = {})
{
    ExportResult result;
    const int vars = std::clamp(opt.variations, 1, 16);
    const auto& formula = genre_bank::formulaAt(formulaIndex);
    const std::string kitFolder = sanitizeToken(formula.id);
    const fs::path kitRoot = outRoot / kitFolder;
    result.rootPath = kitRoot.string();

    std::error_code ec;
    fs::create_directories(kitRoot, ec);
    if (ec)
    {
        result.lastError = "mkdir failed: " + kitRoot.string();
        result.failed = 1;
        return result;
    }

    std::vector<std::string> relativeFiles;
    relativeFiles.reserve(static_cast<size_t>(16 * vars));

    for (int v = 0; v < vars; ++v)
    {
        auto kit = genre_bank::makeFormulaKit(formulaIndex, 0);
        kit = mutateKitSeeds(kit, v, 0xDF41ULL + static_cast<uint64_t>(formulaIndex));

        for (int pad = 0; pad < static_cast<int>(KitProgram::kPadCount); ++pad)
        {
            const auto& recipe = kit.pads[static_cast<size_t>(pad)].recipe;
            auto stereo = ThreeLayerVoice::renderOffline(recipe, opt.sampleRate, 0.95f);
            const int n = std::min(opt.maxFrames, static_cast<int>(stereo.size()));
            if (n <= 0)
            {
                ++result.failed;
                continue;
            }

            std::vector<float> interleaved(static_cast<size_t>(n * 2), 0.f);
            for (int i = 0; i < n; ++i)
            {
                interleaved[static_cast<size_t>(i * 2)] = stereo[static_cast<size_t>(i)].left;
                interleaved[static_cast<size_t>(i * 2 + 1)] = stereo[static_cast<size_t>(i)].right;
            }
            normalizePeak(interleaved.data(), n, opt.peakCeiling);

            const auto qa = export_wav::analyzeInterleavedStereo(interleaved.data(), n);
            if (qa.silent || qa.hasNan)
            {
                ++result.skippedQa;
                continue;
            }

            const std::string family = familyFolder(recipe.family);
            const std::string role = padRoleName(pad);
            char fileName[160];
            if (vars > 1)
                std::snprintf(fileName, sizeof(fileName), "%s_%s_V%02d.wav",
                              kitFolder.c_str(), role.c_str(), v + 1);
            else
                std::snprintf(fileName, sizeof(fileName), "%s_%s.wav",
                              kitFolder.c_str(), role.c_str());

            fs::path dest = kitRoot;
            if (opt.familyFolders)
            {
                dest /= family;
                fs::create_directories(dest, ec);
            }
            dest /= fileName;

            if (!export_wav::writeWav16Stereo(dest.string(), interleaved.data(), n,
                                              static_cast<int>(opt.sampleRate)))
            {
                ++result.failed;
                continue;
            }

            ++result.written;
            if (opt.familyFolders)
                relativeFiles.push_back(family + "/" + fileName);
            else
                relativeFiles.push_back(fileName);
        }
    }

    if (opt.writeManifest)
    {
        writeManifestJson(kitRoot / "MANIFEST.json", formula.id, formula.displayName,
                          static_cast<int>(opt.sampleRate), relativeFiles, vars);
        writeReadme(kitRoot / "README.txt", formula.displayName);
    }
    return result;
}

/** Export every DNA formula (32 kits) under outRoot. */
inline ExportResult exportAllDnaFormulas(const fs::path& outRoot, const ExportOptions& opt = {})
{
    ExportResult total;
    total.rootPath = outRoot.string();
    fs::create_directories(outRoot);
    for (int i = 0; i < genre_bank::kFormulaCount; ++i)
    {
        const auto r = exportDnaFormula(i, outRoot, opt);
        total.written += r.written;
        total.failed += r.failed;
        total.skippedQa += r.skippedQa;
        if (!r.lastError.empty() && total.lastError.empty())
            total.lastError = r.lastError;
    }
    return total;
}

/** Export a live KitProgram (e.g. current MultiGenre kit_) as a pack. */
inline ExportResult exportKitProgram(const KitProgram& kitIn, const fs::path& outRoot,
                                     const char* kitId, const char* displayName,
                                     const ExportOptions& opt = {})
{
    ExportResult result;
    const int vars = std::clamp(opt.variations, 1, 16);
    const std::string folder = sanitizeToken(kitId);
    const fs::path kitRoot = outRoot / folder;
    result.rootPath = kitRoot.string();
    fs::create_directories(kitRoot);

    std::vector<std::string> relativeFiles;
    for (int v = 0; v < vars; ++v)
    {
        auto kit = mutateKitSeeds(kitIn, v, 0xD15C0001ULL);
        for (int pad = 0; pad < static_cast<int>(KitProgram::kPadCount); ++pad)
        {
            const auto& recipe = kit.pads[static_cast<size_t>(pad)].recipe;
            auto stereo = ThreeLayerVoice::renderOffline(recipe, opt.sampleRate, 0.95f);
            const int n = std::min(opt.maxFrames, static_cast<int>(stereo.size()));
            if (n <= 0)
            {
                ++result.failed;
                continue;
            }
            std::vector<float> interleaved(static_cast<size_t>(n * 2), 0.f);
            for (int i = 0; i < n; ++i)
            {
                interleaved[static_cast<size_t>(i * 2)] = stereo[static_cast<size_t>(i)].left;
                interleaved[static_cast<size_t>(i * 2 + 1)] = stereo[static_cast<size_t>(i)].right;
            }
            normalizePeak(interleaved.data(), n, opt.peakCeiling);
            const auto qa = export_wav::analyzeInterleavedStereo(interleaved.data(), n);
            if (qa.silent || qa.hasNan)
            {
                ++result.skippedQa;
                continue;
            }
            const std::string family = familyFolder(recipe.family);
            char fileName[160];
            if (vars > 1)
                std::snprintf(fileName, sizeof(fileName), "%s_%s_V%02d.wav",
                              folder.c_str(), padRoleName(pad), v + 1);
            else
                std::snprintf(fileName, sizeof(fileName), "%s_%s.wav",
                              folder.c_str(), padRoleName(pad));
            fs::path dest = kitRoot;
            if (opt.familyFolders)
            {
                dest /= family;
                fs::create_directories(dest);
            }
            dest /= fileName;
            if (!export_wav::writeWav16Stereo(dest.string(), interleaved.data(), n,
                                              static_cast<int>(opt.sampleRate)))
            {
                ++result.failed;
                continue;
            }
            ++result.written;
            relativeFiles.push_back(opt.familyFolders ? (family + "/" + fileName) : fileName);
        }
    }
    if (opt.writeManifest)
    {
        writeManifestJson(kitRoot / "MANIFEST.json", kitId, displayName,
                          static_cast<int>(opt.sampleRate), relativeFiles, vars);
        writeReadme(kitRoot / "README.txt", displayName);
    }
    return result;
}

} // namespace disklordz::kit_factory
