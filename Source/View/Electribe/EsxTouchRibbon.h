#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
class EsxTouchRibbon : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    juce::Point<float> dot { 0.5f, 0.4f };
};
} // namespace vmpc::view
