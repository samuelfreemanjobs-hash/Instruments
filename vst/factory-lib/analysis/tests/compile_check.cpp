// Phase 2 compile-check: headers alone must parse (no JUCE).
#include "MeterBallistics.h"
#include "MeterBridge.h"
#include "PeakRms.h"

int main() {
  static_assert(dl::analysis::PeakRms::kNumBuckets == 300, "architecture.md §2.1");
  static_assert(dl::analysis::MeterBridge::kNumChannels == 2, "architecture.md §2.2");
  static_assert(std::atomic<float>::is_always_lock_free, "architecture.md §2.2");
  return 0;
}
