#pragma once

#include <cmath>

namespace dxfm {

inline float sine(float phase) { return std::sin(phase * 6.28318530718f); }

// 4-op FM: advances modulators per algorithm; returns carrier mix with levels
inline float processFmSample(float p1, float p2, float p3, float p4, int algo,
                              float l1, float l2, float l3, float l4,
                              float feedback, float &op1FeedbackState) {
  p1 += feedback * op1FeedbackState;

  float c1 = 0.0f, c2 = 0.0f, c3 = 0.0f, c4 = 0.0f;
  switch (algo & 7) {
  case 0:
    c4 = sine(p4);
    c3 = sine(p3 + c4);
    c2 = sine(p2 + c3);
    c1 = sine(p1 + c2);
    break;
  case 1:
    c4 = sine(p4);
    c3 = sine(p3);
    c2 = sine(p2 + c4 + c3);
    c1 = sine(p1 + c2);
    break;
  case 2:
    c2 = sine(p2);
    c4 = sine(p4 + c2 * 0.5f);
    c3 = sine(p3 + c2);
    c1 = sine(p1 + c3);
    break;
  case 3:
    c2 = sine(p2);
    c3 = sine(p3);
    c4 = sine(p4);
    c1 = sine(p1 + (c2 + c3 + c4) * 0.55f);
    break;
  case 4:
    c2 = sine(p2);
    c4 = sine(p4 + c2);
    c3 = sine(p3);
    c1 = sine(p1 + c4 + c3);
    break;
  case 5:
    c4 = sine(p4);
    c3 = sine(p3 + c4);
    c2 = sine(p2 + c3);
    c1 = sine(p1 + c2 * c3);
    break;
  case 6:
    c4 = sine(p4);
    c3 = sine(p3);
    c2 = sine(p2 + c4);
    c1 = sine(p1 + c2 + c3 * c4);
    break;
  default:
    c4 = sine(p4);
    c3 = sine(p3);
    c2 = sine(p2 + c4);
    c1 = sine(p1 + c2 + c3 * 0.4f);
    break;
  }

  const float out = c1 * l1 + c2 * l2 * 0.45f + c3 * l3 * 0.3f + c4 * l4 * 0.25f;
  op1FeedbackState = c1;
  return out;
}

} // namespace dxfm