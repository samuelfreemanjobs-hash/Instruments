#pragma once

#include <string_view>

namespace prophetrev2::presets
{

/** Curated factory programs for default browsing (expand toward 50–100). */
bool isFeaturedFactoryPreset (std::string_view category, std::string_view name) noexcept;

int countFeaturedInBank() noexcept;

} // namespace prophetrev2::presets
