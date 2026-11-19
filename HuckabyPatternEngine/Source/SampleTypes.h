#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <memory>

namespace huckaby
{

/** PCM sample with metadata; owned via std::shared_ptr for safe playback after bank swaps. */
struct SampleData
{
    juce::AudioBuffer<float> buffer;
    double sampleRate { 44100.0 };
    juce::String sampleName;
    int lengthInSamples { 0 };

    SampleData() = default;

    SampleData(juce::AudioBuffer<float>&& srcBuffer, double sr, juce::String name)
        : buffer(std::move(srcBuffer))
        , sampleRate(sr)
        , sampleName(std::move(name))
    {
        lengthInSamples = buffer.getNumSamples();
    }
};

/**
 * Real-time readers load std::shared_ptr with acquire; writers (message / loader thread)
 * replace with release. Voices keep a local shared_ptr copy for the duration of a hit.
 */
class AtomicSharedSample final
{
public:
    AtomicSharedSample() = default;

    std::shared_ptr<SampleData> load() const noexcept
    {
        return ptr.load(std::memory_order_acquire);
    }

    void store(std::shared_ptr<SampleData> next) noexcept
    {
        ptr.store(std::move(next), std::memory_order_release);
    }

    void reset() noexcept
    {
        store(nullptr);
    }

private:
    mutable std::atomic<std::shared_ptr<SampleData>> ptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AtomicSharedSample)
};

} // namespace huckaby
