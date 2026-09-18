#pragma once

#include "Engine/VoiceManager.h"
#include "Engine/SynthParams.h"

namespace aftrdark::engine
{

class SynthEngine final
{
public:
    void prepare (double sampleRate, int /*samplesPerBlock*/) noexcept
    {
        voices_.prepare (sampleRate);
    }

    void reset() noexcept
    {
        voices_.reset();
    }

    void setParams (const SynthParams& p) noexcept
    {
        voices_.setParams (p);
    }

    void noteOn (int note, float velocity) noexcept
    {
        voices_.noteOn (note, velocity);
    }

    void noteOff (int note) noexcept
    {
        voices_.noteOff (note);
    }

    void render (float* outL, float* outR, int numSamples) noexcept
    {
        voices_.render (outL, outR, numSamples);
    }

private:
    VoiceManager voices_{};
};

} // namespace aftrdark::engine
