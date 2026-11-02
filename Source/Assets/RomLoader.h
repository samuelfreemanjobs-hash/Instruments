#pragma once

#include "RomBank.h"

#include <string>

namespace jdupgraded::assets
{

/** Loads the factory cleanroom ROM; optional user path overrides for development. */
class RomLoader final
{
public:
    void loadFactoryRom (const void* embeddedData, std::size_t embeddedSize) noexcept
    {
        factoryEmbedded_ = (embeddedData != nullptr && embeddedSize > 0);
        factoryData_ = embeddedData;
        factorySize_ = embeddedSize;

        if (factoryEmbedded_)
            bank_.loadEmbedded (embeddedData, embeddedSize);

        usingUserRom_ = false;
        userPath_.clear();
        lastUserLoadError_.clear();
    }

    /** Loads only JDUPGROM-format files; leaves factory ROM intact on failure. */
    bool loadUserRomFile (const std::string& path)
    {
        RomBank trial;
        if (! trial.loadFromFile (path))
        {
            lastUserLoadError_ = trial.getLoadError() != nullptr ? trial.getLoadError() : "Unknown ROM error";
            return false;
        }

        bank_ = std::move (trial);
        usingUserRom_ = true;
        userPath_ = path;
        lastUserLoadError_.clear();
        return true;
    }

    bool isUsingUserRom() const noexcept { return usingUserRom_; }
    const std::string& getUserRomPath() const noexcept { return userPath_; }
    const char* getLastUserLoadError() const noexcept { return lastUserLoadError_.c_str(); }

    std::string getSourceDescription() const
    {
        if (usingUserRom_)
            return "User JDUPGROM: " + userPath_;

        if (factoryEmbedded_ && bank_.isLoaded())
            return "Embedded clean-room ROM";

        return "ROM not loaded";
    }

    const RomBank& getBank() const noexcept { return bank_; }
    RomBank& getBank() noexcept { return bank_; }

private:
    RomBank bank_;
    const void* factoryData_ = nullptr;
    std::size_t factorySize_ = 0;
    bool factoryEmbedded_ = false;
    bool usingUserRom_ = false;
    std::string userPath_;
    std::string lastUserLoadError_;
};

} // namespace jdupgraded::assets
