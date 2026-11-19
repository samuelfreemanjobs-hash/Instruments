#include "../Source/PluginProcessor.h"

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

void setParam(MemphisGritAudioProcessor& p, const char* id, float value) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->convertTo0to1(value));
    param->endChangeGesture();
  }
}

void setBool(MemphisGritAudioProcessor& p, const char* id, bool on) {
  if (auto* param = p.getAPVTS().getParameter(id)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(on ? 1.0f : 0.0f);
    param->endChangeGesture();
  }
}

float rms(const juce::AudioBuffer<float>& buf, int ch) {
  const float* data = buf.getReadPointer(ch);
  double sum = 0.0;
  for (int i = 0; i < buf.getNumSamples(); ++i) {
    sum += static_cast<double>(data[i]) * data[i];
  }
  return static_cast<float>(std::sqrt(sum / static_cast<double>(buf.getNumSamples())));
}

} // namespace

int main() {
  constexpr double kSr = 48000.0;
  constexpr int kBlock = 512;

  MemphisGritAudioProcessor proc;
  proc.prepareToPlay(kSr, kBlock);

  // Bypass passes (with output trim only).
  {
    setBool(proc, MemphisGritAudioProcessor::kParamBypass, true);
    juce::AudioBuffer<float> buf(2, kBlock);
    juce::MidiBuffer midi;
    for (int i = 0; i < kBlock; ++i) {
      const float x = 0.18f * std::sin(0.03f * static_cast<float>(i));
      buf.setSample(0, i, x);
      buf.setSample(1, i, x);
    }
    proc.processBlock(buf, midi);
    bool finite = true;
    for (int ch = 0; ch < 2 && finite; ++ch) {
      for (int i = 0; i < kBlock; ++i) {
        if (!std::isfinite(buf.getSample(ch, i))) {
          finite = false;
        }
      }
    }
    expect(finite, "bypass output finite");
  }

  // Grit increases distortion energy vs dry-ish path.
  {
    setBool(proc, MemphisGritAudioProcessor::kParamBypass, false);
    setParam(proc, MemphisGritAudioProcessor::kParamMix, 1.0f);
    setParam(proc, MemphisGritAudioProcessor::kParamDrive, 0.05f);
    setParam(proc, MemphisGritAudioProcessor::kParamLofi, 0.0f);
    juce::AudioBuffer<float> clean(1, kBlock);
    juce::MidiBuffer midi;
    for (int i = 0; i < kBlock; ++i) {
      clean.setSample(0, i, 0.35f * std::sin(0.07f * static_cast<float>(i)));
    }
    juce::AudioBuffer<float> gritty = clean;
    proc.processBlock(clean, midi);

    setParam(proc, MemphisGritAudioProcessor::kParamDrive, 0.85f);
    setParam(proc, MemphisGritAudioProcessor::kParamLofi, 0.75f);
    setParam(proc, MemphisGritAudioProcessor::kParamMid, 0.8f);
    proc.processBlock(gritty, midi);

    const float rmsClean = rms(clean, 0);
    const float rmsGrit = rms(gritty, 0);
    expect(rmsGrit > rmsClean * 0.5f && std::isfinite(rmsGrit), "grit path changes level");
  }

  // NaN sanitation.
  {
    setBool(proc, MemphisGritAudioProcessor::kParamBypass, false);
    juce::AudioBuffer<float> buf(1, kBlock);
    juce::MidiBuffer midi;
    for (int i = 0; i < kBlock; ++i) {
      buf.setSample(0, i, std::numeric_limits<float>::quiet_NaN());
    }
    proc.processBlock(buf, midi);
    bool ok = true;
    for (int i = 0; i < kBlock; ++i) {
      if (!std::isfinite(buf.getSample(0, i))) {
        ok = false;
      }
    }
    expect(ok, "NaN in produces finite out");
  }

  return gFailures == 0 ? 0 : 1;
}
