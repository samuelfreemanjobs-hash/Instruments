#include "MixerBank.h"

namespace vmpc::view
{
MixerBank::MixerBank()
{
    for (auto& strip : strips)
        addAndMakeVisible(strip);
}

void MixerBank::resized()
{
    auto area = getLocalBounds().reduced(2);
    const int w = area.getWidth() / kChannels;
    for (auto& strip : strips)
        strip.setBounds(area.removeFromLeft(w).reduced(1));
}

void MixerBank::meterUpdate(int channel, float peak)
{
    getStrip(channel).meterUpdate(peak);
}
} // namespace vmpc::view
