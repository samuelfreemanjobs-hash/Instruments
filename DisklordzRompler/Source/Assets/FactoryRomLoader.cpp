#include "Assets/FactoryRomLoader.h"

#include <JuceHeader.h>

#include <cstdlib>
#include <filesystem>

#if JUCE_LINUX || JUCE_MAC || JUCE_BSD
 #include <dlfcn.h>
#endif

namespace fs = std::filesystem;

namespace disklordz::rompler::assets
{

namespace
{

juce::File getThisSharedModuleFile()
{
#if JUCE_LINUX || JUCE_MAC || JUCE_BSD
    Dl_info info{};
    if (dladdr (reinterpret_cast<void*> (&getThisSharedModuleFile), &info) != 0 && info.dli_fname != nullptr)
        return juce::File (juce::CharPointer_UTF8 (info.dli_fname));
#endif
    return juce::File::getSpecialLocation (juce::File::currentExecutableFile);
}

fs::path romDirectoryCandidates()
{
    if (const char* env = std::getenv ("DISKLORDZ_ROM_DIR"))
        return fs::path (env);

#if defined (DISKLORDZ_FACTORY_ROM_DIR)
    return fs::path (DISKLORDZ_FACTORY_ROM_DIR);
#else
    return {};
#endif
}

bool loadLibraryFromDirectory (const fs::path& dir, RawRomLibrary& library)
{
    if (! fs::is_directory (dir))
        return false;

    int loaded = 0;
    for (const auto& entry : fs::directory_iterator (dir))
    {
        const auto fname = entry.path().filename().string();
        if (fname.rfind ("pcm_bank_", 0) != 0 || entry.path().extension() != ".dlrrom")
            continue;

        juce::FileInputStream in (juce::File (entry.path().string()));
        if (! in.openedOk())
            continue;

        const int size = static_cast<int> (in.getTotalLength());
        if (size <= 0)
            continue;

        juce::MemoryBlock block (static_cast<std::size_t> (size));
        if (in.read (block.getData(), size) != size)
            continue;

        if (library.loadBankEmbedded (block.getData(), block.getSize()))
            ++loaded;
    }
    return loaded > 0;
}

fs::path resourcesRomDirNextToModule (const juce::File& moduleFile)
{
    return fs::path (moduleFile.getSiblingFile ("Resources").getChildFile ("DisklordzRom").getFullPathName().toStdString());
}

} // namespace

bool loadEmbeddedFactoryRomLibrary (RawRomLibrary& library) noexcept
{
    library = RawRomLibrary{};

    const auto devDir = romDirectoryCandidates();
    if (! devDir.empty() && loadLibraryFromDirectory (devDir, library))
        return true;

    const auto moduleFile = getThisSharedModuleFile();
    const auto resDir = resourcesRomDirNextToModule (moduleFile);
    if (loadLibraryFromDirectory (resDir, library))
        return true;

    return false;
}

} // namespace disklordz::rompler::assets
