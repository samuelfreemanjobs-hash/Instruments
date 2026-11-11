#pragma once

#include "DrumPattern.h"
#include <juce_core/juce_core.h>

namespace DrumPatternIO {
inline uint16_t rotateRight(uint16_t mask, int steps) {
  steps &= 15;
  return static_cast<uint16_t>((mask << steps) | (mask >> (16 - steps)));
}

inline void shiftRight(DrumPattern &p, int steps = 1) {
  p.kick = rotateRight(p.kick, steps);
  p.snare = rotateRight(p.snare, steps);
  p.openHat = rotateRight(p.openHat, steps);
}

inline void shiftLeft(DrumPattern &p, int steps = 1) { shiftRight(p, 16 - (steps & 15)); }

inline juce::String toJson(const DrumPattern &p) {
  auto *obj = new juce::DynamicObject();
  obj->setProperty("kick", static_cast<int>(p.kick));
  obj->setProperty("snare", static_cast<int>(p.snare));
  obj->setProperty("openHat", static_cast<int>(p.openHat));
  return juce::JSON::toString(juce::var(obj));
}

inline DrumPattern fromJson(const juce::String &json) {
  DrumPattern p;
  const auto parsed = juce::JSON::parse(json);
  if (auto *obj = parsed.getDynamicObject()) {
    p.kick = static_cast<uint16_t>(static_cast<int>(obj->getProperty("kick")));
    p.snare = static_cast<uint16_t>(static_cast<int>(obj->getProperty("snare")));
    p.openHat = static_cast<uint16_t>(static_cast<int>(obj->getProperty("openHat")));
  }
  return p;
}
} // namespace DrumPatternIO
