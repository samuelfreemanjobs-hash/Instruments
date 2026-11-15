#include "PhonkTapeDsp.h"

#include <cmath>

namespace {
constexpr float kTwoPi = juce::MathConstants<float>::twoPi;

uint32_t lcgNext(uint32_t& s) noexcept {
  s = s * 1664525u + 1013904223u;
  return s;
}

float pseudoNoise(uint32_t& seed) noexcept {
  const int v = static_cast<int>(lcgNext(seed) >> 9) & 0x7ffff;
  return static_cast<float>(v) / static_cast<float>(0x7ffff) * 2.0f - 1.0f;
}
} // namespace

void PhonkTapeDsp::prepare(const juce::dsp::ProcessSpec& spec) {
  sampleRate_ = spec.sampleRate;
  maxBlockSize_ = static_cast<int>(spec.maximumBlockSize);
  numChannels_ = static_cast<int>(spec.numChannels);

  const float sr = static_cast<float>(sampleRate_);
  baseDelaySamples_ = sr * 0.0105f; // 10.5 ms anchor keeps reads in the past
  maxWowDepthSamples_ = sr * 0.009f;   // up to 9 ms wow
  maxFlutterDepthSamples_ = sr * 0.0025f; // up to 2.5 ms flutter

  ringSize_ = juce::jmax(4096, static_cast<int>(sr * 0.06f) + 64);

  oversampling_ = std::make_unique<juce::dsp::Oversampling<float>>(
      static_cast<size_t>(numChannels_), 2,
      juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);
  oversampling_->initProcessing(static_cast<size_t>(maxBlockSize_));
  oversampling_->reset();

  dryBuffer_.setSize(numChannels_, maxBlockSize_);
  wetScratch_.setSize(numChannels_, maxBlockSize_);

  delayRing_.resize(static_cast<size_t>(numChannels_));
  writePos_.assign(static_cast<size_t>(numChannels_), 0.0f);
  ageLpState_.assign(static_cast<size_t>(numChannels_), 0.0f);
  noiseSeed_.assign(static_cast<size_t>(numChannels_), 0x1234567u);

  for (int ch = 0; ch < numChannels_; ++ch) {
    delayRing_[static_cast<size_t>(ch)].assign(static_cast<size_t>(ringSize_), 0.0f);
    noiseSeed_[static_cast<size_t>(ch)] = 0x9e3779b9u ^ static_cast<uint32_t>(ch * 0x1000193);
  }

  driveSmooth_.setTargetCoeff(10.0f, sr);
  wowSmooth_.setTargetCoeff(20.0f, sr);
  flutterSmooth_.setTargetCoeff(18.0f, sr);
  ageSmooth_.setTargetCoeff(40.0f, sr);
  noiseSmooth_.setTargetCoeff(12.0f, sr);
  mixSmooth_.setTargetCoeff(15.0f, sr);
  gainSmooth_.setTargetCoeff(15.0f, sr);

  driveSmooth_.reset(driveTarget_);
  wowSmooth_.reset(wowTarget_);
  flutterSmooth_.reset(flutterTarget_);
  ageSmooth_.reset(ageTarget_);
  noiseSmooth_.reset(noiseTarget_);
  mixSmooth_.reset(mixTarget_);
  gainSmooth_.reset(outputGainDbTarget_);

  reset();
}

void PhonkTapeDsp::reset() {
  wowPhase_ = 0.0f;
  flutterPhase_ = 0.0f;
  for (size_t ch = 0; ch < delayRing_.size(); ++ch) {
    std::fill(delayRing_[ch].begin(), delayRing_[ch].end(), 0.0f);
    writePos_[ch] = 0.0f;
    ageLpState_[ch] = 0.0f;
  }
  if (oversampling_) {
    oversampling_->reset();
  }
}

int PhonkTapeDsp::getLatencySamples() const noexcept {
  const int base = static_cast<int>(std::ceil(baseDelaySamples_ + maxWowDepthSamples_ + maxFlutterDepthSamples_));
  const int os = oversampling_ ? static_cast<int>(oversampling_->getLatencyInSamples()) : 0;
  return base + os;
}

float PhonkTapeDsp::readDelayLinear(const float* ring, int ringSize, float readIndex) noexcept {
  const int i0 = static_cast<int>(std::floor(readIndex));
  const float frac = readIndex - static_cast<float>(i0);
  const int wrapped0 = (i0 % ringSize + ringSize) % ringSize;
  const int wrapped1 = (wrapped0 + 1) % ringSize;
  return ring[wrapped0] * (1.0f - frac) + ring[wrapped1] * frac;
}

float PhonkTapeDsp::wowFlutterDelaySamples(float wowSm, float flutterSm) const noexcept {
  const float wow = std::sin(wowPhase_) * wowSm * maxWowDepthSamples_;
  const float flutter = std::sin(flutterPhase_) * flutterSm * maxFlutterDepthSamples_;
  return baseDelaySamples_ + wow + flutter;
}

float PhonkTapeDsp::tapeSaturate(float x, float drive) noexcept {
  if (drive <= 1.0e-6f) {
    return x;
  }
  const float k = 1.0f + drive * 6.5f;
  const float ax = std::abs(x);
  const float rational = (x * (1.0f + k)) / (1.0f + k * ax);
  const float soft = std::tanh(x * (1.0f + drive * 2.2f));
  const float y = rational * 0.65f + soft * 0.35f;
  return std::isfinite(y) ? y : 0.0f;
}

void PhonkTapeDsp::process(juce::AudioBuffer<float>& buffer) {
  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  if (numChannels < 1 || numSamples <= 0) {
    return;
  }

  dryBuffer_.makeCopyOf(buffer, true);

  const float wowInc = kWowHz / static_cast<float>(sampleRate_);
  const float flutterInc = kFlutterHz / static_cast<float>(sampleRate_);
  const float wowSm = wowSmooth_.process(wowTarget_);
  const float flutterSm = flutterSmooth_.process(flutterTarget_);
  const float driveSm = driveSmooth_.process(driveTarget_);
  const float ageSm = ageSmooth_.process(ageTarget_);
  const float noiseSm = noiseSmooth_.process(noiseTarget_);
  const float mix = mixSmooth_.process(mixTarget_);
  const float gain = juce::Decibels::decibelsToGain(gainSmooth_.process(outputGainDbTarget_));

  // AGE: one-pole LPF coefficient (18 kHz .. 2.8 kHz as age 0..1)
  const float ageHz = 18000.0f + ageSm * (2800.0f - 18000.0f);
  const float ageAlpha = std::exp(-kTwoPi * ageHz / static_cast<float>(sampleRate_));
  const float dryGain = 1.0f - mix;
  const float wetGain = mix;

  wetScratch_.makeCopyOf(buffer, true);

  for (int i = 0; i < numSamples; ++i) {
    const float delaySamp = wowFlutterDelaySamples(wowSm, flutterSm);

    for (int ch = 0; ch < numChannels; ++ch) {
      auto& ring = delayRing_[static_cast<size_t>(ch)];
      const float in = wetScratch_.getSample(ch, i);

      const int w = static_cast<int>(writePos_[static_cast<size_t>(ch)]) % ringSize_;
      ring[static_cast<size_t>(w)] = in;
      writePos_[static_cast<size_t>(ch)] += 1.0f;
      if (writePos_[static_cast<size_t>(ch)] >= static_cast<float>(ringSize_)) {
        writePos_[static_cast<size_t>(ch)] -= static_cast<float>(ringSize_);
      }

      const float readPos = writePos_[static_cast<size_t>(ch)] - delaySamp;
      float y = readDelayLinear(ring.data(), ringSize_, readPos);

      wetScratch_.setSample(ch, i, y);
    }

    wowPhase_ += kTwoPi * wowInc;
    flutterPhase_ += kTwoPi * flutterInc;
    if (wowPhase_ > kTwoPi) {
      wowPhase_ -= kTwoPi;
    }
    if (flutterPhase_ > kTwoPi) {
      flutterPhase_ -= kTwoPi;
    }
  }

  if (driveSm > 1.0e-5f) {
    juce::dsp::AudioBlock<float> wetBlock(wetScratch_);
    wetBlock = wetBlock.getSubBlock(0, static_cast<size_t>(numSamples));
    juce::dsp::AudioBlock<float> upBlock = oversampling_->processSamplesUp(wetBlock);
    for (size_t ch = 0; ch < upBlock.getNumChannels(); ++ch) {
      auto* up = upBlock.getChannelPointer(ch);
      for (size_t i = 0; i < upBlock.getNumSamples(); ++i) {
        up[i] = tapeSaturate(up[i], driveSm);
      }
    }
    oversampling_->processSamplesDown(wetBlock);
  }

  for (int ch = 0; ch < numChannels; ++ch) {
    float& lp = ageLpState_[static_cast<size_t>(ch)];
    auto& seed = noiseSeed_[static_cast<size_t>(ch)];
    auto* wet = wetScratch_.getWritePointer(ch);
    const float* dry = dryBuffer_.getReadPointer(ch);
    auto* out = buffer.getWritePointer(ch);

    for (int i = 0; i < numSamples; ++i) {
      lp = ageAlpha * lp + (1.0f - ageAlpha) * wet[i];
      float w = lp;

      if (noiseSm > 1.0e-5f) {
        const float hiss = pseudoNoise(seed) * noiseSm * 0.08f;
        w += hiss;
      }

      float y = dry[i] * dryGain + w * wetGain;
      y *= gain;
      if (!std::isfinite(y)) {
        y = 0.0f;
      }
      out[i] = juce::jlimit(-kCeilingLinear, kCeilingLinear, y);
    }
  }
}
