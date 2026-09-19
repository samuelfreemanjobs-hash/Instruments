#include "Content/ContentEngine.h"

#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace disklordz::rompler::content
{

namespace
{

bool loadPcmBanksFromDir (const fs::path& dir, assets::RawRomLibrary& library)
{
    if (! fs::is_directory (dir))
        return false;

    int loaded = 0;
    for (const auto& entry : fs::directory_iterator (dir))
    {
        const auto name = entry.path().filename().string();
        if (name.rfind ("pcm_bank_", 0) != 0 || entry.path().extension() != ".dlrrom")
            continue;

        std::ifstream in (entry.path(), std::ios::binary);
        if (! in)
            continue;
        in.seekg (0, std::ios::end);
        const auto sz = static_cast<std::size_t> (in.tellg());
        in.seekg (0, std::ios::beg);
        std::vector<std::uint8_t> bytes (sz);
        in.read (reinterpret_cast<char*> (bytes.data()), static_cast<std::streamsize> (sz));
        if (library.loadBankEmbedded (bytes.data(), bytes.size()))
            ++loaded;
    }
    return loaded > 0;
}

} // namespace

bool ContentEngine::mountFactory (const std::string& romDirectory) noexcept
{
    if (! loadPcmBanksFromDir (fs::path (romDirectory), library_))
        return false;
    mountedIds_.push_back ("DL_PKG_FACTORY");
    return true;
}

bool ContentEngine::mountExpansion (const std::string& expansionRoot, const std::string& contentId) noexcept
{
    const fs::path romDir = fs::path (expansionRoot) / "rom";
    if (! loadPcmBanksFromDir (romDir, library_))
        return false;
    mountedIds_.push_back (contentId);
    return true;
}

} // namespace disklordz::rompler::content
