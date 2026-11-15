#pragma once

#include <algorithm>
#include <cmath>

namespace play_series {

/** Customize per SKU: foundation layer (sub, analog osc, cowbell, EP tine, …). */
class LayerAEngine {
public:
  void prepare(double sampleRate) noexcept { sampleRate_ = sampleRate; }
  void reset() noexcept { phase_ = 0.0f; subPhase_ = 0.0f; }

  float nextSample(float midiNoteHz, float brightness, float subLevel) noexcept {
    const float inc = midiNoteHz / static_cast<float>(sampleRate_);
    phase_ += inc;
    if (phase_ >= 1.0f)
      phase_ -= 1.0f;
    subPhase_ += inc * 0.5f;
    if (subPhase_ >= 1.0f)
      subPhase_ -= 1.0f;

    const float saw = 2.0f * phase_ - 1.0f;
    const float tri = 1.0f - 4.0f * std::fabs(phase_ - 0.5f);
    const float body = tri * (1.0f - brightness) + saw * brightness;
  const float sub = std::sin(subPhase_ * 6.28318530717958647692f) * subLevel;
    const float mix = body * 0.45f + sub * 0.55f;
    return std::clamp(mix, -1.0f, 1.0f);
  }

private:
  double sampleRate_ = 48000.0;
  float phase_ = 0.0f;
  float subPhase_ = 0.0f;
};

} // namespace play_series