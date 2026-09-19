#include "disklordz/content/ExpansionManifest.h"
#include "disklordz/content/ContentIds.h"

#include <fstream>
#include <regex>

namespace disklordz::content
{

namespace
{

std::string extractJsonString (const std::string& body, const std::string& key)
{
    const std::regex re ("\"" + key + "\"\\s*:\\s*\"([^\"]+)\"");
    std::smatch m;
    if (std::regex_search (body, m, re) && m.size() > 1)
        return m[1].str();
    return {};
}

std::uint32_t extractJsonUint (const std::string& body, const std::string& key)
{
    const std::regex re ("\"" + key + "\"\\s*:\\s*([0-9]+)");
    std::smatch m;
    if (std::regex_search (body, m, re) && m.size() > 1)
        return static_cast<std::uint32_t> (std::stoul (m[1].str()));
    return 0;
}

} // namespace

bool loadContentManifestJson (const std::string& path, ContentManifest& out, std::string& error)
{
    out = {};
    std::ifstream in (path);
    if (! in)
    {
        error = "cannot open manifest";
        return false;
    }
    std::string body ((std::istreambuf_iterator<char> (in)), std::istreambuf_iterator<char> ());

    out.contentId = extractJsonString (body, "contentId");
    out.name = extractJsonString (body, "name");
    out.version = extractJsonString (body, "version");
    out.engineVersion = extractJsonString (body, "engineVersion");
    out.contentVersion = extractJsonUint (body, "contentVersion");
    out.schemaVersion = extractJsonUint (body, "schemaVersion");
    out.manifestVersion = extractJsonUint (body, "manifestVersion");
    out.presetCatalogPath = extractJsonString (body, "presetCatalog");

    if (out.contentId.empty() || out.version.empty())
    {
        error = "manifest missing contentId or version";
        return false;
    }
    return true;
}

bool validateContentManifest (const ContentManifest& manifest, std::string& error)
{
    const bool isFactory = manifest.contentId == std::string (kFactoryPackageId);
    const bool isExp = isValidContentId (manifest.contentId, kExpansionIdPrefix);
    const bool isPkg = isValidContentId (manifest.contentId, kPackageIdPrefix);
    if (! isFactory && ! isExp && ! isPkg)
    {
        error = "contentId must be DL_PKG_* or DL_EXP_*";
        return false;
    }
    if (manifest.manifestVersion == 0 || manifest.schemaVersion == 0)
    {
        error = "manifestVersion and schemaVersion required";
        return false;
    }
    return true;
}

} // namespace disklordz::content
