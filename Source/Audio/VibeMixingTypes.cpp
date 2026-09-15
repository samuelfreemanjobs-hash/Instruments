#include "VibeMixingTypes.h"
#include "MixConsole.h"

namespace resonance::audio
{
juce::String VibeMixTarget::displayName() const
{
    switch (kind)
    {
        case VibeMixTargetKind::Master:
            return "Master bus";
        case VibeMixTargetKind::Drums:
            return "Drums";
        case VibeMixTargetKind::Vocals:
            return "Vocals";
        case VibeMixTargetKind::Synths:
            return "Synths";
        case VibeMixTargetKind::FullMix:
            return "Full mix";
        case VibeMixTargetKind::Channel:
            return "Channel " + juce::String(channelIndex + 1);
    }
    return "Master bus";
}

bool VibeMixTarget::usesMasterBus() const noexcept
{
    return kind == VibeMixTargetKind::Master || kind == VibeMixTargetKind::FullMix;
}

int VibeMixTarget::resolvedChannelIndex() const noexcept
{
    switch (kind)
    {
        case VibeMixTargetKind::Drums:
            return 0;
        case VibeMixTargetKind::Vocals:
            return 1;
        case VibeMixTargetKind::Synths:
            return 2;
        case VibeMixTargetKind::Channel:
            return juce::jlimit(0, MixConsole::kNumChannels - 1, channelIndex);
        default:
            return 0;
    }
}
} // namespace resonance::audio
