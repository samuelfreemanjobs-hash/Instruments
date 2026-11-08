#pragma once

#include "../SP1200Constants.h"

namespace sp1200
{
enum class VoiceFilterRole
{
    flatPass,
    hiTrim
};

/** Spec §2.5: CH3–4 (and 11–12) hi-filter; others flat into bus SSM2044. */
inline VoiceFilterRole filterRoleForPad (int padIndex) noexcept
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return VoiceFilterRole::flatPass;
    const int ch = padIndex % 8;
    if (ch >= 2 && ch <= 3)
        return VoiceFilterRole::hiTrim;
    return VoiceFilterRole::flatPass;
}

} // namespace sp1200
