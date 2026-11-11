/**
 * Contract: MeterTapAudioProcessor verification V3/V4/V5 (architecture.md §7).
 * V4: bypass on + trim +6 dB → output == input bitwise
 * V5: after 200 ms settle, trim −6 dB → output/input = 10^(-6/20) ±1e-4
 * V3: NaN in → finite out
 */
#include "../Source/PluginProcessor.h"

#include <cmath>
#include <cstdint>
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

void setParam(MeterTapAudioProcessor& p, const char* id, float value) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->convertTo0to1(value));
    param->endChangeGesture();
  }
}

void setBool(MeterTapAudioProcessor& p, const char* id, bool on) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(on ? 1.0f : 0.0f);
    param->endChangeGesture();
  }
}

} // namespace

int main() {
  constexpr double kSr = 48000.0;
  constexpr int kBlock = 512;

  MeterTapAudioProcessor proc;
  proc.prepareToPlay(kSr, kBlock);

  // --- V4 bypass bit-identical with trim +6 dB ---
  {
    setBool(proc, MeterTapAudioProcessor::kParamBypass, true);
    setParam(proc, MeterTapAudioProcessor::kParamTrimDb, 6.0f);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int i = 0; i < kBlock; ++i) {
      const float x = 0.25f * std::sin(0.01f * static_cast<float>(i));
      buf.setSample(0, i, x);
      buf.setSample(1, i, x * 0.5f);
    }
    juce::AudioBuffer<float> expected;
    expected.makeCopyOf(buf);

    proc.processBlock(buf, midi);

    bool identical = true;
    for (int ch = 0; ch < 2 && identical; ++ch) {
      for (int i = 0; i < kBlock; ++i) {
        // Bit-identical: compare raw float bits
        const auto a = juce::FloatVectorOperations::findMinAndMax(buf.getReadPointer(ch), kBlock);
        (void)a;
        union {
          float f;
          uint32_t u;
        } out{buf.getSample(ch, i)}, exp{expected.getSample(ch, i)};
        if (out.u != exp.u) {
          identical = false;
          break;
        }
      }
    }
    expect(identical, "V4 bypass bit-identical with trim +6 dB");
  }

  // --- V5 trim law −6 dB after settle ---
  {
    MeterTapAudioProcessor p2;
    p2.prepareToPlay(kSr, kBlock);
    setBool(p2, MeterTapAudioProcessor::kParamBypass, false);
    setParam(p2, MeterTapAudioProcessor::kParamTrimDb, -6.0f);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    const int settleBlocks = static_cast<int>(std::ceil(0.2 * kSr / kBlock)) + 2;
    for (int b = 0; b < settleBlocks; ++b) {
      for (int i = 0; i < kBlock; ++i) {
        buf.setSample(0, i, 0.5f);
        buf.setSample(1, i, 0.5f);
      }
      p2.processBlock(buf, midi);
    }

    const float expectedGain = std::pow(10.0f, -6.0f * 0.05f);
    const float out = buf.getSample(0, kBlock - 1);
    const float ratio = out / 0.5f;
    expect(std::fabs(ratio - expectedGain) <= 1.0e-4f, "V5 trim -6 dB within 1e-4 after settle");
  }

  // --- V3 NaN in → finite out ---
  {
    MeterTapAudioProcessor p3;
    p3.prepareToPlay(kSr, kBlock);
    setBool(p3, MeterTapAudioProcessor::kParamBypass, false);
    setParam(p3, MeterTapAudioProcessor::kParamTrimDb, 0.0f);

    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int i = 0; i < kBlock; ++i) {
      buf.setSample(0, i, (i == 10) ? std::numeric_limits<float>::quiet_NaN() : 0.1f);
      buf.setSample(1, i, (i == 20) ? std::numeric_limits<float>::infinity() : 0.1f);
    }
    p3.processBlock(buf, midi);

    bool finite = true;
    for (int ch = 0; ch < 2 && finite; ++ch) {
      for (int i = 0; i < kBlock; ++i) {
        if (!std::isfinite(buf.getSample(ch, i))) {
          finite = false;
          break;
        }
      }
    }
    expect(finite, "V3 NaN/Inf in -> finite out");
    expect(buf.getSample(0, 10) == 0.0f && buf.getSample(1, 20) == 0.0f,
           "V3 non-finite samples replaced with 0");
  }

  // Bypass parameter wiring
  {
    auto* bp = proc.getBypassParameter();
    expect(bp != nullptr && bp == proc.getAPVTS().getParameter(MeterTapAudioProcessor::kParamBypass),
           "getBypassParameter returns dl_bypass");
  }

  if (gFailures != 0) {
    std::fprintf(stderr, "%d failure(s)\n", gFailures);
    return 1;
  }
  std::printf("All MeterTap processor contracts passed\n");
  return 0;
}
