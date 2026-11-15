/**
 * Offline contracts: sub preservation, harmonic generation, ceiling, NaN safety, bypass-ish null.
 */
#include "../Source/PluginProcessor.h"
#include "../Source/PunisherDsp.h"

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

void setParamPercent(Punisher808AudioProcessor& p, const char* id, float pctOrValue) {
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

  Punisher808AudioProcessor proc;
  proc.prepareToPlay(kSr, kBlock);

  // --- 50 Hz sine: harmonics increase energy vs dry when punish on ---
  {
    setParamPercent(proc, Punisher808AudioProcessor::kParamPunish, 70.0f);
    setParamPercent(proc, Punisher808AudioProcessor::kParamHarmonicShape, 70.0f);
    setParamPercent(proc, Punisher808AudioProcessor::kParamSubCrossover, 60.0f);
    setParamPercent(proc, Punisher808AudioProcessor::kParamMix, 100.0f);
    setParamPercent(proc, Punisher808AudioProcessor::kParamOutputGain, 0.0f);

    juce::AudioBuffer<float> dry(2, kBlock);
    juce::MidiBuffer midi;
    const int settle = static_cast<int>((0.15 * kSr) / kBlock) + 4;
    float dryRms = 0.0f;
    float wetRms = 0.0f;

    juce::AudioBuffer<float> wet(2, kBlock);
    for (int blk = 0; blk < settle; ++blk) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(blk * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.5f * std::sin(juce::MathConstants<float>::twoPi * 50.0f * t);
        dry.setSample(0, i, x);
        dry.setSample(1, i, x);
      }
      wet.makeCopyOf(dry, true);
      proc.processBlock(wet, midi);
      dryRms += dry.getRMSLevel(0, 0, kBlock);
      wetRms += wet.getRMSLevel(0, 0, kBlock);
    }
    dryRms /= static_cast<float>(settle);
    wetRms /= static_cast<float>(settle);
    expect(wetRms > dryRms * 1.02f, "50 Hz punish adds harmonic energy");
    expect(allFinite(wet), "50 Hz output finite");
  }

  // --- Ceiling -0.5 dBFS ---
  {
    Punisher808AudioProcessor p2;
    p2.prepareToPlay(kSr, kBlock);
    setParamPercent(p2, Punisher808AudioProcessor::kParamPunish, 100.0f);
    setParamPercent(p2, Punisher808AudioProcessor::kParamOutputGain, 12.0f);
    setParamPercent(p2, Punisher808AudioProcessor::kParamMix, 100.0f);

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
    expect(peak <= PunisherDsp::kCeilingLinear + 1.0e-4f, "peak <= -0.5 dBFS ceiling");
  }

  // --- NaN in -> finite out ---
  {
    Punisher808AudioProcessor p3;
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

  // --- Null-ish: zero punish + zero mix (dry passthrough contract) ---
  {
    Punisher808AudioProcessor p4;
    setParamPercent(p4, Punisher808AudioProcessor::kParamPunish, 0.0f);
    setParamPercent(p4, Punisher808AudioProcessor::kParamMix, 0.0f);
    setParamPercent(p4, Punisher808AudioProcessor::kParamOutputGain, 0.0f);
    p4.prepareToPlay(kSr, kBlock);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    const int settleBlocks = static_cast<int>((0.15 * kSr) / kBlock) + 2;
    for (int b = 0; b < settleBlocks; ++b) {
      buf.clear();
      p4.processBlock(buf, midi);
    }

    const int blocks = static_cast<int>((0.2 * kSr) / kBlock) + 2;
    double err = 0.0;
    int count = 0;
    for (int b = 0; b < blocks; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.35f * std::sin(juce::MathConstants<float>::twoPi * 90.0f * t);
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
    expect(rmsErr < 1.0e-4f, "zero punish zero mix dry passthrough");
  }

  // --- Zero punish wet mix: LR sum without new harmonics ---
  {
    Punisher808AudioProcessor p5;
    setParamPercent(p5, Punisher808AudioProcessor::kParamPunish, 0.0f);
    setParamPercent(p5, Punisher808AudioProcessor::kParamMix, 100.0f);
    setParamPercent(p5, Punisher808AudioProcessor::kParamOutputGain, 0.0f);
    setParamPercent(p5, Punisher808AudioProcessor::kParamSubCrossover, 60.0f);
    p5.prepareToPlay(kSr, kBlock);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int b = 0; b < 40; ++b) {
      buf.clear();
      p5.processBlock(buf, midi);
    }
    double err = 0.0;
    int count = 0;
    for (int b = 0; b < 20; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        const float t = static_cast<float>(b * kBlock + i) / static_cast<float>(kSr);
        const float x = 0.4f * std::sin(juce::MathConstants<float>::twoPi * 50.0f * t);
        buf.setSample(0, i, x);
        buf.setSample(1, i, x);
      }
      juce::AudioBuffer<float> ref;
      ref.makeCopyOf(buf, true);
      p5.processBlock(buf, midi);
      for (int ch = 0; ch < 2; ++ch) {
        for (int i = 0; i < kBlock; ++i) {
          const double d = static_cast<double>(buf.getSample(ch, i) - ref.getSample(ch, i));
          err += d * d;
          ++count;
        }
      }
    }
    const float rmsErr = static_cast<float>(std::sqrt(err / static_cast<double>(count)));
    expect(rmsErr < 0.12f, "zero punish wet mix LR reconstruction");
  }

  std::printf("Done. failures=%d\n", gFailures);
  return gFailures == 0 ? 0 : 1;
}
