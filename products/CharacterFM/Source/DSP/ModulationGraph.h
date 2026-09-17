#pragma once

#include <array>
#include <cstdint>

namespace characterfm::dsp
{

constexpr int kNumOperators = 6;
constexpr int kMaxEdges = 12;

struct ModulationEdge
{
    std::uint8_t source = 0; // 1..6
    std::uint8_t destination = 0;
    float amount = 1.0f;
};

struct ModulationGraph
{
    std::array<ModulationEdge, kMaxEdges> edges{};
    int edgeCount = 0;
    std::array<float, kNumOperators> feedback{};
    std::uint8_t carrierMask = 0; // bit i => op (i+1) summed to output

    void clear() noexcept;
    bool addEdge (std::uint8_t source, std::uint8_t destination, float amount) noexcept;
    bool validate() const noexcept;
};

} // namespace characterfm::dsp
