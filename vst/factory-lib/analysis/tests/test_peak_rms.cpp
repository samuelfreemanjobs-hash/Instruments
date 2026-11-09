/**
 * Contract: PeakRms detector maths (architecture.md verification V1, V2, V3, drift).
 * V1: −12 dBFS peak sine → RMS −15.01 ±0.5, peak −12 ±0.5 @ 44.1/48/96/192 kHz
 * V2: silence → display ≤ −96 dBFS
 * V3: NaN/Inf samples do not poison RMS/peak (treated as magnitude inputs; sanitizer is processor)
 * Drift: 10 min running windowSum vs recompute within 1e-6 relative
 */
#include "MeterBallistics.h"
#include "MeterBridge.h"
#include "PeakRms.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>
#include <vector>

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

float dbToLin(float db) { return std::pow(10.0f, db * 0.05f); }

void fillSine(std::vector<float>& buf, double sr, float peakLin, double freqHz) {
  const double w = 2.0 * 3.14159265358979323846 * freqHz / sr;
  for (size_t i = 0; i < buf.size(); ++i) {
    buf[i] = peakLin * static_cast<float>(std::sin(w * static_cast<double>(i)));
  }
}

void processWithRandomBlocks(dl::analysis::PeakRms& det,
                             float* left,
                             float* right,
                             int totalSamples,
                             std::mt19937& rng) {
  int pos = 0;
  // Include block sizes 0 and 1 explicitly at the start.
  {
    const float* ch[2] = {left + pos, right + pos};
    det.process(ch, 2, 0);
  }
  if (totalSamples > 0) {
    const float* ch[2] = {left + pos, right + pos};
    det.process(ch, 2, 1);
    pos += 1;
  }
  std::uniform_int_distribution<int> dist(1, 512);
  while (pos < totalSamples) {
    int n = dist(rng);
    if (pos + n > totalSamples) {
      n = totalSamples - pos;
    }
    const float* ch[2] = {left + pos, right + pos};
    det.process(ch, 2, n);
    pos += n;
  }
}

void testV1Sine(double sr) {
  const float peakDb = -12.0f;
  const float peakLin = dbToLin(peakDb);
  const int total = static_cast<int>(sr * 2.0); // 2 seconds
  std::vector<float> L(static_cast<size_t>(total)), R(static_cast<size_t>(total));
  fillSine(L, sr, peakLin, 1000.0);
  R = L;

  dl::analysis::PeakRms det;
  det.prepare(sr);
  std::mt19937 rng(0x4D545231u ^ static_cast<unsigned>(sr));
  processWithRandomBlocks(det, L.data(), R.data(), total, rng);

  const float peakMeasDb = dl::analysis::MeterBallistics::linearToDb(det.blockPeak(0));
  // After last block, blockPeak is only that block — use bridge-style max over a final full buffer read:
  // Re-run last 2048 samples as one block to get a clean peak estimate, then compare RMS.
  const float rmsDb = dl::analysis::MeterBallistics::linearToDb(det.rms(0));

  // Peak of a continuous sine in a long block: re-process a 4096-sample window for peak check.
  dl::analysis::PeakRms peakCheck;
  peakCheck.prepare(sr);
  const int nPeak = std::min(4096, total);
  const float* chP[2] = {L.data(), R.data()};
  peakCheck.process(chP, 2, nPeak);
  const float peakDbMeas = dl::analysis::MeterBallistics::linearToDb(peakCheck.blockPeak(0));

  char name[128];
  std::snprintf(name, sizeof(name), "V1 peak @ %.0f Hz within ±0.5 of -12 (got %.3f)", sr, peakDbMeas);
  expect(std::fabs(peakDbMeas - (-12.0f)) <= 0.5f, name);

  std::snprintf(name, sizeof(name), "V1 RMS @ %.0f Hz within ±0.5 of -15.01 (got %.3f)", sr, rmsDb);
  expect(std::fabs(rmsDb - (-15.01f)) <= 0.5f, name);

  (void)peakMeasDb;
}

void testV2Silence(double sr) {
  const int total = static_cast<int>(sr * 2.0);
  std::vector<float> Z(static_cast<size_t>(total), 0.0f);
  dl::analysis::PeakRms det;
  det.prepare(sr);
  std::mt19937 rng(42);
  processWithRandomBlocks(det, Z.data(), Z.data(), total, rng);

  dl::analysis::MeterBallistics ball;
  ball.reset();
  ball.process(det.blockPeak(0), det.rms(0), 1.0 / 30.0);

  char name[128];
  std::snprintf(name, sizeof(name), "V2 silence peakDb <= -96 @ %.0f (got %.3f)", sr, ball.peakDb());
  expect(ball.peakDb() <= -96.0f + 1e-3f, name);
  std::snprintf(name, sizeof(name), "V2 silence rmsDb <= -96 @ %.0f (got %.3f)", sr, ball.rmsDb());
  expect(ball.rmsDb() <= -96.0f + 1e-3f, name);
}

void testV3NanInf() {
  // Detector itself sees raw samples; non-finite x² would poison — processor sanitizes first.
  // Contract here: finite samples after sanitize path; also bridge/ballistics reject non-finite.
  dl::analysis::MeterBallistics ball;
  ball.process(std::numeric_limits<float>::quiet_NaN(),
               std::numeric_limits<float>::infinity(), 0.01);
  expect(std::isfinite(ball.peakLinear()) && std::isfinite(ball.rmsLinear()),
         "V3 ballistics NaN/Inf in -> finite linear out");
  expect(ball.peakDb() <= -96.0f + 1e-3f && ball.rmsDb() <= -96.0f + 1e-3f,
         "V3 ballistics NaN/Inf -> floor dB");

  dl::analysis::MeterBridge bridge;
  bridge.publish(0, 0.5f, 0.25f);
  bridge.publish(0, std::numeric_limits<float>::quiet_NaN(), 0.1f); // NaN should not CAS-max over 0.5
  // CAS compares with >; NaN > cur is false, so peak stays 0.5
  expect(bridge.consumePeak(0) == 0.5f, "V3 bridge CAS ignores NaN peak");
}

void testDrift(double sr) {
  const float peakLin = dbToLin(-6.0f);
  const double minutes = 10.0;
  const int total = static_cast<int>(sr * 60.0 * minutes);
  // Stream in chunks without holding 10 minutes of audio in RAM.
  dl::analysis::PeakRms det;
  det.prepare(sr);

  constexpr int kChunk = 4096;
  std::vector<float> L(kChunk), R(kChunk);
  const double w = 2.0 * 3.14159265358979323846 * 1000.0 / sr;
  int64_t sampleIndex = 0;
  int remaining = total;
  while (remaining > 0) {
    const int n = std::min(kChunk, remaining);
    for (int i = 0; i < n; ++i) {
      const float s = peakLin * static_cast<float>(std::sin(w * static_cast<double>(sampleIndex + i)));
      L[static_cast<size_t>(i)] = s;
      R[static_cast<size_t>(i)] = s;
    }
    const float* ch[2] = {L.data(), R.data()};
    det.process(ch, 2, n);
    sampleIndex += n;
    remaining -= n;
  }

  const double run = det.windowSumForTest(0);
  const double recomputed = det.recomputeWindowSumForTest(0);
  const double denom = std::max(std::fabs(recomputed), 1e-30);
  const double rel = std::fabs(run - recomputed) / denom;
  char name[160];
  std::snprintf(name, sizeof(name),
                "Drift 10 min @ %.0f: |run-recompute|/|recompute|=%.3e (<=1e-6)", sr, rel);
  expect(rel <= 1.0e-6, name);
}

void testMeterBridgeCasMax() {
  dl::analysis::MeterBridge b;
  b.publish(0, 0.1f, 0.05f);
  b.publish(0, 0.4f, 0.2f);
  b.publish(0, 0.2f, 0.1f); // lower peak must not win
  expect(std::fabs(b.consumePeak(0) - 0.4f) < 1e-6f, "Bridge CAS-max keeps highest peak");
  expect(b.consumePeak(0) == 0.0f, "Bridge consumePeak resets to 0");
  expect(std::fabs(b.loadRms(0) - 0.1f) < 1e-6f, "Bridge rmsLatest is last store");
}

} // namespace

int main() {
  const double rates[] = {44100.0, 48000.0, 96000.0, 192000.0};
  for (double sr : rates) {
    testV1Sine(sr);
    testV2Silence(sr);
  }
  testV3NanInf();
  testMeterBridgeCasMax();
  // Drift at 48 kHz only (10 min) — expensive but required
  testDrift(48000.0);

  if (gFailures != 0) {
    std::fprintf(stderr, "%d failure(s)\n", gFailures);
    return 1;
  }
  std::printf("All PeakRms/MeterBridge contracts passed\n");
  return 0;
}
