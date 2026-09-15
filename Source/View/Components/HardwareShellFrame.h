#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
/**
 * Matte bezel + walnut side cheeks for the “persistent hardware shell” (Phase 2).
 * Place mode content in the centre viewport via setContentComponent().
 */
class HardwareShellFrame : public juce::Component
{
public:
    HardwareShellFrame();

    /** Host for mode UI; add children to this. */
    juce::Component& getContentHost() noexcept { return contentHost; }

    void paint(juce::Graphics& g) override;
    void resized() override;

    juce::Rectangle<int> viewportArea() const;

private:
    juce::Component contentHost;
    int cheekWidth = 28;
};
} // namespace vmpc::view
