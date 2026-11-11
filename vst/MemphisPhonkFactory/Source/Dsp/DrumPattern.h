#pragma once

#include <cstdint>

/** One bar = 16 steps; bit `step` set means hit on that step (repeats each bar). */
struct DrumPattern {
  uint16_t kick = 0;
  uint16_t snare = 0;
  uint16_t openHat = 0;

  static DrumPattern memphisClassic() {
    DrumPattern p;
    for (int s : {0, 3, 7, 10, 14}) {
      p.kick |= static_cast<uint16_t>(1u << s);
    }
    for (int s : {4, 12}) {
      p.snare |= static_cast<uint16_t>(1u << s);
    }
    for (int s : {2, 10}) {
      p.openHat |= static_cast<uint16_t>(1u << s);
    }
    return p;
  }

  static DrumPattern trapSparse() {
    DrumPattern p;
    for (int s : {0, 8}) {
      p.kick |= static_cast<uint16_t>(1u << s);
    }
    for (int s : {4, 12}) {
      p.snare |= static_cast<uint16_t>(1u << s);
    }
    for (int s : {6, 14}) {
      p.openHat |= static_cast<uint16_t>(1u << s);
    }
    return p;
  }

  bool stepKick(int stepInBar) const {
    return (kick & (1u << (stepInBar & 15))) != 0;
  }
  bool stepSnare(int stepInBar) const {
    return (snare & (1u << (stepInBar & 15))) != 0;
  }
  bool stepOpenHat(int stepInBar) const {
    return (openHat & (1u << (stepInBar & 15))) != 0;
  }

  void toggleKick(int step) { kick ^= static_cast<uint16_t>(1u << (step & 15)); }
  void toggleSnare(int step) { snare ^= static_cast<uint16_t>(1u << (step & 15)); }
  void toggleOpenHat(int step) { openHat ^= static_cast<uint16_t>(1u << (step & 15)); }

  bool operator==(const DrumPattern &o) const {
    return kick == o.kick && snare == o.snare && openHat == o.openHat;
  }
  bool operator!=(const DrumPattern &o) const { return !(*this == o); }
};
