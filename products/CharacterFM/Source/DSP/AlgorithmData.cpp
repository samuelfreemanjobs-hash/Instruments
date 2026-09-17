#include "AlgorithmData.h"

#include <algorithm>

namespace characterfm::dsp
{
namespace
{

ModulationGraph buildStackToCarrier (std::uint8_t carrierOp, float feedbackOnCarrier = 0.0f)
{
    ModulationGraph g;
    for (int src = kNumOperators; src >= 2; --src)
        g.addEdge (static_cast<std::uint8_t> (src),
                   static_cast<std::uint8_t> (src - 1),
                   1.0f);
    g.carrierMask = static_cast<std::uint8_t> (1u << (carrierOp - 1));
    if (carrierOp >= 1 && carrierOp <= kNumOperators)
        g.feedback[static_cast<std::size_t> (carrierOp - 1)] = feedbackOnCarrier;
    return g;
}

ModulationGraph buildParallelCarriers (std::uint8_t mask)
{
    ModulationGraph g;
    g.carrierMask = mask;
    return g;
}

ModulationGraph buildCustom (std::initializer_list<ModulationEdge> edgeList,
                             std::uint8_t carrierMask,
                             std::array<float, kNumOperators> fb = {})
{
    ModulationGraph g;
    for (const auto& e : edgeList)
        g.addEdge (e.source, e.destination, e.amount);
    g.carrierMask = carrierMask;
    g.feedback = fb;
    return g;
}

std::array<ModulationGraph, 32> makeAllAlgorithms()
{
    std::array<ModulationGraph, 32> tables{};

    // Classic stacked modulators into OP1 (DX7 algorithms 1–4 family).
    tables[0] = buildStackToCarrier (1, 0.0f);
    tables[1] = buildStackToCarrier (1, 0.33f);
    tables[2] = buildCustom ({ { 6, 5, 1.f }, { 5, 4, 1.f }, { 4, 3, 1.f }, { 3, 1, 1.f }, { 2, 1, 1.f } },
                             1u << 0);
    tables[3] = buildCustom ({ { 6, 5, 1.f }, { 5, 4, 1.f }, { 4, 2, 1.f }, { 3, 2, 1.f }, { 2, 1, 1.f } },
                             1u << 0);

    // Branching trees (approximate common DX7 layouts).
    tables[4] = buildCustom ({ { 6, 5, 1.f }, { 5, 3, 1.f }, { 4, 2, 1.f }, { 3, 1, 1.f }, { 2, 1, 1.f } },
                             1u << 0);
    tables[5] = buildCustom ({ { 6, 4, 1.f }, { 5, 3, 1.f }, { 4, 2, 1.f }, { 3, 2, 1.f }, { 2, 1, 1.f } },
                             (1u << 0) | (1u << 2));
    tables[6] = buildCustom ({ { 6, 5, 1.f }, { 5, 2, 1.f }, { 4, 2, 1.f }, { 3, 1, 1.f } },
                             (1u << 0) | (1u << 1));
    tables[7] = buildCustom ({ { 6, 3, 1.f }, { 5, 3, 1.f }, { 4, 2, 1.f }, { 3, 1, 1.f } },
                             1u << 0);

    for (int i = 8; i < 16; ++i)
        tables[static_cast<std::size_t> (i)] = tables[static_cast<std::size_t> (i % 8)];

    // Later DX7 algorithms often use multiple carriers.
    tables[16] = buildParallelCarriers ((1u << 0) | (1u << 3) | (1u << 5));
    tables[17] = buildCustom ({ { 6, 5, 1.f }, { 4, 3, 1.f }, { 2, 1, 1.f } },
                              (1u << 0) | (1u << 2) | (1u << 4));
    tables[18] = buildCustom ({ { 6, 4, 1.f }, { 5, 2, 1.f }, { 3, 1, 1.f } },
                              (1u << 0) | (1u << 1));
    tables[19] = buildStackToCarrier (2, 0.0f);

    for (int i = 20; i < 31; ++i)
        tables[static_cast<std::size_t> (i)] = tables[static_cast<std::size_t> (16 + (i % 4))];

    tables[31] = buildParallelCarriers (0x3Fu); // all ops as carriers (algo 32 style)

    return tables;
}

const std::array<ModulationGraph, 32> kAlgorithms = makeAllAlgorithms();

} // namespace

const ModulationGraph& getAlgorithmGraph (int algorithmIndex) noexcept
{
    const int idx = std::clamp (algorithmIndex, 0, 31);
    return kAlgorithms[static_cast<std::size_t> (idx)];
}

} // namespace characterfm::dsp
