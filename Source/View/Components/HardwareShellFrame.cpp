#include "HardwareShellFrame.h"

namespace vmpc::view
{
HardwareShellFrame::HardwareShellFrame()
{
    setOpaque(true);
    addAndMakeVisible(contentHost);
}

juce::Rectangle<int> HardwareShellFrame::viewportArea() const
{
    return getLocalBounds().reduced(cheekWidth, 12).withTrimmedTop(8).withTrimmedBottom(10);
}

void HardwareShellFrame::paint(juce::Graphics& g)
{
    const auto matte = juce::Colour(0xff0c0c0e);
    const auto walnut = juce::Colour(0xff3d2b1f);
    const auto walnutHi = walnut.brighter(0.12f);

    g.fillAll(matte);

    auto bounds = getLocalBounds();
    g.setColour(walnut);
    g.fillRect(bounds.removeFromLeft(cheekWidth));
    g.fillRect(bounds.removeFromRight(cheekWidth));

    g.setColour(walnutHi);
    g.drawVerticalLine(cheekWidth, 0.0f, static_cast<float>(getHeight()));
    g.drawVerticalLine(getWidth() - cheekWidth, 0.0f, static_cast<float>(getHeight()));

    auto vp = viewportArea().toFloat();
    g.setColour(juce::Colour(0xff1a1a20));
    g.fillRoundedRectangle(vp, 6.0f);
    g.setColour(juce::Colour(0xff6ec8e8).withAlpha(0.2f));
    g.drawRoundedRectangle(vp, 6.0f, 1.0f);
}

void HardwareShellFrame::resized()
{
    contentHost.setBounds(viewportArea());
}
} // namespace vmpc::view
