#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::synth
{

struct WavData final
{
    std::vector<float> samples;
    std::uint32_t sampleRate = 48000;
    std::uint16_t channels = 1;
};

/** Read mono or first channel of PCM WAV (16/24/32-bit integer PCM). */
bool readWavFile (const std::string& path, WavData& out, std::string& error);

} // namespace disklordz::synth
