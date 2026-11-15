/**
 * Offline contracts: wow/flutter motion, ceiling, NaN safety, dry passthrough.
 */
#include "../Source/PluginProcessor.h"
#include "../Source/PhonkTapeDsp.h"

#include <cmath>
#include <cstdio>
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

void setParamPercent(PhonkTapeAudioProcessor& p, const char* id, float pctOrValue) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->convertTo0to1(pctOrValue));
    param->endChangeGesture();
  }
}

float maxPeak(const juce::AudioBuffer<float>& b) {
  float peak = 0.0f;
  for (int ch = 0; ch < b.getNumChannels(); ++ch) {
    const float m = b.getMagnitude(ch, 0, b.getNumSamples());
    peak = std::fmax(peak, m);
  }
  return peak;
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

} // namespace

int main() {
  constexpr double kSr = 48000.0;
  constexpr int kBlock = 512;

  PhonkTapeAudioProcessor proc;
  proc.prepareToPlay(kSr, kBlock);
  expect(proc.getLatencySamples() > 0, "reports positive latency (delay + OS)");

  // --- Wow/flutter increases short-term variation vs steady tone ---
  {
    setParamPercent(proc, PhonkTapeAudioProcessor::kParamDrive, 30.0f);
    setParamPercent(proc, PhonkTapeAudioProcessor::kParamWow, 85.0f);
    setParamPercent(proc, PhonkTapeAudioProcessor::kParamFlutter, 80.0f);
    setParamPercent(proc, PhonkTapeAudioProcessor::kParamAge, 20.0f);
    setParamPercent(proc, PhonkTapeAudioProcessor::kParamNoise, 0.0f);
    setParamPercent(proc, PhonkTapeAudioProcessor::kParamMix, 100.0f);
    setParamPercent(proc, PhonkTapeAudioProcessor::kParamOutputGain, 0.0f);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int settle = 0; settle < 30; ++settle) {
      buf.clear();
      proc.processBlock(buf, midi);
    }

    float prev = 0.0f;
    float motion = 0.0f;
    for (int b = 0; b < 80; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.45f * std::sin(juce::MathConstants<float>::twoPi * 220.0f * t);
        buf.setSample(0, i, x);
        buf.setSample(1, i, x);
      }
      proc.processBlock(buf, midi);
      const float s = buf.getSample(0, kBlock / 2);
      motion += std::abs(s - prev);
      prev = s;
    }
    expect(motion > 0.05f, "high wow/flutter yields sample-to-sample motion");
    expect(allFinite(buf), "wow/flutter output finite");
  }

  // --- Ceiling -0.5 dBFS ---
  {
    PhonkTapeAudioProcessor p2;
    p2.prepareToPlay(kSr, kBlock);
    setParamPercent(p2, PhonkTapeAudioProcessor::kParamDrive, 100.0f);
    setParamPercent(p2, PhonkTapeAudioProcessor::kParamOutputGain, 12.0f);
    setParamPercent(p2, PhonkTapeAudioProcessor::kParamMix, 100.0f);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int b = 0; b < 24; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        buf.setSample(0, i, 0.99f);
        buf.setSample(1, i, 0.99f);
      }
      p2.processBlock(buf, midi);
    }
    const float peak = maxPeak(buf);
    expect(peak <= PhonkTapeDsp::kCeilingLinear + 1.0e-4f, "peak <= -0.5 dBFS ceiling");
  }

  // --- NaN in -> finite out ---
  {
    PhonkTapeAudioProcessor p3;
    p3.prepareToPlay(kSr, kBlock);
    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int i = 0; i < kBlock; ++i) {
      buf.setSample(0, i, std::numeric_limits<float>::quiet_NaN());
      buf.setSample(1, i, 0.1f);
    }
    p3.processBlock(buf, midi);
    expect(allFinite(buf), "NaN in yields finite out");
  }

  // --- Mix 0 dry passthrough (within latency / smoothing) ---
  {
    PhonkTapeAudioProcessor p4;
    setParamPercent(p4, PhonkTapeAudioProcessor::kParamMix, 0.0f);
    setParamPercent(p4, PhonkTapeAudioProcessor::kParamDrive, 0.0f);
    setParamPercent(p4, PhonkTapeAudioProcessor::kParamWow, 0.0f);
    setParamPercent(p4, PhonkTapeAudioProcessor::kParamFlutter, 0.0f);
    setParamPercent(p4, PhonkTapeAudioProcessor::kParamOutputGain, 0.0f);
    p4.prepareToPlay(kSr, kBlock);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    const int settleBlocks = static_cast<int>((0.2 * kSr) / kBlock) + 4;
    for (int b = 0; b < settleBlocks; ++b) {
      buf.clear();
      p4.processBlock(buf, midi);
    }

    double err = 0.0;
    int count = 0;
    for (int b = 0; b < 16; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.35f * std::sin(juce::MathConstants<float>::twoPi * 440.0f * t);
        buf.setSample(0, i, x);
        buf.setSample(1, i, x);
      }
      juce::AudioBuffer<float> ref;
      ref.makeCopyOf(buf, true);
      p4.processBlock(buf, midi);
      for (int ch = 0; ch < 2; ++ch) {
        for (int i = 0; i < kBlock; ++i) {
          const double d = static_cast<double>(buf.getSample(ch, i) - ref.getSample(ch, i));
          err += d * d;
          ++count;
        }
      }
    }
    const float rmsErr = static_cast<float>(std::sqrt(err / static_cast<double>(count)));
    expect(rmsErr < 1.0e-3f, "mix zero dry passthrough");
  }

  std::printf("Done. failures=%d\n", gFailures);
  return gFailures == 0 ? 0 : 1;
}
