#include "PunisherDsp.h"

#include <cmath>

namespace {
constexpr float kMinCrossoverHz = 40.0f;
constexpr float kMaxCrossoverHz = 120.0f;

float safeCrossover(float hz, double sampleRate) noexcept {
  const float nyquistMargin = static_cast<float>(sampleRate) * 0.45f;
  return juce::jlimit(kMinCrossoverHz, juce::jmin(kMaxCrossoverHz, nyquistMargin), hz);
}
} // namespace

void PunisherDsp::prepare(const juce::dsp::ProcessSpec& spec) {
  sampleRate_ = spec.sampleRate;
  maxBlockSize_ = static_cast<int>(spec.maximumBlockSize);
  numChannels_ = static_cast<int>(spec.numChannels);

  crossover_.prepare(spec);
  crossover_.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
  crossover_.setCutoffFrequency(safeCrossover(crossoverTargetHz_, static_cast<float>(sampleRate_)));

  oversampling_ = std::make_unique<juce::dsp::Oversampling<float>>(
      static_cast<size_t>(numChannels_), 2,
      juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);
  oversampling_->initProcessing(static_cast<size_t>(maxBlockSize_));
  oversampling_->reset();

  dryBuffer_.setSize(numChannels_, maxBlockSize_);
  lowBandBuffer_.setSize(numChannels_, maxBlockSize_);
  highBandBuffer_.setSize(numChannels_, maxBlockSize_);
  harmBuffer_.setSize(numChannels_, maxBlockSize_);

  punishSmooth_.setTargetCoeff(12.0f, static_cast<float>(sampleRate_));
  shapeSmooth_.setTargetCoeff(12.0f, static_cast<float>(sampleRate_));
  mixSmooth_.setTargetCoeff(15.0f, static_cast<float>(sampleRate_));
  gainSmooth_.setTargetCoeff(15.0f, static_cast<float>(sampleRate_));
  crossoverSmooth_.setTargetCoeff(30.0f, static_cast<float>(sampleRate_));

  punishSmooth_.reset(punishTarget_);
  shapeSmooth_.reset(shapeTarget_);
  mixSmooth_.reset(mixTarget_);
  gainSmooth_.reset(outputGainDbTarget_);
  crossoverSmooth_.reset(crossoverTargetHz_);

  reset();
}

void PunisherDsp::reset() {
  crossover_.reset();
  if (oversampling_) {
    oversampling_->reset();
  }
}

int PunisherDsp::getLatencySamples() const noexcept {
  return oversampling_ ? static_cast<int>(oversampling_->getLatencyInSamples()) : 0;
}

float PunisherDsp::waveshapeHarmonic(float low, float punish, float shape) noexcept {
  if (punish <= 1.0e-6f) {
    return 0.0f;
  }

  const float drive = 1.0f + punish * 9.0f;
  const float x = low * drive;
  const float even = x * std::abs(x) / drive;
  const float odd = std::tanh(x * x * x * 0.25f);
  const float blended = even + shape * (odd - even);
  const float delta = (blended - low) * punish;
  if (!std::isfinite(delta)) {
    return 0.0f;
  }
  return delta;
}

void PunisherDsp::process(juce::AudioBuffer<float>& buffer) {
  const int numSamples = buffer.getNumSamples();
  const int numChannels = buffer.getNumChannels();
  if (numChannels < 1 || numSamples <= 0) {
    return;
  }

  const float crossHz = crossoverSmooth_.process(crossoverTargetHz_);
  if (std::abs(crossHz - crossover_.getCutoffFrequency()) > 0.5f) {
    crossover_.setCutoffFrequency(safeCrossover(crossHz, sampleRate_));
  }

  dryBuffer_.makeCopyOf(buffer, true);

  for (int ch = 0; ch < numChannels; ++ch) {
    const float* in = buffer.getReadPointer(ch);
    auto* lowPtr = lowBandBuffer_.getWritePointer(ch);
    auto* highPtr = highBandBuffer_.getWritePointer(ch);
    for (int i = 0; i < numSamples; ++i) {
      crossover_.processSample(ch, in[i], lowPtr[i], highPtr[i]);
    }
  }

  const float punishForHarm = punishSmooth_.process(punishTarget_);
  const float shapeForHarm = shapeSmooth_.process(shapeTarget_);
  harmBuffer_.clear();
  if (punishForHarm > 1.0e-6f) {
    harmBuffer_.makeCopyOf(lowBandBuffer_, true);
    juce::dsp::AudioBlock<float> lowBlock(harmBuffer_);
    lowBlock = lowBlock.getSubBlock(0, static_cast<size_t>(numSamples));
    juce::dsp::AudioBlock<float> upBlock = oversampling_->processSamplesUp(lowBlock);
    for (size_t ch = 0; ch < upBlock.getNumChannels(); ++ch) {
      auto* up = upBlock.getChannelPointer(ch);
      for (size_t i = 0; i < upBlock.getNumSamples(); ++i) {
        up[i] = waveshapeHarmonic(up[i], punishForHarm, shapeForHarm);
      }
    }
    oversampling_->processSamplesDown(lowBlock);
  }

  const float mix = mixSmooth_.process(mixTarget_);
  const float gain = juce::Decibels::decibelsToGain(gainSmooth_.process(outputGainDbTarget_));
  const float dryGain = 1.0f - mix;
  const float wetGain = mix;

  for (int ch = 0; ch < numChannels; ++ch) {
    const float* dry = dryBuffer_.getReadPointer(ch);
    const float* low = lowBandBuffer_.getReadPointer(ch);
    const float* high = highBandBuffer_.getReadPointer(ch);
    const float* harm = harmBuffer_.getReadPointer(ch);
    auto* out = buffer.getWritePointer(ch);

    for (int i = 0; i < numSamples; ++i) {
      const float wet = punishForHarm > 1.0e-6f ? (low[i] + high[i] + harm[i]) : dry[i];
      float y = dry[i] * dryGain + wet * wetGain;
      y *= gain;
      if (!std::isfinite(y)) {
        y = 0.0f;
      }
      out[i] = juce::jlimit(-kCeilingLinear, kCeilingLinear, y);
    }
  }
}
