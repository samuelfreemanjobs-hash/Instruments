#pragma once

#include <string_view>

class MoogVoyagerAudioProcessor;

namespace voyager
{
/** Full Voyager-style factory bank (leads, basses, pads, keys, FX). */
int getFactoryPresetCount() noexcept;
std::string_view getFactoryPresetName(int index) noexcept;
void applyFactoryPreset(MoogVoyagerAudioProcessor& processor, int index) noexcept;
} // namespace voyager
