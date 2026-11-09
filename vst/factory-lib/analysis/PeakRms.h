#pragma once
/**
 * PeakRms — audio-thread peak + sliding 300 ms RMS detector (JUCE-free).
 *
 * Normative: plugins/MeterTap/.ideas/architecture.md §2.1
 *   - Peak: max |x| over the current process() block.
 *   - RMS: bucketed running sum — 300 fixed 1 ms buckets;
 *     bucketLen = round(sampleRate / 1000); Σx² in double;
 *     window recomputed from scratch every 300 buckets (drift cap).
 *   - Fixed std::array storage; no allocation in prepare/process.
 *
 * DPCS Phase 2 contract (declarations). Bodies land in Phase 3.
 */
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace dl {
namespace analysis {

class PeakRms {
public:
  static constexpr int kNumChannels = 2;
  static constexpr int kNumBuckets = 300;

  PeakRms() noexcept = default;

  /** Set bucket length from sample rate; zeros all state. No heap. */
  void prepare(double sampleRate) noexcept;

  /** Zero buckets / accumulators (same as fresh prepare without changing bucketLen). */
  void reset() noexcept;

  /**
   * Analyse up to kNumChannels channels for numSamples (may be 0 or 1).
   * channelPtrs[c] must be valid for c in [0, numChannels).
   * Extra channels beyond kNumChannels are ignored; missing channels treated as silence.
   */
  void process(const float* const* channelPtrs, int numChannels, int numSamples) noexcept;

  /** Max |x| observed in the most recent process() call for channel. */
  [[nodiscard]] float blockPeak(int channel) const noexcept;

  /** Current sliding-window RMS for channel (linear, full-scale = 1.0). */
  [[nodiscard]] float rms(int channel) const noexcept;

  [[nodiscard]] int bucketLength() const noexcept { return bucketLen_; }

private:
  struct ChannelState {
    std::array<double, kNumBuckets> buckets{};
    double windowSum = 0.0;
    int writeIndex = 0;
    int samplesInBucket = 0;
    int bucketsCompleted = 0;
    float lastBlockPeak = 0.0f;
    float lastRms = 0.0f;
  };

  std::array<ChannelState, kNumChannels> ch_{};
  int bucketLen_ = 48; // default @ 48 kHz until prepare()
};

} // namespace analysis
} // namespace dl
