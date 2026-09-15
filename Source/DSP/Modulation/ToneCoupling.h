#pragma once

#include <cstdint>

namespace jdupgraded::dsp
{

enum class ToneCouplingMode : std::uint8_t
{
    independent = 0,
    ringPair01,
    ringPair23,
    crossModPair01,
    crossModPair23,
    hardSyncPair01,
    hardSyncPair23
};

} // namespace jdupgraded::dsp
