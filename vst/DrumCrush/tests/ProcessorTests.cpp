/**
 * Offline contracts: punch energy, ceiling, NaN safety, bypass, latency reporting.
 */
#include "../Source/PluginProcessor.h"
#include "../Source/DrumCrushDsp.h"

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

void setParamPercent(DrumCrushAudioProcessor& p, const char* id, float pctOrValue) {
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

  DrumCrushAudioProcessor proc;
  proc.prepareToPlay(kSr, kBlock);
  expect(proc.getLatencySamples() > 0, "reports oversampling latency");

  // --- Impulse: punch increases HF energy ---
  {
    setParamPercent(proc, DrumCrushAudioProcessor::kParamPunch, 80.0f);
    setParamPercent(proc, DrumCrushAudioProcessor::kParamCrush, 50.0f);
    setParamPercent(proc, DrumCrushAudioProcessor::kParamClip, 50.0f);
    setParamPercent(proc, DrumCrushAudioProcessor::kParamMix, 100.0f);
    setParamPercent(proc, DrumCrushAudioProcessor::kParamOutputGain, 0.0f);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int b = 0; b < 12; ++b) {
      buf.clear();
      buf.setSample(0, 0, 0.9f);
      buf.setSample(1, 0, 0.9f);
      proc.processBlock(buf, midi);
    }
    float energy = 0.0f;
    for (int i = 0; i < kBlock; ++i) {
      const float s = buf.getSample(0, i);
      energy += s * s;
    }
    expect(energy > 0.05f, "punch processes impulse");
    expect(allFinite(buf), "impulse output finite");
  }

  // --- Ceiling -0.5 dBFS ---
  {
    DrumCrushAudioProcessor p2;
    p2.prepareToPlay(kSr, kBlock);
    setParamPercent(p2, DrumCrushAudioProcessor::kParamPunch, 80.0f);
    setParamPercent(p2, DrumCrushAudioProcessor::kParamCrush, 100.0f);
    setParamPercent(p2, DrumCrushAudioProcessor::kParamClip, 100.0f);
    setParamPercent(p2, DrumCrushAudioProcessor::kParamOutputGain, 12.0f);
    setParamPercent(p2, DrumCrushAudioProcessor::kParamMix, 100.0f);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int b = 0; b < 32; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        buf.setSample(0, i, 0.99f);
        buf.setSample(1, i, 0.99f);
      }
      p2.processBlock(buf, midi);
    }
    const float peak = maxPeak(buf);
    expect(peak <= DrumCrushDsp::kCeilingLinear + 1.0e-3f, "peak <= -0.5 dBFS ceiling");
  }

  // --- NaN in -> finite out ---
  {
    DrumCrushAudioProcessor p3;
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

  // --- Zero mix dry passthrough ---
  {
    DrumCrushAudioProcessor p4;
    setParamPercent(p4, DrumCrushAudioProcessor::kParamMix, 0.0f);
    setParamPercent(p4, DrumCrushAudioProcessor::kParamOutputGain, 0.0f);
    setParamPercent(p4, DrumCrushAudioProcessor::kParamPunch, 0.0f);
    setParamPercent(p4, DrumCrushAudioProcessor::kParamCrush, 0.0f);
    p4.prepareToPlay(kSr, kBlock);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    const int settle = static_cast<int>((0.25 * kSr) / kBlock) + 4;
    for (int b = 0; b < settle; ++b) {
      buf.clear();
      p4.processBlock(buf, midi);
    }

    double err = 0.0;
    int count = 0;
    for (int b = 0; b < 16; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.35f * std::sin(juce::MathConstants<float>::twoPi * 200.0f * t);
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
    expect(rmsErr < 1.0e-3f, "zero mix dry passthrough");
  }

  // --- State round-trip ---
  {
    DrumCrushAudioProcessor p5;
    p5.prepareToPlay(kSr, kBlock);
    setParamPercent(p5, DrumCrushAudioProcessor::kParamPunch, 33.0f);
    juce::MemoryBlock blob;
    p5.getStateInformation(blob);

    DrumCrushAudioProcessor p6;
    p6.setStateInformation(blob.getData(), static_cast<int>(blob.getSize()));
    const float punch =
        p6.getAPVTS().getRawParameterValue(DrumCrushAudioProcessor::kParamPunch)->load();
    expect(std::abs(punch - 33.0f) < 0.2f, "state save/restore punch");
  }

  std::printf("Done. failures=%d\n", gFailures);
  return gFailures == 0 ? 0 : 1;
}
