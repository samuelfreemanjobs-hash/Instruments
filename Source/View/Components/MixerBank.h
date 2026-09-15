#pragma once

#include <JuceHeader.h>
#include "MixerChannelStrip.h"

namespace vmpc::view
{
class MixerBank : public juce::Component
{
public:
    static constexpr int kChannels = 4;

    MixerBank();

    void resized() override;
    void meterUpdate(int channel, float peak);

    MixerChannelStrip& getStrip(int index) { return strips[static_cast<size_t>(index & 3)]; }

private:
    std::array<MixerChannelStrip, kChannels> strips {
        MixerChannelStrip { "DRUM" },
        MixerChannelStrip { "BASS" },
        MixerChannelStrip { "LEAD" },
        MixerChannelStrip { "FX" },
    };
};
} // namespace vmpc::view
