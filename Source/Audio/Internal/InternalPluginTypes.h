#pragma once

#include <JuceHeader.h>
#include <vector>

namespace resonance::audio::internal
{
enum class MixPluginId
{
    Gain = 0,
    Pan,
    Eq3,
    Compressor,
    Limiter,
    Hpf,
    DeEsser,
    StereoWidth,
    TapeSat
};

inline juce::String mixPluginIdToString(MixPluginId id)
{
    switch (id)
    {
        case MixPluginId::Gain:
            return "Gain";
        case MixPluginId::Pan:
            return "Pan";
        case MixPluginId::Eq3:
            return "Eq3";
        case MixPluginId::Compressor:
            return "Compressor";
        case MixPluginId::Limiter:
            return "Limiter";
        case MixPluginId::Hpf:
            return "Hpf";
        case MixPluginId::DeEsser:
            return "DeEsser";
        case MixPluginId::StereoWidth:
            return "StereoWidth";
        case MixPluginId::TapeSat:
            return "TapeSat";
    }
    return {};
}

inline MixPluginId mixPluginIdFromString(const juce::String& text)
{
    if (text == "Gain")
        return MixPluginId::Gain;
    if (text == "Pan")
        return MixPluginId::Pan;
    if (text == "Eq3")
        return MixPluginId::Eq3;
    if (text == "Compressor")
        return MixPluginId::Compressor;
    if (text == "Limiter")
        return MixPluginId::Limiter;
    if (text == "Hpf")
        return MixPluginId::Hpf;
    if (text == "DeEsser")
        return MixPluginId::DeEsser;
    if (text == "StereoWidth")
        return MixPluginId::StereoWidth;
    if (text == "TapeSat")
        return MixPluginId::TapeSat;
    return MixPluginId::Gain;
}

struct MixPluginInfo
{
    MixPluginId id;
    juce::String displayName;
    juce::String description;
};

inline const std::vector<MixPluginInfo>& allMixPlugins()
{
    static const std::vector<MixPluginInfo> list {
        { MixPluginId::Gain, "Resonance Gain", "Trim level (dB)" },
        { MixPluginId::Pan, "Resonance Pan", "Constant-power stereo pan" },
        { MixPluginId::Eq3, "Resonance EQ-3", "Low / mid / high tone shaping" },
        { MixPluginId::Compressor, "Resonance Compressor", "Dynamics control" },
        { MixPluginId::Limiter, "Resonance Limiter", "Brickwall safety limiter" },
        { MixPluginId::Hpf, "Resonance HPF", "High-pass filter (mud / rumble)" },
        { MixPluginId::DeEsser, "Resonance De-Esser", "Sibilance control" },
        { MixPluginId::StereoWidth, "Resonance Width", "Stereo image (M/S)" },
        { MixPluginId::TapeSat, "Resonance Tape", "Warm saturation" },
    };
    return list;
}

std::unique_ptr<juce::AudioProcessor> createMixPlugin(MixPluginId id);
} // namespace resonance::audio::internal
