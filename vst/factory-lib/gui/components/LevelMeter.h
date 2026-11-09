#pragma once

#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

enum class MeterBarSize { sm, md, lg, xl };

/**
 * Vertical level meter (display only). Ballistics live in factory-lib/analysis;
 * this widget takes prepared dB values via setLevels().
 */
class LevelMeter : public juce::Component
{
public:
    explicit LevelMeter (MeterBarSize barSize = MeterBarSize::md);

    void setLevels (float peakDb, float rmsDb, float holdDb, bool overLatched);
    void setBarSize (MeterBarSize s);

    std::function<void()> onClear;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;
    bool keyPressed (const juce::KeyPress&) override;

    /** Sample a pixel colour for render tests (local coords). */
    juce::Colour samplePixel (int x, int y) const;

    float getPeakDb() const noexcept { return peakDb; }
    float getRmsDb() const noexcept { return rmsDb; }
    float getHoldDb() const noexcept { return holdDb; }
    bool isOver() const noexcept { return over; }

private:
    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override;
    int barWidth() const;

    MeterBarSize barSize;
    float peakDb = meter::defaultFloorDb;
    float rmsDb = meter::defaultFloorDb;
    float holdDb = meter::defaultFloorDb;
    bool over = false;

    juce::Rectangle<float> barBounds;
    juce::Rectangle<float> overCapBounds;
    juce::Path trackPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};

} // namespace gui
} // namespace fds
