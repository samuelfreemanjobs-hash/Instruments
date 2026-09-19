#include "disklordz/WavReader.h"

#include <algorithm>
#include <cstring>
#include <fstream>

namespace disklordz::synth
{

namespace
{

bool readU32 (std::ifstream& in, std::uint32_t& v)
{
    in.read (reinterpret_cast<char*> (&v), 4);
    return in.good();
}

bool readU16 (std::ifstream& in, std::uint16_t& v)
{
    in.read (reinterpret_cast<char*> (&v), 2);
    return in.good();
}

} // namespace

bool readWavFile (const std::string& path, WavData& out, std::string& error)
{
    out = {};
    std::ifstream in (path, std::ios::binary);
    if (! in)
    {
        error = "cannot open wav";
        return false;
    }

    char riff[4];
    in.read (riff, 4);
    if (std::strncmp (riff, "RIFF", 4) != 0)
    {
        error = "not RIFF";
        return false;
    }

    std::uint32_t riffSize = 0;
    if (! readU32 (in, riffSize))
        return false;

    char wave[4];
    in.read (wave, 4);
    if (std::strncmp (wave, "WAVE", 4) != 0)
    {
        error = "not WAVE";
        return false;
    }

    std::uint16_t audioFormat = 0;
    std::uint16_t channels = 0;
    std::uint32_t sampleRate = 0;
    std::uint16_t bitsPerSample = 0;
    std::uint32_t dataSize = 0;
    std::streampos dataPos = 0;

    while (in && in.tellg() < static_cast<std::streampos> (8 + riffSize))
    {
        char chunkId[4];
        in.read (chunkId, 4);
        std::uint32_t chunkSize = 0;
        if (! readU32 (in, chunkSize))
            break;

        if (std::strncmp (chunkId, "fmt ", 4) == 0)
        {
            readU16 (in, audioFormat);
            readU16 (in, channels);
            readU32 (in, sampleRate);
            std::uint32_t byteRate = 0;
            readU32 (in, byteRate);
            std::uint16_t blockAlign = 0;
            readU16 (in, blockAlign);
            readU16 (in, bitsPerSample);
            if (chunkSize > 16)
                in.seekg (static_cast<std::streamoff> (chunkSize - 16), std::ios::cur);
        }
        else if (std::strncmp (chunkId, "data", 4) == 0)
        {
            dataSize = chunkSize;
            dataPos = in.tellg();
            in.seekg (static_cast<std::streamoff> (chunkSize), std::ios::cur);
        }
        else
        {
            in.seekg (static_cast<std::streamoff> (chunkSize + (chunkSize % 2)), std::ios::cur);
        }
    }

    if (audioFormat != 1 || dataSize == 0 || bitsPerSample == 0)
    {
        error = "unsupported wav format";
        return false;
    }

    in.clear();
    in.seekg (dataPos);

    const std::size_t frameCount = dataSize / (bitsPerSample / 8) / std::max<std::uint16_t> (1, channels);
    out.sampleRate = sampleRate;
    out.channels = channels;
    out.samples.resize (frameCount);

    for (std::size_t i = 0; i < frameCount; ++i)
    {
        float acc = 0.0f;
        for (std::uint16_t ch = 0; ch < channels; ++ch)
        {
            float s = 0.0f;
            if (bitsPerSample == 16)
            {
                std::int16_t v = 0;
                in.read (reinterpret_cast<char*> (&v), 2);
                s = static_cast<float> (v) / 32768.0f;
            }
            else if (bitsPerSample == 24)
            {
                unsigned char b[3];
                in.read (reinterpret_cast<char*> (b), 3);
                std::int32_t v = (b[0] | (b[1] << 8) | (b[2] << 16));
                if (v & 0x800000)
                    v |= ~0xffffff;
                s = static_cast<float> (v) / 8388608.0f;
            }
            else if (bitsPerSample == 32)
            {
                std::int32_t v = 0;
                in.read (reinterpret_cast<char*> (&v), 4);
                s = static_cast<float> (v) / 2147483648.0f;
            }
            else
            {
                error = "unsupported bit depth";
                return false;
            }
            if (ch == 0)
                acc = s;
        }
        out.samples[i] = acc;
    }

    return ! out.samples.empty();
}

} // namespace disklordz::synth
