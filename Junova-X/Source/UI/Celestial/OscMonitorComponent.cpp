#include "OscMonitorComponent.h"

#include "Parameters/ParameterIds.h"
#include "PluginProcessor.h"

namespace junovax::ui::celestial
{
namespace
{
float previewSample (float phase, int wave) noexcept
{
    switch (wave)
    {
        case 1:
            return phase < 0.5f ? 1.0f : -1.0f;
        case 2:
            return (phase < 0.25f) ? 1.0f : ((phase < 0.5f) ? -0.6f : 0.3f);
        case 0:
        default:
            return std::sin (phase * juce::MathConstants<float>::twoPi);
    }
}
} // namespace

OscMonitorComponent::OscMonitorComponent (JunovaXAudioProcessor& processor)
    : processor_ (processor)
{
    setOpaque (true);
    startTimerHz (30);
}

void OscMonitorComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (4.0f);
    g.setColour (juce::Colour (0xff050508));
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (Theme::electricCyan().withAlpha (0.35f));
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

    g.setFont (Theme::labelFont());
    g.setColour (Theme::textMuted());
    g.drawText ("OLED MONITOR // REAL-TIME DSP",
                bounds.removeFromTop (14.0f).reduced (6.0f, 0.0f),
                juce::Justification::centredLeft);

    float peak = 0.0f;
    for (const auto s : samples_)
        peak = juce::jmax (peak, std::abs (s));

    int lfoWave = 0;
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (
            processor_.getApvts().getParameter (junovax::ParameterIDs::lfoWaveform)))
        lfoWave = p->getIndex();

    float lfoRate = 4.2f;
    if (auto* v = processor_.getApvts().getRawParameterValue (junovax::ParameterIDs::lfoRate))
        lfoRate = v->load();

    previewPhase_ += lfoRate * 0.008f;
    if (previewPhase_ > 1.0f)
        previewPhase_ -= 1.0f;

    const bool useLive = peak > 1.0e-5f;
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();
    const float mid = bounds.getCentreY();

    juce::Path pathL, pathR;
    for (int i = 0; i < static_cast<int> (samples_.size()); ++i)
    {
        const float x = bounds.getX() + (static_cast<float> (i) / static_cast<float> (samples_.size() - 1)) * w;
        float sample = samples_[static_cast<std::size_t> (i)];
        const float ph = previewPhase_ + static_cast<float> (i) / static_cast<float> (samples_.size());
        if (! useLive)
            sample = previewSample (ph - std::floor (ph), lfoWave) * 0.55f;
        const float yL = mid - sample * h * 0.38f;
        float sampleR = samples_[static_cast<std::size_t> ((i + 48) % samples_.size())];
        if (! useLive)
            sampleR = previewSample (ph + 0.12f - std::floor (ph + 0.12f), lfoWave) * 0.55f;
        const float yR2 = mid - sampleR * h * 0.32f;

        if (i == 0)
        {
            pathL.startNewSubPath (x, yL);
            pathR.startNewSubPath (x, yR2);
        }
        else
        {
            pathL.lineTo (x, yL);
            pathR.lineTo (x, yR2);
        }
    }

    g.setColour (Theme::stellarCrimson().withAlpha (0.9f));
    g.strokePath (pathL, juce::PathStrokeType (1.8f));
    g.setColour (Theme::electricCyan().withAlpha (0.9f));
    g.strokePath (pathR, juce::PathStrokeType (1.8f));
}

void OscMonitorComponent::timerCallback()
{
    processor_.copyScopeSamples (samples_.data(), static_cast<int> (samples_.size()));
    repaint();
}
} // namespace junovax::ui::celestial