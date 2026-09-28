#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>

namespace junovax::dsp
{
/** Lock-free mono peak capture for UI oscilloscope (audio thread writes). */
class ScopeFifo
{
public:
    static constexpr int kCapacity = 512;

    void reset() noexcept
    {
        writeIndex_.store (0, std::memory_order_relaxed);
    }

    void pushSample (float sample) noexcept
    {
        const int idx = writeIndex_.load (std::memory_order_relaxed);
        buffer_[static_cast<std::size_t> (idx % kCapacity)] = sample;
        writeIndex_.store (idx + 1, std::memory_order_release);
    }

    void copyRecent (float* dest, int numSamples) const noexcept
    {
        if (dest == nullptr || numSamples <= 0)
            return;

        const int write = writeIndex_.load (std::memory_order_acquire);
        const int available = std::min ({ numSamples, kCapacity, write });
        const int start = write - available;

        for (int i = 0; i < numSamples; ++i)
        {
            if (i < available)
                dest[i] = buffer_[static_cast<std::size_t> ((start + i) % kCapacity)];
            else
                dest[i] = 0.0f;
        }
    }

private:
    std::array<float, kCapacity> buffer_{};
    std::atomic<int> writeIndex_ { 0 };
};
} // namespace junovax::dsp
