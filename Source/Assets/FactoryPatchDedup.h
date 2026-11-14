#pragma once

#include "FactoryPatchLibrary.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jdupgraded::assets::detail
{

std::uint64_t fingerprintPatch (const FactoryPatch& patch) noexcept;

/** Ensures unique display names and unique tone fingerprints across factory storage. */
void deduplicatePatchStore (std::vector<std::string>& names, std::vector<FactoryPatch>& patches);

} // namespace jdupgraded::assets::detail
