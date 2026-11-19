#include "MemphisGritDsp.h"

#include <cmath>

namespace {
constexpr float kSmoothMs = 12.0f;
constexpr float kGainSmoothMs = 15.0f;
} // namespace

void MemphisGritDsp::prepare(double sampleRate, int maxBlockSize, int numChannels) {
  sampleRate_ = sampleRate;
  maxBlockSize_ = maxBlockSize;
  numChannels_ = juce::jmax(1, numChannels);

  dryBuffer_.setSize(numChannels_, maxBlockSize_);

  holdCounters_.assign(static_cast<size_t>(numChannels_), 0);
  holdValues_.assign(static_cast<size_t>(numChannels_), 0.0f);
  hpState_.assign(static_cast<size_t>(numChannels_), 0.0f);

  const float sr = static_cast<float>(sampleRate_);
  driveSmooth_.setTargetCoeff(kSmoothMs, sr);
  lofiSmooth_.setTargetCoeff(kSmoothMs, sr);
  midSmooth_.setTargetCoeff(kSmoothMs, sr);
  mixSmooth_.setTargetCoeff(kSmoothMs, sr);
  gainSmooth_.setTargetCoeff(kGainSmoothMs, sr);

  driveSmooth_.reset(driveTarget_);
  lofiSmooth_.reset(lofiTarget_);
  midSmooth_.reset(midTarget_);
  mixSmooth_.reset(mixTarget_);
  gainSmooth_.reset(outputGainDbTarget_);

  reset();
}

void MemphisGritDsp::reset() {
  std::fill(holdCounters_.begin(), holdCounters_.end(), 0);
  std::fill(holdValues_.begin(), holdValues_.end(), 0.0f);
  std::fill(hpState_.begin(), hpState_.end(), 0.0f);
}

float MemphisGritDsp::softClip(float x, float drive) noexcept {
  const float g = 1.0f + drive * 11.0f;
  const float y = std::tanh(x * g);
  const float norm = std::tanh(g);
  return norm > 1.0e-6f ? y / norm : x;
}

float MemphisGritDsp::quantizeBits(float x, float lofiNorm) noexcept {
  if (lofiNorm <= 1.0e-5f) {
    return x;
  }
  const float bits = juce::jmap(lofiNorm, 16.0f, 5.0f);
  const float steps = std::pow(2.0f, bits - 1.0f);
  const float q = std::round(x * steps) / steps;
  return std::isfinite(q) ? q : 0.0f;
}

int MemphisGritDsp::holdInterval(float lofiNorm) noexcept {
  if (lofiNorm <= 1.0e-5f) {
    return 1;
  }
  return juce::jmax(1, static_cast<int>(std::lround(juce::jmap(lofiNorm, 1.0f, 48.0f))));
}

float MemphisGritDsp::applyMidGrit(float x, float midNorm) noexcept {
  if (midNorm <= 1.0e-5f) {
    return x;
  }
  // Asymmetric edge for midrange poke (Memphis crunch character).
  const float pos = juce::jmax(0.0f, x);
  const float neg = juce::jmin(0.0f, x);
  const float boosted = pos * (1.0f + midNorm * 0.9f) + neg * (1.0f + midNorm * 0.35f);
  return boosted;
}

void MemphisGritDsp::processChannel(float* samples, int numSamples, float drive, float lofi,
                                    float mid, int channel) noexcept {
  const int holdEvery = holdInterval(lofi);
  const float hpCoeff = 0.985f;
  float& hp = hpState_[static_cast<size_t>(channel)];
  int& holdCounter = holdCounters_[static_cast<size_t>(channel)];
  float& holdVal = holdValues_[static_cast<size_t>(channel)];

  for (int i = 0; i < numSamples; ++i) {
    float x = samples[i];
    const float hpOut = x - hp;
    hp += hpCoeff * hpOut;
    x = applyMidGrit(x + mid * hpOut * 0.35f, mid);
    x = softClip(x, drive);
    x = quantizeBits(x, lofi);

    if (holdEvery > 1) {
      if (holdCounter <= 0) {
        holdVal = x;
        holdCounter = holdEvery;
      }
      x = holdVal;
      --holdCounter;
    }

    if (!std::isfinite(x)) {
      x = 0.0f;
    }
    samples[i] = juce::jlimit(-kCeilingLinear, kCeilingLinear, x);
  }
}

void MemphisGritDsp::process(juce::AudioBuffer<float>& buffer) {
  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  if (numSamples <= 0 || numChannels < 1) {
    return;
  }

  dryBuffer_.makeCopyOf(buffer, true);

  const float drive = driveSmooth_.process(driveTarget_);
  const float lofi = lofiSmooth_.process(lofiTarget_);
  const float mid = midSmooth_.process(midTarget_);
  const float mix = mixSmooth_.process(mixTarget_);
  const float gain = juce::Decibels::decibelsToGain(gainSmooth_.process(outputGainDbTarget_));
  const float dryGain = 1.0f - mix;

  for (int ch = 0; ch < numChannels; ++ch) {
    processChannel(buffer.getWritePointer(ch), numSamples, drive, lofi, mid, ch);
  }

  for (int ch = 0; ch < numChannels; ++ch) {
    const float* dry = dryBuffer_.getReadPointer(ch);
    float* wet = buffer.getWritePointer(ch);
    for (int i = 0; i < numSamples; ++i) {
      float out = dry[i] * dryGain + wet[i] * mix;
      out *= gain;
      out = juce::jlimit(-kCeilingLinear, kCeilingLinear, out);
      wet[i] = std::isfinite(out) ? out : 0.0f;
    }
  }
}
