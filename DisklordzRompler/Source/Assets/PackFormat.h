#pragma once

#include "disklordz/DlromPackFormat.h"

namespace disklordz::rompler::assets
{

using PackHeader = disklordz::dlrom::PackHeader;
using SampleRegion = disklordz::dlrom::SampleRegion;
using SampleCategory = disklordz::dlrom::SampleCategory;

inline constexpr std::uint32_t kPackVersion = disklordz::dlrom::kPackVersion;
inline constexpr std::uint32_t kMaxToneLayers = disklordz::dlrom::kMaxToneLayers;

} // namespace disklordz::rompler::assets
