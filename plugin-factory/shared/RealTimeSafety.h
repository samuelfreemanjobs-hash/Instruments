#pragma once

#include <cassert>

/** Small helpers for audio-thread code in factory plugins. */
namespace pf::rt
{
template <typename T>
constexpr T* assumeNotNull(T* ptr) noexcept
{
    assert(ptr != nullptr);
    return ptr;
}
} // namespace pf::rt
