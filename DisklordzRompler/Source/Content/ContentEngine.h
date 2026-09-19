#pragma once

#include "Assets/RawRomBank.h"

#include <string>
#include <vector>

namespace disklordz::rompler::content
{

/**
 * Mounts factory + expansion PCM packages (message thread only).
 * License / entitlement resolves to a list of contentIds — never consulted on audio thread.
 */
class ContentEngine final
{
public:
    bool mountFactory (const std::string& romDirectory) noexcept;
    bool mountExpansion (const std::string& expansionRoot, const std::string& contentId) noexcept;

    const assets::RawRomLibrary& getRomLibrary() const noexcept { return library_; }
    assets::RawRomLibrary& getRomLibrary() noexcept { return library_; }

    std::vector<std::string> getMountedContentIds() const noexcept { return mountedIds_; }

private:
    assets::RawRomLibrary library_;
    std::vector<std::string> mountedIds_;
};

} // namespace disklordz::rompler::content
