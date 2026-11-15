#pragma once

#include "MultisampleProgram.h"

#include <JuceHeader.h>

#include <array>

namespace autosampler
{

class SamplerEngine
{
public:
    static constexpr int kMaxVoices = 32;

    void prepare (double sampleRate, int maxBlockSize);
    void reset() noexcept;

    void setProgram (const MultisampleProgram* program) noexcept { program_ = program; }
    void setOutputGainDb (float db) noexcept { outputGainDb_ = db; }

    void process (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);

private:
    struct Voice
    {
        bool active = false;
        int zoneIndex = -1;
        double position = 0.0;
        float gain = 1.0f;
        int note = 60;
        int crossfadeRemaining = 0;
        int crossfadeTotal = 0;
    };

    void startVoice (int note, float velocity);
    void stopVoice (int note);
    float renderSample (const Voice& v) const noexcept;

    const MultisampleProgram* program_ = nullptr;
    double sampleRate_ = 44100.0;
    float outputGainDb_ = 0.0f;
    std::array<Voice, kMaxVoices> voices_{};
};

} // namespace autosampler
