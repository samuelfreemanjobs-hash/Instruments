#include "PatternGridComponent.h"

PatternGridComponent::PatternGridComponent() {
  pattern_ = DrumPattern::memphisClassic();

  setupRow(0, "Kick", juce::Colour(0xffe86c2a), [](DrumPattern &p) -> uint16_t & { return p.kick; },
           [](DrumPattern &p, int s) { p.toggleKick(s); });
  setupRow(1, "Snare", juce::Colour(0xffc4a882),
           [](DrumPattern &p) -> uint16_t & { return p.snare; },
           [](DrumPattern &p, int s) { p.toggleSnare(s); });
  setupRow(2, "Open hat", juce::Colour(0xff6eb5ff),
           [](DrumPattern &p) -> uint16_t & { return p.openHat; },
           [](DrumPattern &p, int s) { p.toggleOpenHat(s); });

  addAndMakeVisible(presetMemphis_);
  addAndMakeVisible(presetTrap_);
  addAndMakeVisible(clearButton_);

  presetMemphis_.onClick = [this] {
    setPattern(DrumPattern::memphisClassic());
    notifyChange();
  };
  presetTrap_.onClick = [this] {
    setPattern(DrumPattern::trapSparse());
    notifyChange();
  };
  clearButton_.onClick = [this] {
    setPattern({});
    notifyChange();
  };

  syncFromPattern();
}

void PatternGridComponent::setupRow(int idx, const juce::String &label, juce::Colour c,
                                    std::function<uint16_t &(DrumPattern &)> mask,
                                    std::function<void(DrumPattern &, int)> toggler) {
  auto &row = rows_[static_cast<size_t>(idx)];
  row.label = label;
  row.colour = c;
  row.mask = mask;
  row.toggler = toggler;
  for (int s = 0; s < 16; ++s) {
    auto &btn = rows_[static_cast<size_t>(idx)].steps[static_cast<size_t>(s)];
    btn.setButtonText(juce::String(s + 1));
    btn.setClickingTogglesState(true);
    btn.onClick = [this, idx, s] {
      rows_[static_cast<size_t>(idx)].toggler(pattern_, s);
      syncFromPattern();
      notifyChange();
    };
    addAndMakeVisible(btn);
  }
}

void PatternGridComponent::setPattern(const DrumPattern &p) {
  pattern_ = p;
  syncFromPattern();
}

void PatternGridComponent::syncFromPattern() {
  for (size_t r = 0; r < rows_.size(); ++r) {
    const uint16_t mask = (r == 0) ? pattern_.kick : (r == 1) ? pattern_.snare : pattern_.openHat;
    for (int s = 0; s < 16; ++s) {
      rows_[r].steps[static_cast<size_t>(s)].setToggleState((mask & (1u << s)) != 0,
                                                            juce::dontSendNotification);
    }
  }
}

void PatternGridComponent::notifyChange() {
  if (onPatternChanged) {
    onPatternChanged(pattern_);
  }
}

void PatternGridComponent::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::white.withAlpha(0.85f));
  g.setFont(12.0f);
  g.drawText("16-step pattern (closed hats on every off-step)", getLocalBounds().removeFromTop(18),
             juce::Justification::centredLeft);
}

void PatternGridComponent::resized() {
  auto area = getLocalBounds().reduced(4);
  area.removeFromTop(20);
  auto presetRow = area.removeFromTop(28);
  presetMemphis_.setBounds(presetRow.removeFromLeft(90).reduced(2));
  presetTrap_.setBounds(presetRow.removeFromLeft(90).reduced(2));
  clearButton_.setBounds(presetRow.removeFromLeft(70).reduced(2));

  const int labelW = 64;
  const int stepH = 26;
  for (size_t r = 0; r < rows_.size(); ++r) {
    auto line = area.removeFromTop(stepH);
    auto label = line.removeFromLeft(labelW);
    (void)label;
    const int stepW = line.getWidth() / 16;
    for (int s = 0; s < 16; ++s) {
      rows_[r].steps[static_cast<size_t>(s)].setBounds(line.removeFromLeft(stepW).reduced(1));
    }
    area.removeFromTop(4);
  }
}
