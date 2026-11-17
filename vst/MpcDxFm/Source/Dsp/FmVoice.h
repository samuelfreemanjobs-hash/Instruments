#pragma once

#include "FmAlgorithms4.h"
#include <cmath>

namespace dxfm {

struct VoiceParams {
  float opLevel[4]{1.f, 0.7f, 0.5f, 0.6f};
  float opRatio[4]{1.f, 2.f, 1.f, 3.f};
  float opDetuneCents[4]{};
  int algo = 3;
  float feedback = 0.12f;
};

struct FmVoice {
  bool active = false;
  int note = -1;
  float baseFreq = 110.0f;
  float velocity = 0.0f;
  VoiceParams params{};
  float phase[4]{};
  float op1FeedbackState = 0.0f;
  float envLevel = 0.0f;
  float envIncAttack = 0.0f;
  float envIncDecay = 0.0f;
  float envIncRelease = 0.0f;
  float envSustain = 0.75f;
  int envStage = 0; // 0 atk 1 dec 2 sus 3 rel

  void startNote(int midiNote, float vel, double sampleRate, float a, float d,
                 float s, float r, const VoiceParams &p) {
    active = true;
    note = midiNote;
    velocity = vel;
    params = p;
    baseFreq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    op1FeedbackState = 0.0f;
    envLevel = 0.0f;
    envStage = 0;
    envSustain = s;
    const float sr = static_cast<float>(sampleRate);
    envIncAttack = (a > 0.001f) ? 1.0f / (a * sr) : 1.0f;
    envIncDecay = (d > 0.001f) ? (1.0f - s) / (d * sr) : (1.0f - s);
    envIncRelease = (r > 0.001f) ? s / (r * sr) : s;
  }

  void noteOff() {
    if (active)
      envStage = 3;
  }

  void advanceEnvelope() {
    if (!active)
      return;
    switch (envStage) {
    case 0:
      envLevel += envIncAttack;
      if (envLevel >= 1.0f) {
        envLevel = 1.0f;
        envStage = 1;
      }
      break;
    case 1:
      envLevel -= envIncDecay;
      if (envLevel <= envSustain) {
        envLevel = envSustain;
        envStage = 2;
      }
      break;
    case 2:
      envLevel = envSustain;
      break;
    case 3:
      envLevel -= envIncRelease;
      if (envLevel <= 0.0f) {
        envLevel = 0.0f;
        active = false;
      }
      break;
    default:
      break;
    }
  }

  float renderSample(double sampleRate, float lfoPitchMod) {
    if (!active)
      return 0.0f;
    advanceEnvelope();

    const float sr = static_cast<float>(sampleRate);
    float p[4]{};
    for (int i = 0; i < 4; ++i) {
      const float det = std::pow(2.0f, params.opDetuneCents[i] / 1200.0f);
      const float inc =
          (baseFreq * params.opRatio[i] * det * (1.0f + lfoPitchMod)) / sr;
      phase[i] += inc;
      if (phase[i] > 1.0f)
        phase[i] -= 1.0f;
      p[i] = phase[i];
    }

    float y = processFmSample(
        p[0], p[1], p[2], p[3], params.algo, params.opLevel[0],
        params.opLevel[1], params.opLevel[2], params.opLevel[3], params.feedback,
        op1FeedbackState);

    y = std::tanh(y * 0.85f) * envLevel * velocity * 0.55f;
    return y;
  }
};

} // namespace dxfm