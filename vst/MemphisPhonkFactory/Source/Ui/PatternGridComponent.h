#pragma once

#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Dsp/DrumPattern.h"

class PatternGridComponent : public juce::Component {
public:
  PatternGridComponent();

  void setPattern(const DrumPattern &p);
  DrumPattern getPattern() const { return pattern_; }

  std::function<void(const DrumPattern &)> onPatternChanged;

  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  DrumPattern pattern_;
  juce::TextButton presetMemphis_{"Memphis"};
  juce::TextButton presetTrap_{"Trap sparse"};
  juce::TextButton clearButton_{"Clear"};

  struct Row {
    juce::String label;
    juce::Colour colour;
    std::array<juce::ToggleButton, 16> steps;
    std::function<uint16_t &(DrumPattern &)> mask;
    std::function<void(DrumPattern &, int)> toggler;
  };

  std::array<Row, 3> rows_{};

  void setupRow(int idx, const juce::String &label, juce::Colour c,
                std::function<uint16_t &(DrumPattern &)> mask,
                std::function<void(DrumPattern &, int)> toggler);
  void syncFromPattern();
  void notifyChange();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternGridComponent)
};
