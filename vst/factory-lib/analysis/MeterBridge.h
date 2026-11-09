#pragma once
/**
 * MeterBridge — lock-free audio → UI handoff (JUCE-free).
 *
 * Normative: plugins/MeterTap/.ideas/architecture.md §2.2
 *   - peakSinceRead: audio CAS-max with block peak; UI exchange(0) defines refresh window.
 *   - rmsLatest: audio store(relaxed); UI load(relaxed).
 *   - One CAS loop per channel per block (not per sample).
 *
 * DPCS Phase 2 contract (declarations). Bodies land in Phase 3.
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
  void publish(int channel, float blockPeak, float rms) noexcept;

  /** UI thread: read-and-reset peak window (exchange 0). */
  [[nodiscard]] float consumePeak(int channel) noexcept;

  /** UI thread: latest RMS (relaxed). */
  [[nodiscard]] float loadRms(int channel) const noexcept;

  /** Test / prepare helper: zero both channels. */
  void reset() noexcept;

private:
  struct ChannelAtomics {
    std::atomic<float> peakSinceRead{0.0f};
    std::atomic<float> rmsLatest{0.0f};
  };

  std::array<ChannelAtomics, kNumChannels> ch_{};
};

} // namespace analysis
} // namespace dl
