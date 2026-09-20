#pragma once

#include <cstdint>
#include <vector>

namespace sp1200
{
/** Mono sample memory stored as 12-bit values (0..4095). */
class TwelveBitBuffer
{
public:
    void resize (std::size_t numSamples);
    [[nodiscard]] std::size_t size() const noexcept { return samples_.size(); }
    [[nodiscard]] bool empty() const noexcept { return samples_.empty(); }

    void clear() noexcept;
    void quantizeFromFloat (const float* src, std::size_t count);
    void writeToFloat (float* dest, std::size_t count) const;

    [[nodiscard]] std::uint16_t getSample (std::size_t index) const;
    void setSample (std::size_t index, std::uint16_t value12);

    /** Pack for project embed (2 samples → 3 bytes). */
    [[nodiscard]] std::vector<std::uint8_t> pack() const;
    void unpack (const std::uint8_t* data, std::size_t byteCount, std::size_t numSamples);

private:
    std::vector<std::uint16_t> samples_;
};

} // namespace sp1200
