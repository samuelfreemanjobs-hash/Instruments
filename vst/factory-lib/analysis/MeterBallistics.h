#pragma once
/**
 * MeterBallistics — UI-thread display laws with explicit dt (JUCE-free).
 *
 * Normative: plugins/MeterTap/.ideas/architecture.md §3.1
 *   Peak: attack 5 ms TC toward higher; release 300 ms toward lower;
 *         optional hold 1.5 s then 300 ms release.
 *   RMS:  attack 300 ms / release 300 ms; no hold.
 *   dBFS: 20*log10(max(v, 1e-12)); numeric floor −96 dBFS.
 */
#include <algorithm>
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

  void reset() noexcept {
    peakLin_ = 0.0f;
    rmsLin_ = 0.0f;
    holdPeakLin_ = 0.0f;
    holdRemaining_ = 0.0;
  }

  void setPeakHoldEnabled(bool enabled) noexcept {
    peakHoldEnabled_ = enabled;
    if (!enabled) {
      holdRemaining_ = 0.0;
      holdPeakLin_ = 0.0f;
    }
  }

  [[nodiscard]] bool peakHoldEnabled() const noexcept { return peakHoldEnabled_; }

  /**
   * Advance display state by dtSeconds (must be >= 0).
   * peakLinearIn / rmsLinearIn are detector outputs (linear full-scale).
   */
  void process(float peakLinearIn, float rmsLinearIn, double dtSeconds) noexcept {
    if (!(dtSeconds > 0.0) || !std::isfinite(dtSeconds)) {
      dtSeconds = 0.0;
    }
    if (!std::isfinite(peakLinearIn) || peakLinearIn < 0.0f) {
      peakLinearIn = 0.0f;
    }
    if (!std::isfinite(rmsLinearIn) || rmsLinearIn < 0.0f) {
      rmsLinearIn = 0.0f;
    }

    float peakTarget = peakLinearIn;
    if (peakHoldEnabled_) {
      if (peakLinearIn >= holdPeakLin_) {
        holdPeakLin_ = peakLinearIn;
        holdRemaining_ = static_cast<double>(kPeakHoldSeconds);
      } else if (holdRemaining_ > 0.0) {
        holdRemaining_ -= dtSeconds;
        if (holdRemaining_ < 0.0) {
          holdRemaining_ = 0.0;
        }
      }
      if (holdRemaining_ > 0.0) {
        peakTarget = holdPeakLin_;
      }
    }

    const float peakAttack = coeffFromMs(kPeakAttackMs, dtSeconds);
    const float peakRelease = coeffFromMs(kPeakReleaseMs, dtSeconds);
    if (peakTarget >= peakLin_) {
      peakLin_ = toward(peakLin_, peakTarget, peakAttack);
    } else {
      peakLin_ = toward(peakLin_, peakTarget, peakRelease);
    }

    const float rmsAttack = coeffFromMs(kRmsAttackMs, dtSeconds);
    const float rmsRelease = coeffFromMs(kRmsReleaseMs, dtSeconds);
    if (rmsLinearIn >= rmsLin_) {
      rmsLin_ = toward(rmsLin_, rmsLinearIn, rmsAttack);
    } else {
      rmsLin_ = toward(rmsLin_, rmsLinearIn, rmsRelease);
    }
  }

  [[nodiscard]] float peakLinear() const noexcept { return peakLin_; }
  [[nodiscard]] float rmsLinear() const noexcept { return rmsLin_; }

  /** Display dBFS after numeric floor (−96). */
  [[nodiscard]] float peakDb() const noexcept { return linearToDb(peakLin_); }
  [[nodiscard]] float rmsDb() const noexcept { return linearToDb(rmsLin_); }

  /** Convert linear amplitude to dBFS with architecture floor. */
  [[nodiscard]] static float linearToDb(float linear) noexcept {
    if (!std::isfinite(linear) || linear < kMinLinear) {
      return kNumericFloorDb;
    }
    const float db = 20.0f * std::log10(linear);
    return (db < kNumericFloorDb) ? kNumericFloorDb : db;
  }

  /** Test hook: remaining peak-hold time in seconds. */
  [[nodiscard]] double holdRemainingForTest() const noexcept { return holdRemaining_; }

private:
  static float coeffFromMs(float timeMs, double dtSeconds) noexcept {
    if (!(dtSeconds > 0.0) || !(timeMs > 0.0f)) {
      return 1.0f;
    }
    const double tau = static_cast<double>(timeMs) * 0.001;
    const double c = 1.0 - std::exp(-dtSeconds / tau);
    return static_cast<float>(std::min(1.0, std::max(0.0, c)));
  }

  static float toward(float current, float target, float coeff) noexcept {
    return current + (target - current) * coeff;
  }

  float peakLin_ = 0.0f;
  float rmsLin_ = 0.0f;
  float holdPeakLin_ = 0.0f;
  double holdRemaining_ = 0.0;
  bool peakHoldEnabled_ = true;
};

} // namespace analysis
} // namespace dl
