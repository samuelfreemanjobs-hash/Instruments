#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::content
{

struct ContentManifest final
{
    std::string contentId;
    std::string name;
    std::string version;
    std::string engineVersion;
    std::uint32_t contentVersion = 1;
    std::uint32_t schemaVersion = 1;
    std::uint32_t manifestVersion = 1;
    std::vector<std::string> romCards;
    std::string presetCatalogPath;
};

/** Parse minimal fields from manifest.json (hand-rolled, no external JSON dep). */
bool loadContentManifestJson (const std::string& path, ContentManifest& out, std::string& error);

bool validateContentManifest (const ContentManifest& manifest, std::string& error);

} // namespace disklordz::content
