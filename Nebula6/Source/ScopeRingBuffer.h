#pragma once

#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>

namespace play_series {

/** Lock-free mono peak samples for WebView scope (UI thread reads). */
class ScopeRingBuffer {
public:
  static constexpr int kCapacity = 4096;

  void reset() noexcept {
    writePos_.store(0, std::memory_order_relaxed);
  }

  void pushPeak(float peak) noexcept {
    const int idx = writePos_.fetch_add(1, std::memory_order_relaxed) % kCapacity;
    buffer_[static_cast<size_t>(idx)] = std::fabs(peak);
  }

  int readRecentPeaks(float *dest, int maxItems) const noexcept {
    if (dest == nullptr || maxItems <= 0)
      return 0;
    const int end = writePos_.load(std::memory_order_acquire);
    const int count = std::min(maxItems, kCapacity);
    for (int i = 0; i < count; ++i) {
      const int idx = (end - 1 - i + kCapacity * 64) % kCapacity;
      dest[i] = buffer_[static_cast<size_t>(idx)];
    }
    return count;
  }

private:
  std::array<float, static_cast<size_t>(kCapacity)> buffer_{};
  std::atomic<int> writePos_{0};
};

} // namespace play_series