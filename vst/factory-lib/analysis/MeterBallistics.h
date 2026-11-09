#pragma once
/**
 * MeterBallistics — UI-thread display laws with explicit dt (JUCE-free).
 *
 * Normative: plugins/MeterTap/.ideas/architecture.md §3.1
 *   Peak: attack 5 ms TC toward higher; release 300 ms toward lower;
 *         optional hold 1.5 s then 300 ms release.
 *   RMS:  attack 300 ms / release 300 ms; no hold.
 *   dBFS: 20*log10(max(v, 1e-12)); numeric floor −96 dBFS.
 *
 * DPCS Phase 2 contract (declarations). Bodies land in Phase 3.
 */
#include <cmath>

namespace dl {
namespace analysis {

class MeterBallistics {
public:
  static constexpr float kPeakAttackMs = 5.0f;
  static constexpr float kPeakReleaseMs = 300.0f;
  static constexpr float kRmsAttackMs = 300.0f;
  static constexpr float kRmsReleaseMs = 300.0f;
  static constexpr float kPeakHoldSeconds = 1.5f;
  static constexpr float kNumericFloorDb = -96.0f;
  static constexpr float kMinLinear = 1.0e-12f;

  MeterBallistics() noexcept = default;

  void reset() noexcept;

  void setPeakHoldEnabled(bool enabled) noexcept;
  [[nodiscard]] bool peakHoldEnabled() const noexcept { return peakHoldEnabled_; }

  /**
   * Advance display state by dtSeconds (must be >= 0).
   * peakLinearIn / rmsLinearIn are detector outputs (linear full-scale).
   */
  void process(float peakLinearIn, float rmsLinearIn, double dtSeconds) noexcept;

  [[nodiscard]] float peakLinear() const noexcept { return peakLin_; }
  [[nodiscard]] float rmsLinear() const noexcept { return rmsLin_; }

  /** Display dBFS after numeric floor (−96). */
  [[nodiscard]] float peakDb() const noexcept;
  [[nodiscard]] float rmsDb() const noexcept;

  /** Convert linear amplitude to dBFS with architecture floor. */
  [[nodiscard]] static float linearToDb(float linear) noexcept;

private:
  static float coeffFromMs(float timeMs, double dtSeconds) noexcept;
  static float toward(float current, float target, float coeff) noexcept;

  float peakLin_ = 0.0f;
  float rmsLin_ = 0.0f;
  float holdPeakLin_ = 0.0f;
  double holdRemaining_ = 0.0;
  bool peakHoldEnabled_ = true;
};

} // namespace analysis
} // namespace dl
