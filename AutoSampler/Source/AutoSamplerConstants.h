#pragma once

namespace autosampler
{
/** In-plugin host program slots (was 16; +16 for 32 total). */
constexpr int kBankSlots = 32;

/** Max source VST3 plugins in one automated mega-bank job. */
constexpr int kMaxSourcePlugins = 26;
} // namespace autosampler
