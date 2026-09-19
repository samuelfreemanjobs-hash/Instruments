#pragma once

#include <string_view>

namespace SynthParamIDs
{
inline constexpr std::string_view outputGainDb { "outputGainDb" };

inline constexpr std::string_view osc1Level { "osc1Level" };
inline constexpr std::string_view osc1Octave { "osc1Octave" };
inline constexpr std::string_view osc1Fine { "osc1Fine" };
inline constexpr std::string_view osc1Wave { "osc1Wave" };

inline constexpr std::string_view osc2Level { "osc2Level" };
inline constexpr std::string_view osc2Octave { "osc2Octave" };
inline constexpr std::string_view osc2Fine { "osc2Fine" };
inline constexpr std::string_view osc2Wave { "osc2Wave" };

inline constexpr std::string_view osc3Level { "osc3Level" };
inline constexpr std::string_view osc3Octave { "osc3Octave" };
inline constexpr std::string_view osc3Fine { "osc3Fine" };
inline constexpr std::string_view osc3Wave { "osc3Wave" };

inline constexpr std::string_view noiseLevel { "noiseLevel" };

inline constexpr std::string_view filterCutoff { "filterCutoff" };
inline constexpr std::string_view filterResonance { "filterResonance" };
inline constexpr std::string_view filterDrive { "filterDrive" };
inline constexpr std::string_view filterEnvAmount { "filterEnvAmount" };
inline constexpr std::string_view filterKeyTrack { "filterKeyTrack" };

inline constexpr std::string_view filterAttack { "filterAttack" };
inline constexpr std::string_view filterDecay { "filterDecay" };
inline constexpr std::string_view filterSustain { "filterSustain" };
inline constexpr std::string_view filterRelease { "filterRelease" };

inline constexpr std::string_view ampAttack { "ampAttack" };
inline constexpr std::string_view ampDecay { "ampDecay" };
inline constexpr std::string_view ampSustain { "ampSustain" };
inline constexpr std::string_view ampRelease { "ampRelease" };

inline constexpr std::string_view lfoRate { "lfoRate" };
inline constexpr std::string_view lfoToPitch { "lfoToPitch" };
inline constexpr std::string_view lfoToFilter { "lfoToFilter" };

inline constexpr std::string_view glideTime { "glideTime" };

inline constexpr std::string_view mixerDrive { "mixerDrive" };
} // namespace SynthParamIDs
