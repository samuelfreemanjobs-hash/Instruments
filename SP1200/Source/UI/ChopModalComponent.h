#pragma once

#include "../Engine/SamplerEngine.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

/** MOD 11 — waveform chop / truncate editor overlay. */
class ChopModalComponent : public juce::Component
{
public:
    ChopModalComponent (sp1200::SamplerEngine& engine, std::size_t segmentIndex, std::function<void()> onClosed);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

private:
    void refreshWaveform();
    void sampleFromX (int x, std::int64_t& outSample) const;
    juce::Rectangle<int> waveformBounds() const;

    sp1200::SamplerEngine& engine_;
    std::size_t segmentIndex_;
    std::function<void()> onClosed_;

    std::vector<float> waveform_;
    std::int64_t startSample_ = 0;
    std::int64_t endSample_ = 0;
    enum class DragTarget
    {
        none,
        start,
        end
    };
    DragTarget drag_ = DragTarget::none;

    juce::Label titleLabel_;
    juce::TextButton truncateButton_ { "TRUNCATE" };
    juce::TextButton chop8Button_ { "AUTO 8" };
    juce::TextButton chop16Button_ { "AUTO 16" };
    juce::TextButton normalizeButton_ { "NORMALIZE" };
    juce::TextButton reverseButton_ { "REVERSE" };
    juce::TextButton previewButton_ { "PREVIEW" };
    juce::TextButton closeButton_ { "CLOSE" };
};
