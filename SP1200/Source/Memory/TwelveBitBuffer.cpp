#include "TwelveBitBuffer.h"

#include <algorithm>
#include <cmath>

namespace sp1200
{
namespace
{
constexpr float kScale = 1.0f / 2048.0f;

std::uint16_t floatTo12 (float x) noexcept
{
    const float clamped = std::clamp (x, -1.0f, 1.0f);
    const int scaled = static_cast<int> (std::lround (clamped * 2047.0f));
    return static_cast<std::uint16_t> (std::clamp (scaled + 2048, 0, 4095));
}
} // namespace

void TwelveBitBuffer::resize (std::size_t numSamples)
{
    samples_.resize (numSamples, 2048);
}

void TwelveBitBuffer::clear() noexcept
{
    std::fill (samples_.begin(), samples_.end(), static_cast<std::uint16_t> (2048));
}

void TwelveBitBuffer::quantizeFromFloat (const float* src, std::size_t count)
{
    resize (count);
    for (std::size_t i = 0; i < count; ++i)
        samples_[i] = floatTo12 (src[i]);
}

void TwelveBitBuffer::writeToFloat (float* dest, std::size_t count) const
{
    const auto n = std::min (count, samples_.size());
    for (std::size_t i = 0; i < n; ++i)
        dest[i] = (static_cast<float> (samples_[i]) - 2048.0f) * kScale;
}

std::uint16_t TwelveBitBuffer::getSample (std::size_t index) const
{
    return index < samples_.size() ? samples_[index] : 2048;
}

void TwelveBitBuffer::setSample (std::size_t index, std::uint16_t value12)
{
    if (index < samples_.size())
        samples_[index] = static_cast<std::uint16_t> (value12 & 4095u);
}

std::vector<std::uint8_t> TwelveBitBuffer::pack() const
{
    std::vector<std::uint8_t> out;
    const auto n = samples_.size();
    out.resize ((n * 3 + 1) / 2);
    std::size_t o = 0;
    for (std::size_t i = 0; i + 1 < n; i += 2)
    {
        const auto a = samples_[i] & 4095u;
        const auto b = samples_[i + 1] & 4095u;
        out[o++] = static_cast<std::uint8_t> ((a >> 4) & 0xff);
        out[o++] = static_cast<std::uint8_t> (((a & 0xf) << 4) | ((b >> 8) & 0xf));
        out[o++] = static_cast<std::uint8_t> (b & 0xff);
    }
    if (n % 2 == 1)
    {
        const auto a = samples_[n - 1] & 4095u;
        if (o < out.size())
            out[o++] = static_cast<std::uint8_t> ((a >> 4) & 0xff);
        if (o < out.size())
            out[o++] = static_cast<std::uint8_t> ((a & 0xf) << 4);
    }
    return out;
}

void TwelveBitBuffer::unpack (const std::uint8_t* data, std::size_t byteCount, std::size_t numSamples)
{
    resize (numSamples);
    std::size_t o = 0;
    for (std::size_t i = 0; i + 1 < numSamples; i += 2)
    {
        if (o + 2 >= byteCount)
            break;
        const auto b0 = data[o++];
        const auto b1 = data[o++];
        const auto b2 = data[o++];
        samples_[i] = static_cast<std::uint16_t> ((b0 << 4) | (b1 >> 4));
        samples_[i + 1] = static_cast<std::uint16_t> (((b1 & 0xf) << 8) | b2);
    }
    if (numSamples % 2 == 1 && o + 1 <= byteCount)
    {
        const auto b0 = data[o++];
        const auto b1 = data[o < byteCount ? o : o - 1];
        samples_[numSamples - 1] = static_cast<std::uint16_t> ((b0 << 4) | (b1 >> 4));
    }
}

} // namespace sp1200
