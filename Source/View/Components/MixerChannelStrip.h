#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
/** Fader, mute/solo, EQ curve sketch, meter ladder (Phase 2/4). */
class MixerChannelStrip : public juce::Component
{
public:
    MixerChannelStrip(const juce::String& channelName);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void meterUpdate(float peak);

    juce::Slider& getFader() { return fader; }

private:
    juce::String name;
    juce::Slider fader;
    juce::TextButton muteBtn { "M" };
    juce::TextButton soloBtn { "S" };
    float meterPeak = 0.0f;
};
} // namespace vmpc::view
