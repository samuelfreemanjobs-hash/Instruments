#pragma once

#include <JuceHeader.h>

#include <vector>

namespace autosampler
{

struct VelocityLayer
{
    int velStart = 0;
    int velEnd = 127;
    juce::AudioBuffer<float> audio;
    int loopStart = -1;
    int loopEnd = -1;
    float loopConfidence = 0.0f;

    bool hasLoop() const noexcept { return loopStart >= 0 && loopEnd > loopStart && loopConfidence >= 0.35f; }
};

struct Zone
{
    int rootNote = 60;
    juce::AudioBuffer<float> audio;
    double sampleRate = 44100.0;
    int loopStart = -1;
    int loopEnd = -1;
    float loopConfidence = 0.0f;
    std::vector<VelocityLayer> velocityLayers;

    void syncPrimaryFromLayers() noexcept;
    void addVelocityLayer (VelocityLayer layer);
    int velocityLayerCount() const noexcept;
    std::vector<VelocityLayer> velocityLayersForExport() const;

    bool hasLoop() const noexcept
    {
        if (! velocityLayers.empty())
        {
            for (const auto& layer : velocityLayers)
                if (layer.hasLoop())
                    return true;
        }
        return loopStart >= 0 && loopEnd > loopStart && loopConfidence >= 0.35f;
    }
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

    void reprocessAllZones (const struct PostProcessConfig& config) noexcept;

    int zoneIndexForNote (int midiNote) const noexcept;
    int zoneIndexForRoot (int rootNote) const noexcept;

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
