/**
 * Offline contracts: ceiling (incl. 4x true-peak estimate), sub balance, latency, NaN safety.
 */
#include "../Source/PluginProcessor.h"
#include "../Source/StemShieldDsp.h"

#include <cmath>
#include <cstdio>
#include <limits>
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

float bufferTruePeak4x(const juce::AudioBuffer<float>& b) {
  float peak = 0.0f;
  for (int ch = 0; ch < b.getNumChannels(); ++ch) {
    const float* x = b.getReadPointer(ch);
    peak = std::fmax(peak, StemShieldDsp::estimateTruePeak4x(x, b.getNumSamples()));
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

  StemShieldAudioProcessor proc;
  setParamValue(proc, StemShieldAudioProcessor::kParamCeiling, -0.5f);
  setParamValue(proc, StemShieldAudioProcessor::kParamSubBalance, 65.0f);
  setParamValue(proc, StemShieldAudioProcessor::kParamSubCrossover, 80.0f);
  setParamValue(proc, StemShieldAudioProcessor::kParamRelease, 120.0f);
  setParamValue(proc, StemShieldAudioProcessor::kParamLookahead, 2.5f);
  setParamValue(proc, StemShieldAudioProcessor::kParamGain, 6.0f);
  proc.prepareToPlay(kSr, kBlock);

  expect(proc.getLatencySamples() > 0, "latency samples reported");

  // --- Hot input: true-peak estimate must not exceed ceiling ---
  {
    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    const float ceilingLinear =
        juce::Decibels::decibelsToGain(StemShieldDsp::kDefaultCeilingDbTp, -120.0f);
  const int settle = static_cast<int>((0.35 * kSr) / kBlock) + 6;
    float maxTp = 0.0f;
    for (int b = 0; b < settle; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.98f * std::sin(juce::MathConstants<float>::twoPi * 110.0f * t);
        buf.setSample(0, i, x);
        buf.setSample(1, i, x);
      }
      proc.processBlock(buf, midi);
      maxTp = std::fmax(maxTp, bufferTruePeak4x(buf));
    }
    expect(maxTp <= ceilingLinear + 2.0e-3f, "hot input true-peak <= ceiling");
    expect(allFinite(buf), "hot input output finite");
  }

  // --- Sub-heavy 50 Hz: balance reduces low-band dominance vs bypass ---
  {
    StemShieldAudioProcessor p2;
    setParamValue(p2, StemShieldAudioProcessor::kParamSubBalance, 100.0f);
    setParamValue(p2, StemShieldAudioProcessor::kParamGain, 0.0f);
    setParamValue(p2, StemShieldAudioProcessor::kParamCeiling, 0.0f);
    p2.prepareToPlay(kSr, kBlock);

    StemShieldAudioProcessor pDry;
    setParamValue(pDry, StemShieldAudioProcessor::kParamSubBalance, 0.0f);
    setParamValue(pDry, StemShieldAudioProcessor::kParamCeiling, 0.0f);
    setParamValue(pDry, StemShieldAudioProcessor::kParamGain, 0.0f);
    pDry.prepareToPlay(kSr, kBlock);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int b = 0; b < 60; ++b) {
      buf.clear();
      p2.processBlock(buf, midi);
      pDry.processBlock(buf, midi);
    }

    float rmsBalanced = 0.0f;
    float rmsDry = 0.0f;
    for (int b = 0; b < 30; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.9f * std::sin(juce::MathConstants<float>::twoPi * 48.0f * t);
        buf.setSample(0, i, x);
        buf.setSample(1, i, x * 0.2f);
      }
      juce::AudioBuffer<float> ref;
      ref.makeCopyOf(buf, true);
      p2.processBlock(buf, midi);
      pDry.processBlock(ref, midi);
      rmsBalanced += buf.getRMSLevel(0, 0, kBlock);
      rmsDry += ref.getRMSLevel(0, 0, kBlock);
    }
    rmsBalanced /= 30.0f;
    rmsDry /= 30.0f;
    expect(rmsBalanced < rmsDry * 0.98f, "sub balance attenuates hot sub");
  }

  // --- NaN in -> finite out ---
  {
    StemShieldAudioProcessor p3;
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

  // --- Factory presets load ---
  {
    StemShieldAudioProcessor p4;
    p4.prepareToPlay(kSr, kBlock);
    for (int i = 0; i < kStemShieldFactoryPresetCount; ++i) {
      p4.setCurrentProgram(i);
      expect(!p4.getProgramName(i).isEmpty(), "factory preset name");
    }
  }

  std::printf("Done. failures=%d\n", gFailures);
  return gFailures == 0 ? 0 : 1;
}
