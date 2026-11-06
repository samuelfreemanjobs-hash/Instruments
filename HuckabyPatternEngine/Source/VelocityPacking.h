#pragma once

#include "TrackModel.h"
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace huckaby::velpack
{

inline void unpackInto(std::array<std::atomic<uint64_t>, 4>& packs, std::array<float, kMaxSteps>& out) noexcept
{
    uint64_t words[4] {
        packs[0].load(std::memory_order_relaxed),
        packs[1].load(std::memory_order_relaxed),
        packs[2].load(std::memory_order_relaxed),
        packs[3].load(std::memory_order_relaxed),
    };

    for (int s = 0; s < kMaxSteps; ++s)
    {
        const int word = s / 16;
        const int shift = (s % 16) * 4;
        const int nibble = static_cast<int>((words[word] >> shift) & 0xFull);
        const float g = static_cast<float>(nibble + 1) / 16.0f;
        out[static_cast<size_t>(s)] = juce::jlimit(0.05f, 1.0f, g);
    }
}

inline void packFrom(const std::array<float, kMaxSteps>& v, std::array<std::atomic<uint64_t>, 4>& packs) noexcept
{
    uint64_t w[4] {};

    for (int s = 0; s < kMaxSteps; ++s)
    {
        const int q = juce::jlimit(0, 15, juce::roundToInt(v[static_cast<size_t>(s)] * 15.0f));
        const int word = s / 16;
        const int shift = (s % 16) * 4;
        w[word] |= (static_cast<uint64_t>(q) & 0xFull) << shift;
    }

    for (int i = 0; i < 4; ++i)
        packs[static_cast<size_t>(i)].store(w[static_cast<size_t>(i)], std::memory_order_relaxed);
}

inline void fillUniform(std::array<float, kMaxSteps>& v, float level) noexcept
{
    for (auto& x : v)
        x = level;
}

inline void applyBitsToVelocities(uint64_t bits, std::array<float, kMaxSteps>& v, float onLevel = 0.88f) noexcept
{
    for (int s = 0; s < kMaxSteps; ++s)
        v[static_cast<size_t>(s)] = ((bits >> s) & 1ull) != 0 ? onLevel : 0.5f;
}

} // namespace huckaby::velpack
