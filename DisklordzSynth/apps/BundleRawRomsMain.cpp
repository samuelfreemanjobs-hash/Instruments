#include "disklordz/RawRomBundleFormat.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

int main (int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: DisklordzSynth_BundleRawRoms <input_dir_with_dlrrom> <output.cat.bin>\n";
        return EXIT_FAILURE;
    }

    const fs::path inDir (argv[1]);
    const fs::path outPath (argv[2]);

    std::vector<fs::path> files;
    for (const auto& e : fs::directory_iterator (inDir))
    {
        const auto name = e.path().filename().string();
        if (name.rfind ("pcm_bank_", 0) != 0 || e.path().extension() != ".dlrrom")
            continue;
        files.push_back (e.path());
    }
    std::sort (files.begin(), files.end());

    if (files.empty())
    {
        std::cerr << "No .dlrrom files in " << inDir << '\n';
        return EXIT_FAILURE;
    }

    std::vector<std::vector<std::uint8_t>> payloads;
    payloads.reserve (files.size());
    for (const auto& f : files)
    {
        std::ifstream in (f, std::ios::binary);
        in.seekg (0, std::ios::end);
        const auto sz = static_cast<std::size_t> (in.tellg());
        in.seekg (0, std::ios::beg);
        std::vector<std::uint8_t> buf (sz);
        in.read (reinterpret_cast<char*> (buf.data()), static_cast<std::streamsize> (sz));
        payloads.push_back (std::move (buf));
    }

    disklordz::rawrom::RomBundleHeader hdr{};
    std::memcpy (hdr.magic, disklordz::rawrom::kRomBundleMagic, 8);
    hdr.version = disklordz::rawrom::kRomBundleVersion;
    hdr.fileCount = static_cast<std::uint32_t> (files.size());

    const std::size_t tableBytes = files.size() * sizeof (disklordz::rawrom::RomBundleEntry);
    std::uint32_t cursor = static_cast<std::uint32_t> (sizeof (hdr) + tableBytes);

    std::vector<disklordz::rawrom::RomBundleEntry> table (files.size());
    for (std::size_t i = 0; i < files.size(); ++i)
    {
        auto& ent = table[i];
        std::memset (ent.name, 0, sizeof (ent.name));
        const auto stem = files[i].filename().string();
        std::memcpy (ent.name, stem.c_str(), std::min (stem.size(), sizeof (ent.name) - 1));
        ent.offset = cursor;
        ent.size = static_cast<std::uint32_t> (payloads[i].size());
        cursor += ent.size;
    }

    std::ofstream out (outPath, std::ios::binary);
    if (! out)
        return EXIT_FAILURE;

    out.write (reinterpret_cast<const char*> (&hdr), sizeof (hdr));
    out.write (reinterpret_cast<const char*> (table.data()),
               static_cast<std::streamsize> (table.size() * sizeof (table[0])));
    for (const auto& p : payloads)
        out.write (reinterpret_cast<const char*> (p.data()), static_cast<std::streamsize> (p.size()));

    std::cout << "Bundled " << files.size() << " ROM files → " << outPath << '\n';
    return out.good() ? EXIT_SUCCESS : EXIT_FAILURE;
}
