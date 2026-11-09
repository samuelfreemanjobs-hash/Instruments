/**
 * Contract: MeterBallistics V6 (architecture.md §3.1 / verification V6).
 * - Peak hold keeps value 1.5 s ±1 tick, then decays at 300 ms release.
 * - RMS follows 300 ms time-constant (≈63.2% after one tau on a step).
 */
#include "MeterBallistics.h"

#include <cmath>
#include <cstdio>

namespace {

int gFailures = 0;

void expect(bool cond, const char* name) {
  if (!cond) {
    std::fprintf(stderr, "FAIL: %s\n", name);
    ++gFailures;
  } else {
    std::printf("PASS: %s\n", name);
  }
}

} // namespace

int main() {
  using dl::analysis::MeterBallistics;

  // --- Peak hold 1.5 s then release ---
  {
    MeterBallistics b;
    b.setPeakHoldEnabled(true);
    constexpr double kDt = 1.0 / 30.0; // 30 Hz UI tick
    b.process(0.5f, 0.0f, kDt);       // establish hold peak
    expect(std::fabs(b.peakLinear() - 0.5f) < 0.05f, "V6 peak jumps toward 0.5 with attack");

    const int holdTicks = static_cast<int>(std::lround(1.5 / kDt));
    for (int i = 0; i < holdTicks - 1; ++i) {
      b.process(0.0f, 0.0f, kDt); // input drops; hold should retain
    }
    expect(b.holdRemainingForTest() > 0.0, "V6 hold still active before 1.5 s");
    expect(b.peakLinear() > 0.45f, "V6 peak held near 0.5 during hold window");

    // One more tick past hold → should begin releasing
    b.process(0.0f, 0.0f, kDt);
    b.process(0.0f, 0.0f, kDt);
    // After hold expires, release toward 0 with 300 ms TC
    for (int i = 0; i < 30; ++i) { // ~1 s
      b.process(0.0f, 0.0f, kDt);
    }
    expect(b.peakLinear() < 0.45f, "V6 peak decays after hold expires");
  }

  // --- RMS 300 ms time constant ---
  {
    MeterBallistics b;
    b.setPeakHoldEnabled(false);
    b.reset();
    constexpr double kDt = 0.001; // 1 ms steps for accuracy
    const float target = 1.0f;
    const int steps = 300; // one tau
    for (int i = 0; i < steps; ++i) {
      b.process(0.0f, target, kDt);
    }
    // 1 - e^{-1} ≈ 0.632
    const float expected = static_cast<float>(1.0 - std::exp(-1.0));
    expect(std::fabs(b.rmsLinear() - expected) < 0.05f, "V6 RMS ~63% after 300 ms TC");
  }

  if (gFailures != 0) {
    std::fprintf(stderr, "%d failure(s)\n", gFailures);
    return 1;
  }
  std::printf("All MeterBallistics contracts passed\n");
  return 0;
}
