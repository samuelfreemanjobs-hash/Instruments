#include "disklordz/WavWriter.h"

#include <algorithm>
#include <cmath>
#include <fstream>

namespace disklordz::synth
{

bool writeMonoWav24 (const std::string& path, std::span<const float> samples, std::uint32_t sampleRate)
{
    if (samples.empty() || sampleRate == 0)
        return false;

    const std::uint32_t numSamples = static_cast<std::uint32_t> (samples.size());
    const std::uint16_t channels = 1;
    const std::uint16_t bits = 24;
    const std::uint32_t byteRate = sampleRate * channels * (bits / 8);
    const std::uint16_t blockAlign = channels * (bits / 8);
    const std::uint32_t dataSize = numSamples * (bits / 8);
    const std::uint32_t riffSize = 36 + dataSize;

    std::ofstream out (path, std::ios::binary);
    if (! out)
        return false;

    out.write ("RIFF", 4);
    out.write (reinterpret_cast<const char*> (&riffSize), 4);
    out.write ("WAVE", 4);
    out.write ("fmt ", 4);
    const std::uint32_t fmtSize = 16;
    const std::uint16_t audioFormat = 1;
    out.write (reinterpret_cast<const char*> (&fmtSize), 4);
    out.write (reinterpret_cast<const char*> (&audioFormat), 2);
    out.write (reinterpret_cast<const char*> (&channels), 2);
    out.write (reinterpret_cast<const char*> (&sampleRate), 4);
    out.write (reinterpret_cast<const char*> (&byteRate), 4);
    out.write (reinterpret_cast<const char*> (&blockAlign), 2);
    out.write (reinterpret_cast<const char*> (&bits), 2);
    out.write ("data", 4);
    out.write (reinterpret_cast<const char*> (&dataSize), 4);

    for (float s : samples)
    {
        s = std::clamp (s, -1.0f, 1.0f);
        const int32_t scaled = static_cast<int32_t> (s * 8388607.0f);
        const unsigned char b0 = static_cast<unsigned char> (scaled & 0xff);
        const unsigned char b1 = static_cast<unsigned char> ((scaled >> 8) & 0xff);
        const unsigned char b2 = static_cast<unsigned char> ((scaled >> 16) & 0xff);
        out.put (static_cast<char> (b0));
        out.put (static_cast<char> (b1));
        out.put (static_cast<char> (b2));
    }

    return out.good();
}

} // namespace disklordz::synth
