#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace disklordz::synth
{

/** Write mono 32-bit float PCM as 24-bit WAV (offline tools). */
bool writeMonoWav24 (const std::string& path, std::span<const float> samples, std::uint32_t sampleRate);

} // namespace disklordz::synth
