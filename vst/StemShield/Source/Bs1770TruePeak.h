#pragma once

/**
 * ITU-R BS.1770-4 Annex 2 true-peak interpolator (4× polyphase FIR, 12 taps / phase).
 * Sidechain detection only — audio is not upsampled.
 */
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <functional>

class Bs1770TruePeak {
public:
  static constexpr int kTapsPerPhase = 12;
  static constexpr int kNumPhases = 4;

  enum class RatePolicy { Full4x, Dual2x, PassThrough };

  void prepare(double sampleRate) noexcept {
    sampleRate_ = sampleRate;
    if (sampleRate >= 176400.0) {
      policy_ = RatePolicy::PassThrough;
      groupDelaySamples_ = 0;
    } else if (sampleRate >= 88200.0) {
      policy_ = RatePolicy::Dual2x;
      groupDelaySamples_ = 6;
    } else {
      policy_ = RatePolicy::Full4x;
      groupDelaySamples_ = 6;
    }
  }

  int getGroupDelaySamples() const noexcept { return groupDelaySamples_; }
  RatePolicy getPolicy() const noexcept { return policy_; }
  double getSampleRate() const noexcept { return sampleRate_; }

  /** Measure true peak at index using fetch(-11)..fetch(0) == oldest..newest at center index. */
  template <typename FetchFn>
  float measureAt(FetchFn&& fetch, int centerIndex) const noexcept {
    const float sample = std::fabs(fetch(centerIndex));
    if (policy_ == RatePolicy::PassThrough) {
      return sample;
    }

    float peak = sample;
    const auto evalPhase = [&](int phase) noexcept {
      float sum = 0.0f;
      const auto& coefs = kPhases[static_cast<size_t>(phase)];
      for (int t = 0; t < kTapsPerPhase; ++t) {
        const int sampleIndex = centerIndex - (kTapsPerPhase - 1 - t);
        sum += coefs[static_cast<size_t>(t)] * fetch(sampleIndex);
      }
      peak = std::fmax(peak, std::fabs(sum));
    };

    if (policy_ == RatePolicy::Full4x) {
      for (int p = 0; p < kNumPhases; ++p) {
        evalPhase(p);
      }
    } else {
      evalPhase(0);
      evalPhase(2);
    }
    return peak;
  }

  template <typename FetchFn>
  float measureLinkedPeakInRange(FetchFn&& fetch, int startIndex, int endIndex) const noexcept {
    float peak = 0.0f;
    int idx = startIndex;
    for (;;) {
      peak = std::fmax(peak, measureAt(fetch, idx));
      if (idx == endIndex) {
        break;
      }
      ++idx;
    }
    return peak;
  }

  /** Offline programme peak (reference tests). */
  static float measureBufferProgrammePeak(const float* samples, int count, double sampleRate) {
    if (count <= 0) {
      return 0.0f;
    }
    Bs1770TruePeak det;
    det.prepare(sampleRate);
    auto fetch = [&](int i) -> float {
      if (i < 0 || i >= count) {
        return 0.0f;
      }
      return samples[i];
    };
    float peak = 0.0f;
    for (int i = 0; i < count; ++i) {
      peak = std::fmax(peak, det.measureAt(fetch, i));
    }
    return peak;
  }

private:
  static constexpr std::array<float, kTapsPerPhase> kPhase0 = {
      0.001708984375f,   0.010986328125f,  -0.0196533203125f, 0.033203125f,
      -0.0594482421875f, 0.1373291015625f, 0.97216796875f,    -0.102294921875f,
      0.047607421875f,   -0.026611328125f, 0.014892578125f,   -0.00830078125f};
  static constexpr std::array<float, kTapsPerPhase> kPhase1 = {
      -0.0291748046875f, 0.029296875f,   -0.0517578125f,    0.089111328125f,
      -0.16650390625f,   0.465087890625f, 0.77978515625f,    -0.2003173828125f,
      0.1015625f,        -0.0582275390625f, 0.0330810546875f, -0.0189208984375f};
  static constexpr std::array<float, kTapsPerPhase> kPhase2 = {
      -0.0189208984375f, 0.0330810546875f, -0.0582275390625f, 0.1015625f,
      -0.2003173828125f, 0.77978515625f,   0.465087890625f,   -0.16650390625f,
      0.089111328125f,   -0.0517578125f,   0.029296875f,      -0.0291748046875f};
  static constexpr std::array<float, kTapsPerPhase> kPhase3 = {
      -0.00830078125f,   0.014892578125f, -0.026611328125f,  0.047607421875f,
      -0.102294921875f,  0.97216796875f,   0.1373291015625f,  -0.0594482421875f,
      0.033203125f,      -0.0196533203125f, 0.010986328125f,  0.001708984375f};

  static constexpr std::array<std::array<float, kTapsPerPhase>, kNumPhases> kPhases = {
      kPhase0, kPhase1, kPhase2, kPhase3};

  RatePolicy policy_ = RatePolicy::Full4x;
  int groupDelaySamples_ = 6;
  double sampleRate_ = 48000.0;
};
