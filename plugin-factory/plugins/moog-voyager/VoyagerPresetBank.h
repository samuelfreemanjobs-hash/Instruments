#pragma once

#include <string_view>

class MoogVoyagerAudioProcessor;

namespace voyager
{
/** Moog / Voyager factory bank — 300 programs (see PRESETS.md). */
int getFactoryPresetCount() noexcept;
std::string_view getFactoryPresetName(int index) noexcept;
void applyFactoryPreset(MoogVoyagerAudioProcessor& processor, int index) noexcept;
} // namespace voyager
