#pragma once

#include "RenaissanceSurfaceMap.h"

#include <JuceHeader.h>

namespace renaissance
{
/** Simple per-pad sine voices — placeholder for your custom sampler/sequencer. */
class PadVoiceEngine
{
public:
    void prepare (double sampleRate);
    void render (juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    void triggerPad (int padIndex, float velocity);
    void setPadLevel (int padIndex, float level0to1);

    [[nodiscard]] float padLevel (int padIndex) const noexcept;

private:
    struct Voice
    {
        double phase = 0.0;
        double inc = 440.0;
        float env = 0.0f;
        bool active = false;
    };

    std::array<Voice, kNumPads> voices_ {};
    std::array<float, kNumPads> levels_ {};
    double sampleRate_ = 44100.0;
};

} // namespace renaissance
