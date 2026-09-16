#pragma once
/*
 * ADSR envelope generator (per-sample), output 0..1.
 */
#include <cmath>
#include <cstdint>

struct DspAdsr {
  enum class Stage : uint8_t { kIdle, kAttack, kDecay, kSustain, kRelease };

  float attackSec = 0.01f;
  float decaySec = 0.1f;
  float sustainLevel = 0.7f;
  float releaseSec = 0.2f;
  float sampleRate = 48000.0f;

  Stage stage = Stage::kIdle;
  float level = 0.0f;

  void reset() {
    stage = Stage::kIdle;
    level = 0.0f;
  }

  void setSampleRate(float sr) { sampleRate = sr > 0.0f ? sr : 48000.0f; }

  void gateOn() {
    stage = Stage::kAttack;
  }

  void gateOff() {
    if (stage != Stage::kIdle) {
      stage = Stage::kRelease;
    }
  }

  bool isActive() const { return stage != Stage::kIdle; }

  float process() {
    if (stage == Stage::kIdle) {
      return 0.0f;
    }

    const float attackCoef = coefForTime(attackSec);
    const float decayCoef = coefForTime(decaySec);
    const float releaseCoef = coefForTime(releaseSec);

    switch (stage) {
    case Stage::kAttack:
      level += (1.0f - level) * attackCoef;
      if (level >= 0.999f) {
        level = 1.0f;
        stage = Stage::kDecay;
      }
      break;
    case Stage::kDecay:
      level += (sustainLevel - level) * decayCoef;
      if (std::fabs(level - sustainLevel) < 1.0e-5f) {
        level = sustainLevel;
        stage = Stage::kSustain;
      }
      break;
    case Stage::kSustain:
      level = sustainLevel;
      break;
    case Stage::kRelease:
      level *= (1.0f - releaseCoef);
      if (level < 1.0e-6f) {
        level = 0.0f;
        stage = Stage::kIdle;
      }
      break;
    default:
      break;
    }
    return level;
  }

private:
  float coefForTime(float seconds) const {
    if (seconds <= 0.0f) {
      return 1.0f;
    }
    return 1.0f - std::exp(-1.0f / (seconds * sampleRate));
  }
};
