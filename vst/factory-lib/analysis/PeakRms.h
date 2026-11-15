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
  void prepare(double sampleRate) noexcept {
    const double sr = (sampleRate > 0.0) ? sampleRate : 48000.0;
    const int len = static_cast<int>(std::lround(sr / 1000.0));
    bucketLen_ = (len < 1) ? 1 : len;
    reset();
  }

  /** Zero buckets / accumulators (same as fresh prepare without changing bucketLen). */
  void reset() noexcept {
    for (auto& c : ch_) {
      c.buckets.fill(0.0);
      c.currentBucket = 0.0;
      c.windowSum = 0.0;
      c.writeIndex = 0;
      c.samplesInBucket = 0;
      c.bucketsCompleted = 0;
      c.lastBlockPeak = 0.0f;
      c.lastRms = 0.0f;
    }
  }

  /**
   * Analyse up to kNumChannels channels for numSamples (may be 0 or 1).
   * channelPtrs[c] must be valid for c in [0, numChannels).
   * Extra channels beyond kNumChannels are ignored; missing channels treated as silence.
   */
  void process(const float* const* channelPtrs, int numChannels, int numSamples) noexcept {
    if (numSamples < 0) {
      numSamples = 0;
    }
    const int channels = (numChannels < kNumChannels) ? numChannels : kNumChannels;
    const double denom = static_cast<double>(kNumBuckets) * static_cast<double>(bucketLen_);

    for (int ch = 0; ch < kNumChannels; ++ch) {
      auto& s = ch_[static_cast<size_t>(ch)];
      float peak = 0.0f;

      if (ch < channels && channelPtrs != nullptr && channelPtrs[ch] != nullptr && numSamples > 0) {
        const float* x = channelPtrs[ch];
        for (int i = 0; i < numSamples; ++i) {
          const float v = x[i];
          const float a = (v < 0.0f) ? -v : v;
          if (a > peak) {
            peak = a;
          }
          const double xx = static_cast<double>(v) * static_cast<double>(v);
          s.currentBucket += xx;
          ++s.samplesInBucket;

          if (s.samplesInBucket >= bucketLen_) {
            finishBucket(s);
          }
        }
      }

      s.lastBlockPeak = peak;
      const double meanSq = (denom > 0.0) ? (s.windowSum / denom) : 0.0;
      s.lastRms = (meanSq > 0.0) ? static_cast<float>(std::sqrt(meanSq)) : 0.0f;
    }
  }

  /** Max |x| observed in the most recent process() call for channel. */
  [[nodiscard]] float blockPeak(int channel) const noexcept {
    if (channel < 0 || channel >= kNumChannels) {
      return 0.0f;
    }
    return ch_[static_cast<size_t>(channel)].lastBlockPeak;
  }

  /** Current sliding-window RMS for channel (linear, full-scale = 1.0). */
  [[nodiscard]] float rms(int channel) const noexcept {
    if (channel < 0 || channel >= kNumChannels) {
      return 0.0f;
    }
    return ch_[static_cast<size_t>(channel)].lastRms;
  }

  [[nodiscard]] int bucketLength() const noexcept { return bucketLen_; }

  /** Test hook: running window Σx² (excludes the in-progress bucket). */
  [[nodiscard]] double windowSumForTest(int channel) const noexcept {
    if (channel < 0 || channel >= kNumChannels) {
      return 0.0;
    }
    return ch_[static_cast<size_t>(channel)].windowSum;
  }

  /** Test hook: recompute Σ buckets from scratch (architecture drift check). */
  [[nodiscard]] double recomputeWindowSumForTest(int channel) const noexcept {
    if (channel < 0 || channel >= kNumChannels) {
      return 0.0;
    }
    const auto& s = ch_[static_cast<size_t>(channel)];
    double sum = 0.0;
    for (double b : s.buckets) {
      sum += b;
    }
    return sum;
  }

private:
  struct ChannelState {
    std::array<double, kNumBuckets> buckets{};
    double currentBucket = 0.0;
    double windowSum = 0.0;
    int writeIndex = 0;
    int samplesInBucket = 0;
    int bucketsCompleted = 0;
    float lastBlockPeak = 0.0f;
    float lastRms = 0.0f;
  };

  static void finishBucket(ChannelState& s) noexcept {
    const double oldest = s.buckets[static_cast<size_t>(s.writeIndex)];
    s.windowSum -= oldest;
    s.buckets[static_cast<size_t>(s.writeIndex)] = s.currentBucket;
    s.windowSum += s.currentBucket;

    s.writeIndex = (s.writeIndex + 1) % kNumBuckets;
    s.currentBucket = 0.0;
    s.samplesInBucket = 0;
    ++s.bucketsCompleted;

    // architecture.md §2.1 — recompute every full window to cap FP drift
    if ((s.bucketsCompleted % kNumBuckets) == 0) {
      double sum = 0.0;
      for (double b : s.buckets) {
        sum += b;
      }
      s.windowSum = sum;
    }
  }

  std::array<ChannelState, kNumChannels> ch_{};
  int bucketLen_ = 48; // default @ 48 kHz until prepare()
};

} // namespace analysis
} // namespace dl
