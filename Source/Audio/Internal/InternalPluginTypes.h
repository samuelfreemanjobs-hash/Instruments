#pragma once

#include <JuceHeader.h>
#include <vector>

namespace vmpc::audio::internal
{
enum class MixPluginId
{
    Gain = 0,
    Pan,
    Eq3,
    Compressor,
    Limiter
};

struct MixPluginInfo
{
    MixPluginId id;
    juce::String displayName;
    juce::String description;
};

inline const std::vector<MixPluginInfo>& allMixPlugins()
{
    static const std::vector<MixPluginInfo> list {
        { MixPluginId::Gain, "VMPC Gain", "Trim level (dB)" },
        { MixPluginId::Pan, "VMPC Pan", "Constant-power stereo pan" },
        { MixPluginId::Eq3, "VMPC EQ-3", "Low / mid / high tone shaping" },
        { MixPluginId::Compressor, "VMPC Compressor", "Dynamics control" },
        { MixPluginId::Limiter, "VMPC Limiter", "Brickwall safety limiter" },
    };
    return list;
}

std::unique_ptr<juce::AudioProcessor> createMixPlugin(MixPluginId id);
} // namespace vmpc::audio::internal
