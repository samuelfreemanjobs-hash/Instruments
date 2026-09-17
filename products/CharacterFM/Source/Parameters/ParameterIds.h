#pragma once

namespace characterfm::params
{

inline constexpr const char* masterGain = "masterGain";
inline constexpr const char* algorithm = "algorithm";

inline constexpr const char* opRatio (int op1Based)
{
    switch (op1Based)
    {
        case 1: return "op1Ratio";
        case 2: return "op2Ratio";
        case 3: return "op3Ratio";
        case 4: return "op4Ratio";
        case 5: return "op5Ratio";
        case 6: return "op6Ratio";
        default: return "op1Ratio";
    }
}

inline constexpr const char* opLevel (int op1Based)
{
    switch (op1Based)
    {
        case 1: return "op1Level";
        case 2: return "op2Level";
        case 3: return "op3Level";
        case 4: return "op4Level";
        case 5: return "op5Level";
        case 6: return "op6Level";
        default: return "op1Level";
    }
}

} // namespace characterfm::params
