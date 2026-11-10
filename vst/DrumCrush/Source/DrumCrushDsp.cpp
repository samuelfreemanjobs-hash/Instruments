#include "DrumCrushDsp.h"

#include <cmath>

namespace {
constexpr float kFastDecay = 0.992f;
constexpr float kSlowAttack = 0.018f;
constexpr float kShelfLpHz = 2200.0f;

float shelfLowpassCoeff(double sampleRate) noexcept {
  const float tau = 1.0f / (juce::MathConstants<float>::twoPi * kShelfLpHz);
  const float dt = 1.0f / static_cast<float>(sampleRate);
  return dt / (tau + dt);
}
} // namespace

void DrumCrushDsp::prepare(const juce::dsp::ProcessSpec& spec) {
  sampleRate_ = spec.sampleRate;
  maxBlockSize_ = static_cast<int>(spec.maximumBlockSize);
  numChannels_ = static_cast<int>(spec.numChannels);

  oversampling_ = std::make_unique<juce::dsp::Oversampling<float>>(
      static_cast<size_t>(numChannels_), 2,
      juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);
  oversampling_->initProcessing(static_cast<size_t>(maxBlockSize_));
  oversampling_->reset();

  peakLimiter_.prepare(spec);
  peakLimiter_.setRelease(80.0f);
  peakLimiter_.setThreshold(-0.3f);

  dryBuffer_.setSize(numChannels_, maxBlockSize_);
  wetBuffer_.setSize(numChannels_, maxBlockSize_);

  envFast_.assign(static_cast<size_t>(numChannels_), 0.0f);
  envSlow_.assign(static_cast<size_t>(numChannels_), 0.0f);
  shelfLowpass_.assign(static_cast<size_t>(numChannels_), 0.0f);

  punchSmooth_.setTargetCoeff(10.0f, static_cast<float>(sampleRate_));
  crushSmooth_.setTargetCoeff(12.0f, static_cast<float>(sampleRate_));
  clipSmooth_.setTargetCoeff(10.0f, static_cast<float>(sampleRate_));
  mixSmooth_.setTargetCoeff(15.0f, static_cast<float>(sampleRate_));
  gainSmooth_.setTargetCoeff(15.0f, static_cast<float>(sampleRate_));

  punchSmooth_.reset(punchTarget_);
  crushSmooth_.reset(crushTarget_);
  clipSmooth_.reset(clipTarget_);
  mixSmooth_.reset(mixTarget_);
  gainSmooth_.reset(outputGainDbTarget_);

  reset();
}

void DrumCrushDsp::reset() {
  if (oversampling_) {
    oversampling_->reset();
  }
  peakLimiter_.reset();
  std::fill(envFast_.begin(), envFast_.end(), 0.0f);
  std::fill(envSlow_.begin(), envSlow_.end(), 0.0f);
  std::fill(shelfLowpass_.begin(), shelfLowpass_.end(), 0.0f);
}

int DrumCrushDsp::getLatencySamples() const noexcept {
  return oversampling_ ? static_cast<int>(oversampling_->getLatencyInSamples()) : 0;
}

float DrumCrushDsp::saturateCrush(float x, float crushNorm) noexcept {
  if (crushNorm <= 1.0e-6f) {
    return x;
  }
  const float drive = 1.0f + crushNorm * 14.0f;
  const float yEven = x + (x * std::abs(x)) * (0.35f * crushNorm);
  const float yOdd = std::tanh(x * drive);
  const float y = yEven * (1.0f - crushNorm * 0.35f) + yOdd * (crushNorm * 0.65f + 0.35f);
  return std::isfinite(y) ? y : 0.0f;
}

float DrumCrushDsp::applySoftClip(float x, float clipNorm) noexcept {
  if (clipNorm <= 1.0e-6f) {
    return x;
  }
  const float preGain = 1.0f + clipNorm * 5.5f;
  const float z = x * preGain;
  const float knee = 0.85f + (1.0f - clipNorm) * 0.12f;
  const float y = z / (knee + std::abs(z));
  const float out = y / preGain * (1.0f + clipNorm * 0.25f);
  return std::isfinite(out) ? out : 0.0f;
}

void DrumCrushDsp::processWetChannel(float* samples, int numSamples, int channel, float punch,
                                     float crush, float clip) noexcept {
  juce::ignoreUnused(crush);

  float& fast = envFast_[static_cast<size_t>(channel)];
  float& slow = envSlow_[static_cast<size_t>(channel)];
  float& shelfLp = shelfLowpass_[static_cast<size_t>(channel)];
  const float shelfCoeff = shelfLowpassCoeff(sampleRate_);
  const float shelfAmt = punch * 2.2f;

  for (int i = 0; i < numSamples; ++i) {
    const float x = samples[i];
    const float rect = std::abs(x);
    fast = std::fmax(rect, fast * kFastDecay);
    slow += (rect - slow) * kSlowAttack;
    const float transient = juce::jmax(0.0f, fast - slow * 1.15f);
    shelfLp += shelfCoeff * (x - shelfLp);
    const float high = x - shelfLp;
    float y = x + transient * punch * 2.8f + high * shelfAmt;
    y = applySoftClip(y, clip);
    samples[i] = y;
  }
}

void DrumCrushDsp::process(juce::AudioBuffer<float>& buffer) {
  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  if (numChannels < 1 || numSamples <= 0) {
    return;
  }

  const float punch = punchSmooth_.process(punchTarget_);
  const float crush = crushSmooth_.process(crushTarget_);
  const float clip = clipSmooth_.process(clipTarget_);
  const float mix = mixSmooth_.process(mixTarget_);
  const float gain = juce::Decibels::decibelsToGain(gainSmooth_.process(outputGainDbTarget_));

  dryBuffer_.makeCopyOf(buffer, true);
  wetBuffer_.makeCopyOf(buffer, true);

  for (int ch = 0; ch < numChannels; ++ch) {
    processWetChannel(wetBuffer_.getWritePointer(ch), numSamples, ch, punch, crush, clip);
  }

  juce::dsp::AudioBlock<float> wetBlock(wetBuffer_);
  wetBlock = wetBlock.getSubBlock(0, static_cast<size_t>(numSamples));
  juce::dsp::AudioBlock<float> upBlock = oversampling_->processSamplesUp(wetBlock);

  if (crush > 1.0e-4f) {
    for (size_t ch = 0; ch < upBlock.getNumChannels(); ++ch) {
      auto* up = upBlock.getChannelPointer(ch);
      for (size_t i = 0; i < upBlock.getNumSamples(); ++i) {
        up[i] = saturateCrush(up[i], crush);
      }
    }
  }
  oversampling_->processSamplesDown(wetBlock);

  juce::dsp::ProcessContextReplacing<float> limitContext(wetBlock);
  peakLimiter_.process(limitContext);

  const float dryGain = 1.0f - mix;
  const float wetGain = mix;

  for (int ch = 0; ch < numChannels; ++ch) {
    const float* dry = dryBuffer_.getReadPointer(ch);
    const float* wet = wetBuffer_.getReadPointer(ch);
    auto* out = buffer.getWritePointer(ch);

    for (int i = 0; i < numSamples; ++i) {
      float y = dry[i] * dryGain + wet[i] * wetGain;
      y *= gain;
      if (!std::isfinite(y)) {
        y = 0.0f;
      }
      out[i] = juce::jlimit(-kCeilingLinear, kCeilingLinear, y);
    }
  }
}
