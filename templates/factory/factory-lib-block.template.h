#pragma once
// {{BLOCK_NAME}} ({{CATEGORY}}.{{BLOCK_ID}}) — DPCS Phase 2 contract.
// Spec: plugins/{{PLUGIN_NAME}}/.ideas/architecture.md · SOP-VF-10 reusable module.
// Realtime: no allocation, locks, I/O or logging in process(). JUCE-free.

#include <cstddef>

namespace factory::{{AREA}} {

class {{BLOCK_CLASS}} {
public:
    /** Called off the audio thread before processing; may size fixed storage only. */
    void prepare(double sampleRate, int maxBlockSize) noexcept;

    /** Clears all state to the post-prepare condition. Realtime-safe. */
    void reset() noexcept;

    /** Processes numSamples (0..maxBlockSize) per channel. Realtime-safe. */
    void process(const float* const* channels, int numChannels, int numSamples) noexcept;

private:
    double sampleRate_ = 0.0;
};

} // namespace factory::{{AREA}}
