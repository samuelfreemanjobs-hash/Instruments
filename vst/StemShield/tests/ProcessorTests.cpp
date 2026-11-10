/**
 * Offline contracts: BS.1770 true-peak ceiling, latency impulse, NaN/denormal, silence.
 */
#include "../Source/Bs1770TruePeak.h"
#include "../Source/PluginProcessor.h"
#include "../Source/StemShieldDsp.h"

#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
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

void setParamValue(StemShieldAudioProcessor& p, const char* id, float value) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->convertTo0to1(value));
    param->endChangeGesture();
  }
}

bool allFinite(const juce::AudioBuffer<float>& b) {
  for (int ch = 0; ch < b.getNumChannels(); ++ch) {
    const float* x = b.getReadPointer(ch);
    for (int i = 0; i < b.getNumSamples(); ++i) {
      if (!std::isfinite(x[i])) {
        return false;
      }
    }
  }
  return true;
}

float referenceTruePeak8xCascaded(const float* samples, int count, double sampleRate) {
  if (count <= 0) {
    return 0.0f;
  }
  Bs1770TruePeak stage1;
  stage1.prepare(sampleRate);
  const int factor = (stage1.getPolicy() == Bs1770TruePeak::RatePolicy::PassThrough) ? 1 : 4;
  const int upCount = count * factor;
  auto fetchIn = [&](int i) -> float {
    if (i < 0 || i >= count) {
      return 0.0f;
    }
    return samples[i];
  };

  std::vector<float> up1(static_cast<size_t>(upCount), 0.0f);
  float peak = 0.0f;
  constexpr std::array<float, 12> kP0 = {
      0.001708984375f,   0.010986328125f,  -0.0196533203125f, 0.033203125f,
      -0.0594482421875f, 0.1373291015625f, 0.97216796875f,    -0.102294921875f,
      0.047607421875f,   -0.026611328125f, 0.014892578125f,   -0.00830078125f};
  constexpr std::array<float, 12> kP1 = {
      -0.0291748046875f, 0.029296875f,   -0.0517578125f,    0.089111328125f,
      -0.16650390625f,   0.465087890625f, 0.77978515625f,    -0.2003173828125f,
      0.1015625f,        -0.0582275390625f, 0.0330810546875f, -0.0189208984375f};
  constexpr std::array<float, 12> kP2 = {
      -0.0189208984375f, 0.0330810546875f, -0.0582275390625f, 0.1015625f,
      -0.2003173828125f, 0.77978515625f,   0.465087890625f,   -0.16650390625f,
      0.089111328125f,   -0.0517578125f,   0.029296875f,      -0.0291748046875f};
  constexpr std::array<float, 12> kP3 = {
      -0.00830078125f,   0.014892578125f, -0.026611328125f,  0.047607421875f,
      -0.102294921875f,  0.97216796875f,   0.1373291015625f,  -0.0594482421875f,
      0.033203125f,      -0.0196533203125f, 0.010986328125f,  0.001708984375f};
  const std::array<std::array<float, 12>, 4> phases = {kP0, kP1, kP2, kP3};

  auto convolvePhase = [&](int center, int phase) {
    float sum = 0.0f;
    for (int t = 0; t < 12; ++t) {
      sum += phases[static_cast<size_t>(phase)][static_cast<size_t>(t)] *
             fetchIn(center - (11 - t));
    }
    return sum;
  };

  for (int i = 0; i < count; ++i) {
    up1[static_cast<size_t>(i * factor + 0)] = convolvePhase(i, 0);
    up1[static_cast<size_t>(i * factor + 1)] = convolvePhase(i, 1);
    up1[static_cast<size_t>(i * factor + 2)] = convolvePhase(i, 2);
    up1[static_cast<size_t>(i * factor + 3)] = convolvePhase(i, 3);
    peak = std::fmax(peak, std::fabs(up1[static_cast<size_t>(i * factor + 0)]));
    peak = std::fmax(peak, std::fabs(up1[static_cast<size_t>(i * factor + 1)]));
    peak = std::fmax(peak, std::fabs(up1[static_cast<size_t>(i * factor + 2)]));
    peak = std::fmax(peak, std::fabs(up1[static_cast<size_t>(i * factor + 3)]));
  }

  Bs1770TruePeak stage2;
  stage2.prepare(sampleRate * static_cast<double>(factor));
  float peak2 = 0.0f;
  auto fetchUp = [&](int i) -> float {
    if (i < 0 || i >= upCount) {
      return 0.0f;
    }
    return up1[static_cast<size_t>(i)];
  };
  for (int i = 0; i < upCount; ++i) {
    peak2 = std::fmax(peak2, stage2.measureAt(fetchUp, i));
  }
  return std::fmax(peak, peak2);
}

int measureImpulseDelayAligned(StemShieldAudioProcessor& proc, double sampleRate, int blockSize) {
  juce::AudioBuffer<float> buf(2, blockSize);
  juce::MidiBuffer midi;
  const int latency = proc.getLatencySamples();
  const int impulseIndex = latency + blockSize * 8 + 64;

  std::vector<float> output;
  output.reserve(static_cast<size_t>(impulseIndex + blockSize * 24));

  int inSample = 0;
  const int totalSamples = impulseIndex + blockSize * 24;
  while (inSample < totalSamples) {
    for (int i = 0; i < blockSize; ++i) {
      if (inSample < totalSamples) {
        const float x = (inSample == impulseIndex) ? 1.0f : 0.0f;
        buf.setSample(0, i, x);
        buf.setSample(1, i, x);
        ++inSample;
      } else {
        buf.setSample(0, i, 0.0f);
        buf.setSample(1, i, 0.0f);
      }
    }
    proc.processBlock(buf, midi);
    for (int i = 0; i < blockSize; ++i) {
      output.push_back(buf.getSample(0, i));
    }
  }

  int peakIndex = 0;
  float peakVal = 0.0f;
  for (int i = 0; i < static_cast<int>(output.size()); ++i) {
    const float a = std::fabs(output[static_cast<size_t>(i)]);
    if (a > peakVal) {
      peakVal = a;
      peakIndex = i;
    }
  }
  return peakIndex - impulseIndex;
}

void runTruePeakCeilingSuite(double sampleRate, const char* label) {
  constexpr int kBlock = 512;
  constexpr float kCeilingDb = -0.5f;
  const float ceilingLinear = juce::Decibels::decibelsToGain(kCeilingDb, -120.0f);
  const float toleranceLinear = juce::Decibels::decibelsToGain(kCeilingDb + 0.1f, -120.0f);

  StemShieldAudioProcessor proc;
  setParamValue(proc, StemShieldAudioProcessor::kParamCeiling, kCeilingDb);
  setParamValue(proc, StemShieldAudioProcessor::kParamSubBalance, 0.0f);
  setParamValue(proc, StemShieldAudioProcessor::kParamGain, 0.0f);
  setParamValue(proc, StemShieldAudioProcessor::kParamRelease, 80.0f);
  setParamValue(proc, StemShieldAudioProcessor::kParamLookahead, 3.0f);
  proc.prepareToPlay(sampleRate, kBlock);

  const int settleBlocks = static_cast<int>((0.5 * sampleRate) / kBlock) + 8;
  const int captureBlocks = static_cast<int>((0.35 * sampleRate) / kBlock) + 4;
  std::vector<float> capture;

  auto processSignal = [&](auto&& fillSample) {
    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    capture.clear();
    for (int b = 0; b < settleBlocks; ++b) {
      buf.clear();
      proc.processBlock(buf, midi);
    }
    for (int b = 0; b < captureBlocks; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(sampleRate);
        const float x = fillSample(t, i);
        buf.setSample(0, i, x);
        buf.setSample(1, i, x);
      }
      proc.processBlock(buf, midi);
      capture.insert(capture.end(), buf.getReadPointer(0), buf.getReadPointer(0) + kBlock);
    }
  };

  // fs/4 sine, 45° phase (inter-sample peak case)
  processSignal([&](float t, int) {
    const float w = juce::MathConstants<float>::twoPi * static_cast<float>(sampleRate * 0.25) / static_cast<float>(sampleRate);
    return std::sin(w * t + juce::MathConstants<float>::halfPi * 0.5f);
  });
  const float tpSine = referenceTruePeak8xCascaded(capture.data(), static_cast<int>(capture.size()), sampleRate);
  const float overshootSineDb =
      juce::Decibels::gainToDecibels(tpSine, -120.0f) - kCeilingDb;
  std::printf("METRIC %s fs/4+45deg referenceTP=%.4f dBTP overshoot=%.3f dB\n", label,
              juce::Decibels::gainToDecibels(tpSine, -120.0f), overshootSineDb);
  expect(tpSine <= toleranceLinear + 1.0e-4f, (std::string(label) + " fs/4 sine true-peak <= ceiling+0.1dB").c_str());

  // 0 dBFS sample-peak square
  processSignal([&](float, int i) { return (i & 1) == 0 ? 1.0f : -1.0f; });
  const float tpSquare = referenceTruePeak8xCascaded(capture.data(), static_cast<int>(capture.size()), sampleRate);
  const float overshootSquareDb =
      juce::Decibels::gainToDecibels(tpSquare, -120.0f) - kCeilingDb;
  std::printf("METRIC %s square referenceTP=%.4f dBTP overshoot=%.3f dB\n", label,
              juce::Decibels::gainToDecibels(tpSquare, -120.0f), overshootSquareDb);
  expect(tpSquare <= toleranceLinear + 1.0e-4f, (std::string(label) + " square true-peak <= ceiling+0.1dB").c_str());
}

} // namespace

int main() {
  constexpr int kBlock = 512;

  std::printf("=== True-peak ceiling suites ===\n");
  runTruePeakCeilingSuite(44100.0, "44.1k");
  runTruePeakCeilingSuite(48000.0, "48k");

  for (double sr : {44100.0, 48000.0}) {
    StemShieldAudioProcessor proc;
    setParamValue(proc, StemShieldAudioProcessor::kParamCeiling, -0.5f);
    setParamValue(proc, StemShieldAudioProcessor::kParamSubBalance, 0.0f);
    setParamValue(proc, StemShieldAudioProcessor::kParamLookahead, 2.5f);
    setParamValue(proc, StemShieldAudioProcessor::kParamGain, 0.0f);
    proc.prepareToPlay(sr, kBlock);
    const int lat = proc.getLatencySamples();
    std::printf("METRIC latency @ %.0f Hz = %d samples (reported)\n", sr, lat);

    setParamValue(proc, StemShieldAudioProcessor::kParamLookahead, 2.5f);
    setParamValue(proc, StemShieldAudioProcessor::kParamRelease, 400.0f);
    const int measured = measureImpulseDelayAligned(proc, sr, kBlock);
    std::printf("METRIC latency @ %.0f Hz measured impulse delay = %d samples\n", sr, measured);
    expect(measured == lat, sr == 44100.0 ? "44.1k latency == impulse delay" : "48k latency == impulse delay");
  }

  {
    StemShieldAudioProcessor proc;
    setParamValue(proc, StemShieldAudioProcessor::kParamGain, 0.0f);
    setParamValue(proc, StemShieldAudioProcessor::kParamSubBalance, 0.0f);
    proc.prepareToPlay(48000.0, kBlock);
    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int b = 0; b < 80; ++b) {
      buf.clear();
      proc.processBlock(buf, midi);
    }
    float maxAbs = 0.0f;
    for (int ch = 0; ch < 2; ++ch) {
      maxAbs = std::fmax(maxAbs, buf.getMagnitude(ch, 0, kBlock));
    }
    expect(maxAbs < 1.0e-9f, "silence in silence out");
    expect(allFinite(buf), "silence output finite");
  }

  {
    StemShieldAudioProcessor p3;
    p3.prepareToPlay(48000.0, kBlock);
    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int i = 0; i < kBlock; ++i) {
      buf.setSample(0, i, std::numeric_limits<float>::quiet_NaN());
      buf.setSample(1, i, 0.1f);
    }
    p3.processBlock(buf, midi);
    expect(allFinite(buf), "NaN in yields finite out");
  }

  std::printf("Done. failures=%d\n", gFailures);
  return gFailures == 0 ? 0 : 1;
}
