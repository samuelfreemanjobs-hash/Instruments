#pragma once

#include <algorithm>
#include <cmath>

namespace play_series {

/** Customize per SKU: texture layer (choir formants, bells, tape flute, …). */
class LayerBEngine {
public:
  void prepare(double sampleRate) noexcept { sampleRate_ = sampleRate; }
  void reset() noexcept { phase_ = 0.0f; wtPhase_ = 0.0f; }

  float nextSample(float midiNoteHz, float wavetablePos, float airAmount) noexcept {
    const float inc = midiNoteHz / static_cast<float>(sampleRate_);
    phase_ += inc;
    if (phase_ >= 1.0f)
      phase_ -= 1.0f;

    wtPhase_ += inc * (1.0f + wavetablePos * 2.0f);
    if (wtPhase_ >= 1.0f)
      wtPhase_ -= 1.0f;

    const float square = phase_ < 0.5f ? 0.85f : -0.85f;
    const float shimmer =
        std::sin(wtPhase_ * 6.28318530717958647692f) * 0.35f +
        std::sin(wtPhase_ * 12.566370614359173f) * 0.2f;
    const float air = (square * 0.15f + shimmer) * airAmount;
    const float tone = square * (1.0f - airAmount * 0.5f) + shimmer * (0.5f + wavetablePos * 0.5f);
    return std::clamp(tone + air * 0.35f, -1.0f, 1.0f);
  }

private:
  double sampleRate_ = 48000.0;
  float phase_ = 0.0f;
  float wtPhase_ = 0.0f;
};

} // namespace play_series