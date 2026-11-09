#pragma once
/**
 * MeterBridge — lock-free audio → UI handoff (JUCE-free).
 *
 * Normative: plugins/MeterTap/.ideas/architecture.md §2.2
 *   - peakSinceRead: audio CAS-max with block peak; UI exchange(0) defines refresh window.
 *   - rmsLatest: audio store(relaxed); UI load(relaxed).
 *   - One CAS loop per channel per block (not per sample).
 */
#include <array>
#include <atomic>
#include <cmath>

namespace dl {
namespace analysis {

class MeterBridge {
public:
  static constexpr int kNumChannels = 2;

  static_assert(std::atomic<float>::is_always_lock_free,
                "MeterBridge requires lock-free std::atomic<float> (architecture.md §2.2)");

  MeterBridge() noexcept = default;

  /** Audio thread: fold blockPeak into peakSinceRead (CAS max); store rms. */
  void publish(int channel, float blockPeak, float rms) noexcept {
    if (channel < 0 || channel >= kNumChannels) {
      return;
    }
    auto& a = ch_[static_cast<size_t>(channel)];
    float cur = a.peakSinceRead.load(std::memory_order_relaxed);
    while (blockPeak > cur) {
      if (a.peakSinceRead.compare_exchange_weak(cur, blockPeak, std::memory_order_relaxed,
                                                 std::memory_order_relaxed)) {
        break;
      }
    }
    a.rmsLatest.store(rms, std::memory_order_relaxed);
  }

  /** UI thread: read-and-reset peak window (exchange 0). */
  [[nodiscard]] float consumePeak(int channel) noexcept {
    if (channel < 0 || channel >= kNumChannels) {
      return 0.0f;
    }
    return ch_[static_cast<size_t>(channel)].peakSinceRead.exchange(0.0f, std::memory_order_relaxed);
  }

  /** UI thread: latest RMS (relaxed). */
  [[nodiscard]] float loadRms(int channel) const noexcept {
    if (channel < 0 || channel >= kNumChannels) {
      return 0.0f;
    }
    return ch_[static_cast<size_t>(channel)].rmsLatest.load(std::memory_order_relaxed);
  }

  /** Test / prepare helper: zero both channels. */
  void reset() noexcept {
    for (auto& a : ch_) {
      a.peakSinceRead.store(0.0f, std::memory_order_relaxed);
      a.rmsLatest.store(0.0f, std::memory_order_relaxed);
    }
  }

private:
  struct ChannelAtomics {
    std::atomic<float> peakSinceRead{0.0f};
    std::atomic<float> rmsLatest{0.0f};
  };

  std::array<ChannelAtomics, kNumChannels> ch_{};
};

} // namespace analysis
} // namespace dl
