#pragma once

#include "disklordz/content/ExpansionManifest.h"
#include "disklordz/content/PatchRecord.h"
#include "disklordz/content/SampleAssetRecord.h"

#include <string>

namespace disklordz::content
{

struct ValidationReport final
{
    bool ok = true;
    std::string message;
};

ValidationReport validateSampleAsset (const SampleAssetRecord& asset);
ValidationReport validatePatchRecord (const PatchRecord& patch);
ValidationReport validatePackageManifest (const ContentManifest& manifest);

} // namespace disklordz::content
