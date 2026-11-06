#pragma once

#include <JuceHeader.h>

#include <vector>

namespace autosampler
{

struct Zone
{
    int rootNote = 60;
    juce::AudioBuffer<float> audio;
    double sampleRate = 44100.0;
    /** Inclusive loop points in samples; negative = no loop. */
    int loopStart = -1;
    int loopEnd = -1;

    bool hasLoop() const noexcept { return loopStart >= 0 && loopEnd > loopStart; }
};

/** One playable multisample preset (MPC Auto Sampler-style program). */
class MultisampleProgram
{
public:
    juce::String name { "Init" };

    void clear() noexcept { zones_.clear(); }

    bool empty() const noexcept { return zones_.empty(); }
    std::size_t zoneCount() const noexcept { return zones_.size(); }

    const std::vector<Zone>& zones() const noexcept { return zones_; }
    std::vector<Zone>& zones() noexcept { return zones_; }

    void addZone (Zone zone);
    void rebuildKeyRanges() noexcept;

    /** Find zone index for MIDI note, or -1. */
    int zoneIndexForNote (int midiNote) const noexcept;

    int lowKeyForZone (std::size_t index) const noexcept;
    int highKeyForZone (std::size_t index) const noexcept;

    juce::ValueTree toValueTree() const;
    bool loadFromValueTree (const juce::ValueTree& tree);

private:
    std::vector<Zone> zones_;
    std::vector<int> lowKeys_;
    std::vector<int> highKeys_;
};

} // namespace autosampler
