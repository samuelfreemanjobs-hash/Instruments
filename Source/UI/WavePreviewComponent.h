#pragma once

#include "Assets/CleanroomWaveLibrary.h"
#include "Assets/RomBank.h"
#include "DSP/PcmWaveform.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>

/** Mini waveform plot for ROM or fallback wave index (UI thread only). */
class WavePreviewComponent final : public juce::Component
{
public:
    void setWaveSource (const jdupgraded::assets::RomBank* bank,
                        const jdupgraded::assets::CleanroomWaveLibrary* fallback,
                        int waveIndex) noexcept
    {
        bank_ = bank;
        fallback_ = fallback;
        waveIndex_ = waveIndex;
        rebuildPath();
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced (2.0f);
        g.setColour (juce::Colour (0xff121218));
        g.fillRoundedRectangle (bounds, 4.0f);
        g.setColour (juce::Colours::white.withAlpha (0.25f));
        g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

        if (pathPoints_.size() < 2)
            return;

        juce::Path path;
        const float w = bounds.getWidth();
        const float h = bounds.getHeight();
        const float midY = bounds.getCentreY();
        const float halfH = h * 0.42f;

        for (std::size_t i = 0; i < pathPoints_.size(); ++i)
        {
            const float x = bounds.getX() + (static_cast<float> (i) / static_cast<float> (pathPoints_.size() - 1)) * w;
            const float y = midY - pathPoints_[i] * halfH;
            if (i == 0)
                path.startNewSubPath (x, y);
            else
                path.lineTo (x, y);
        }

        g.setColour (juce::Colour (0xff6ec8ff));
        g.strokePath (path, juce::PathStrokeType (1.5f));
    }

private:
    void rebuildPath() noexcept
    {
        pathPoints_.clear();
        const jdupgraded::dsp::PcmWaveform* wave = nullptr;

        if (bank_ != nullptr && bank_->isLoaded())
            wave = &bank_->getWave (static_cast<std::size_t> (waveIndex_));
        else if (fallback_ != nullptr)
            wave = &fallback_->getWave (static_cast<std::size_t> (waveIndex_));

        if (wave == nullptr || ! wave->isValid())
            return;

        constexpr std::size_t kPlotPoints = 128;
        const std::size_t frames = wave->numFrames;
        pathPoints_.resize (kPlotPoints);

        for (std::size_t i = 0; i < kPlotPoints; ++i)
        {
            const std::size_t frame = (i * frames) / kPlotPoints;
            pathPoints_[i] = wave->samples[frame % frames];
        }
    }

    const jdupgraded::assets::RomBank* bank_ = nullptr;
    const jdupgraded::assets::CleanroomWaveLibrary* fallback_ = nullptr;
    int waveIndex_ = 0;
    std::vector<float> pathPoints_;
};
