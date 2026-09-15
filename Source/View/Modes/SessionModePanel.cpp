#include "SessionModePanel.h"

namespace resonance::view
{
SessionModePanel::SessionModePanel(controller::AppController& controller)
    : appController(controller)
{
}

void SessionModePanel::mouseDown(const juce::MouseEvent& e)
{
    const int tracks = resonance::model::SessionMatrix::kTracks;
    const int scenes = resonance::model::SessionMatrix::kScenes;
    const auto area = getLocalBounds().withTrimmedTop(36).reduced(16);
    const int cw = area.getWidth() / tracks;
    const int ch = area.getHeight() / scenes;
    const int t = (e.x - area.getX()) / cw;
    const int s = (e.y - area.getY()) / ch;
    if (t < 0 || t >= tracks || s < 0 || s >= scenes)
        return;

    auto& matrix = appController.getWorkspace().getSessionMatrix();
    auto& clip = matrix.clip(t, s);
    clip.hasClip = true;
    clip.clipName = "Clip " + juce::String(t + 1) + ":" + juce::String(s + 1);
    clip.isPlaying = !clip.isPlaying;
    repaint();
}

void SessionModePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101820));
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.drawText("Ableton Session — clip matrix, quantize, scene launch", getLocalBounds().removeFromTop(32),
               juce::Justification::centred);

    const auto& matrix = appController.getWorkspace().getSessionMatrix();
    const int tracks = resonance::model::SessionMatrix::kTracks;
    const int scenes = resonance::model::SessionMatrix::kScenes;
    const auto area = getLocalBounds().withTrimmedTop(36).reduced(16);
    const int cw = area.getWidth() / tracks;
    const int ch = area.getHeight() / scenes;

    for (int t = 0; t < tracks; ++t)
    {
        for (int s = 0; s < scenes; ++s)
        {
            auto r = juce::Rectangle<int>(area.getX() + t * cw, area.getY() + s * ch, cw - 4, ch - 4);
            const auto& clip = matrix.clip(t, s);
            g.setColour(clip.isPlaying ? juce::Colour(0xff3d9a5a) : juce::Colour(0xff252a34));
            g.fillRoundedRectangle(r.toFloat(), 4.0f);
            if (clip.hasClip)
            {
                g.setColour(juce::Colours::white.withAlpha(0.7f));
                g.setFont(10.0f);
                g.drawText(clip.clipName, r.reduced(4), juce::Justification::topLeft);
            }
        }
    }

    g.setColour(juce::Colour(0xff6ec8e8).withAlpha(0.6f));
    g.drawText("Quantize: " + juce::String(matrix.getQuantizeBeats(), 2) + " beats",
               getLocalBounds().removeFromBottom(20), juce::Justification::centred);
}
} // namespace resonance::view
