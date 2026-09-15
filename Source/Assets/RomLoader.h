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
        if (embeddedData != nullptr && embeddedSize > 0)
            bank_.loadEmbedded (embeddedData, embeddedSize);
    }

    bool loadUserRomFile (const std::string& path)
    {
        return bank_.loadFromFile (path);
    }

    const RomBank& getBank() const noexcept { return bank_; }
    RomBank& getBank() noexcept { return bank_; }

private:
    RomBank bank_;
};

} // namespace jdupgraded::assets
