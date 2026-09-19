#include "disklordz/content/ContentValidation.h"
#include "disklordz/content/ContentIds.h"

#include <cmath>

namespace disklordz::content
{

ValidationReport validateSampleAsset (const SampleAssetRecord& asset)
{
    ValidationReport r;
    if (! isValidContentId (asset.id, kSampleIdPrefix))
    {
        r.ok = false;
        r.message = "invalid sample id";
        return r;
    }
    if (asset.sampleRate < 8000 || asset.sampleRate > 192000)
    {
        r.ok = false;
        r.message = "unsupported sample rate";
        return r;
    }
    if (asset.peakDbfs > 0.1f)
    {
        r.ok = false;
        r.message = "peak suggests clipping";
        return r;
    }
    if (asset.loopEnabled && asset.loopEnd <= asset.loopStart)
    {
        r.ok = false;
        r.message = "invalid loop range";
        return r;
    }
    return r;
}

ValidationReport validatePatchRecord (const PatchRecord& patch)
{
    ValidationReport r;
    if (! isValidContentId (patch.id, kPatchIdPrefix))
    {
        r.ok = false;
        r.message = "invalid patch id";
        return r;
    }
    for (const auto& tid : patch.toneIds)
    {
        if (tid.empty())
            continue;
        if (! isValidContentId (tid, kToneIdPrefix))
        {
            r.ok = false;
            r.message = "invalid tone id in patch";
            return r;
        }
    }
    return r;
}

ValidationReport validatePackageManifest (const ContentManifest& manifest)
{
    ValidationReport r;
    std::string err;
    if (! validateContentManifest (manifest, err))
    {
        r.ok = false;
        r.message = err;
    }
    return r;
}

} // namespace disklordz::content
