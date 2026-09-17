#include "ModulationGraph.h"

namespace characterfm::dsp
{

void ModulationGraph::clear() noexcept
{
    edgeCount = 0;
    feedback.fill (0.0f);
    carrierMask = 0;
    for (auto& e : edges)
        e = {};
}

bool ModulationGraph::addEdge (std::uint8_t source, std::uint8_t destination, float amount) noexcept
{
    if (source < 1 || source > kNumOperators || destination < 1 || destination > kNumOperators)
        return false;
    if (edgeCount >= kMaxEdges)
        return false;

    edges[static_cast<std::size_t> (edgeCount++)] = { source, destination, amount };
    return true;
}

bool ModulationGraph::validate() const noexcept
{
    for (int i = 0; i < edgeCount; ++i)
    {
        const auto& e = edges[static_cast<std::size_t> (i)];
        if (e.source < 1 || e.source > kNumOperators || e.destination < 1 || e.destination > kNumOperators)
            return false;
        if (e.amount < 0.0f || e.amount > 4.0f)
            return false;
    }
    return carrierMask != 0;
}

} // namespace characterfm::dsp
