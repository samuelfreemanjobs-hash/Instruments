#include "StemShieldDsp.h"

namespace {

constexpr float kMinCrossoverHz = 40.0f;
constexpr float kMaxCrossoverHz = 160.0f;

float safeCrossover(float hz, double sampleRate) noexcept {
  const float nyquistMargin = static_cast<float>(sampleRate) * 0.45f;
  return juce::jlimit(kMinCrossoverHz, juce::jmin(kMaxCrossoverHz, nyquistMargin), hz);
}

int msToSamples(float ms, double sampleRate) noexcept {
  return juce::jmax(1, static_cast<int>(std::lround(ms * 0.001 * sampleRate)));
}

float releaseCoeffFromMs(float ms, double sampleRate) noexcept {
  if (ms <= 0.1f) {
    return 1.0f;
  }
  const float tau = ms * 0.001f;
  return 1.0f - std::exp(-1.0f / (tau * static_cast<float>(sampleRate)));
}

int wrapIndex(int index, int size) noexcept {
  int w = index % size;
  if (w < 0) {
    w += size;
  }
  return w;
}

} // namespace

void StemShieldDsp::prepare(const juce::dsp::ProcessSpec& spec) {
  sampleRate_ = spec.sampleRate;
  maxBlockSize_ = static_cast<int>(spec.maximumBlockSize);
  numChannels_ = static_cast<int>(spec.numChannels);

  crossover_.prepare(spec);
  crossover_.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
  crossover_.setCutoffFrequency(safeCrossover(static_cast<float>(crossoverTargetHz_), sampleRate_));

  truePeakDetector_.prepare(sampleRate_);
  detectorGroupDelay_ = truePeakDetector_.getGroupDelaySamples();

  const int maxLookahead = msToSamples(12.0f, sampleRate_);
  lookaheadSamples_ = msToSamples(lookaheadMsTarget_, sampleRate_);
  delaySize_ = maxBlockSize_ + maxLookahead + detectorGroupDelay_ + 96;

  latencySamples_ = lookaheadSamples_ + detectorGroupDelay_;

  delayBuffer_.assign(static_cast<size_t>(numChannels_), std::vector<float>(static_cast<size_t>(delaySize_), 0.0f));
  limitedScratch_.assign(static_cast<size_t>(numChannels_), 0.0f);

  ceilingSmooth_.setTargetCoeff(20.0f, static_cast<float>(sampleRate_));
  subBalanceSmooth_.setTargetCoeff(25.0f, static_cast<float>(sampleRate_));
  crossoverSmooth_.setTargetCoeff(35.0f, static_cast<float>(sampleRate_));
  releaseSmooth_.setTargetCoeff(30.0f, static_cast<float>(sampleRate_));
  lookaheadSmooth_.setTargetCoeff(40.0f, static_cast<float>(sampleRate_));
  inputGainSmooth_.setTargetCoeff(15.0f, static_cast<float>(sampleRate_));

  ceilingSmooth_.reset(ceilingDbTarget_);
  subBalanceSmooth_.reset(subBalanceTarget_);
  crossoverSmooth_.reset(crossoverTargetHz_);
  releaseSmooth_.reset(releaseMsTarget_);
  lookaheadSmooth_.reset(lookaheadMsTarget_);
  inputGainSmooth_.reset(inputGainDbTarget_);

  writePos_ = 0;
  subLowEnv_ = 0.0f;
  subFullEnv_ = 0.0f;
  gainEnvelope_ = 1.0f;
  reset();
}

void StemShieldDsp::reset() {
  crossover_.reset();
  for (auto& ch : delayBuffer_) {
    std::fill(ch.begin(), ch.end(), 0.0f);
  }
  gainEnvelope_ = 1.0f;
  subLowEnv_ = 0.0f;
  subFullEnv_ = 0.0f;
}

float StemShieldDsp::linkedTruePeakInRange(int startIndex, int endIndex, int writeIndex) const noexcept {
  float linked = 0.0f;
  int idx = startIndex;
  for (;;) {
    for (int ch = 0; ch < numChannels_; ++ch) {
      const auto& line = delayBuffer_[static_cast<size_t>(ch)];
      auto fetch = [&](int sampleIndex) -> float {
        if (sampleIndex == writeIndex) {
          return limitedScratch_[static_cast<size_t>(ch)];
        }
        const int w = wrapIndex(sampleIndex, delaySize_);
        return line[static_cast<size_t>(w)];
      };
      linked = std::fmax(linked, truePeakDetector_.measureAt(fetch, idx));
    }
    if (idx == endIndex) {
      break;
    }
    idx = wrapIndex(idx + 1, delaySize_);
  }
  return linked;
}

void StemShieldDsp::process(juce::AudioBuffer<float>& buffer) {
  const int numSamples = buffer.getNumSamples();
  const int numChannels = juce::jmin(buffer.getNumChannels(), numChannels_);
  if (numSamples <= 0 || numChannels < 1) {
    return;
  }

  const float crossHz = crossoverSmooth_.process(crossoverTargetHz_);
  if (std::abs(crossHz - crossover_.getCutoffFrequency()) > 0.5f) {
    crossover_.setCutoffFrequency(safeCrossover(crossHz, sampleRate_));
  }

  const int newLookahead = msToSamples(lookaheadSmooth_.process(lookaheadMsTarget_), sampleRate_);
  if (newLookahead != lookaheadSamples_) {
    lookaheadSamples_ = newLookahead;
    latencySamples_ = lookaheadSamples_ + detectorGroupDelay_;
  }

  const float releaseMs = releaseSmooth_.process(releaseMsTarget_);
  const float releaseCoeff = releaseCoeffFromMs(releaseMs, sampleRate_);
  const float ceilingLinear =
      juce::Decibels::decibelsToGain(ceilingSmooth_.process(ceilingDbTarget_), -120.0f);
  const float subBalance = subBalanceSmooth_.process(subBalanceTarget_);
  const float inputGain = juce::Decibels::decibelsToGain(inputGainSmooth_.process(inputGainDbTarget_), -120.0f);

  const float subAttack = 1.0f - std::exp(-1.0f / (0.010f * static_cast<float>(sampleRate_)));
  const float subRelease = 1.0f - std::exp(-1.0f / (0.120f * static_cast<float>(sampleRate_)));

  blockOutputPeak_ = 0.0f;
  blockGrDb_ = 0.0f;

  for (int i = 0; i < numSamples; ++i) {
    for (int ch = 0; ch < numChannels; ++ch) {
      float in = buffer.getSample(ch, i) * inputGain;
      float low = 0.0f;
      float high = 0.0f;
      crossover_.processSample(ch, in, low, high);

      const float mixAbs = std::fabs(low) + std::fabs(high);
      const float lowAbs = std::fabs(low);
      if (lowAbs > subLowEnv_) {
        subLowEnv_ += (lowAbs - subLowEnv_) * subAttack;
      } else {
        subLowEnv_ += (lowAbs - subLowEnv_) * subRelease;
      }
      if (mixAbs > subFullEnv_) {
        subFullEnv_ += (mixAbs - subFullEnv_) * subAttack;
      } else {
        subFullEnv_ += (mixAbs - subFullEnv_) * subRelease;
      }

      constexpr float kTargetSubShare = 0.55f;
      const float share = subLowEnv_ / (subFullEnv_ + 1.0e-9f);
      float subGain = 1.0f;
      if (share > kTargetSubShare) {
        const float excess = juce::jlimit(0.0f, 1.0f, (share - kTargetSubShare) / kTargetSubShare);
        subGain = 1.0f - subBalance * excess;
      }
      low *= subGain;

      const float sum = low + high;
      limitedScratch_[static_cast<size_t>(ch)] = std::isfinite(sum) ? sum : 0.0f;
    }

    const int readPos =
        wrapIndex(writePos_ - lookaheadSamples_ - detectorGroupDelay_, delaySize_);

    const float linkedPeakAhead = linkedTruePeakInRange(readPos, writePos_, writePos_);

    float targetGain = 1.0f;
    if (linkedPeakAhead > ceilingLinear) {
      targetGain = ceilingLinear / (linkedPeakAhead + 1.0e-12f);
    }

    if (targetGain < gainEnvelope_) {
      gainEnvelope_ = targetGain;
    } else {
      gainEnvelope_ += (targetGain - gainEnvelope_) * releaseCoeff;
    }

    blockGrDb_ = std::fmin(blockGrDb_, juce::Decibels::gainToDecibels(gainEnvelope_, -120.0f));

    for (int ch = 0; ch < numChannels; ++ch) {
      auto& delay = delayBuffer_[static_cast<size_t>(ch)];
      float out = delay[static_cast<size_t>(readPos)] * gainEnvelope_;
      if (!std::isfinite(out)) {
        out = 0.0f;
      } else if (std::fabs(out) < 1.0e-15f) {
        out = 0.0f;
      }
      buffer.setSample(ch, i, out);
      blockOutputPeak_ = std::fmax(blockOutputPeak_, std::fabs(out));
      delay[static_cast<size_t>(writePos_)] = limitedScratch_[static_cast<size_t>(ch)];
    }

    writePos_ = wrapIndex(writePos_ + 1, delaySize_);
  }

  meters_.outputPeakDb.store(juce::Decibels::gainToDecibels(blockOutputPeak_, -100.0f));
  meters_.gainReductionDb.store(blockGrDb_);
}
