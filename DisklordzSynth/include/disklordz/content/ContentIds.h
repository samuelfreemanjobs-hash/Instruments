#pragma once

#include <cstdint>
#include <string_view>

namespace disklordz::content
{

/** Prefixes — filenames are not identity. */
inline constexpr std::string_view kSampleIdPrefix = "DL_SAMPLE_";
inline constexpr std::string_view kMultisampleIdPrefix = "DL_MULTI_";
inline constexpr std::string_view kToneIdPrefix = "DL_TONE_";
inline constexpr std::string_view kPatchIdPrefix = "DL_PATCH_";
inline constexpr std::string_view kExpansionIdPrefix = "DL_EXP_";
inline constexpr std::string_view kPackageIdPrefix = "DL_PKG_";

inline constexpr std::string_view kFactoryPackageId = "DL_PKG_FACTORY";

bool isValidContentId (std::string_view id, std::string_view expectedPrefix) noexcept;

} // namespace disklordz::content
