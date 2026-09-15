#include "MaschineModePanel.h"

namespace resonance::view
{
MaschineModePanel::MaschineModePanel(controller::AppController& controller)
    : appController(controller)
{
    groupBtn.setButtonText(appController.getWorkspace().getMaschineKit().getGroup(0).name);

    groupBtn.onClick = [this]() {
        auto& kit = appController.getWorkspace().getMaschineKit();
        const int next = (kit.getActiveGroup() + 1) % resonance::model::MaschineKit::kGroups;
        kit.setActiveGroup(next);
        groupBtn.setButtonText(kit.getGroup(next).name);
        repaint();
    };
    addAndMakeVisible(groupBtn);

    for (int s = 0; s < 8; ++s)
    {
        auto* b = sceneButtons.add(new juce::TextButton("S" + juce::String(s + 1)));
        b->onClick = [this, s]() {
            appController.getWorkspace().getMaschineKit().setActiveScene(s);
            repaint();
        };
        addAndMakeVisible(b);
    }
}

int MaschineModePanel::hitPad(juce::Point<int> pos) const
{
    const int cols = 4;
    const int pad = 8;
    const auto area = getLocalBounds().withTrimmedTop(80).withTrimmedBottom(48).reduced(24);
    const int w = (area.getWidth() - pad * (cols + 1)) / cols;
    const int h = w;

    for (int i = 0; i < 16; ++i)
    {
        const int col = i % cols;
        const int row = i / cols;
        auto r = juce::Rectangle<int>(area.getX() + pad + col * (w + pad), area.getY() + pad + row * (h + pad), w, h);
        if (r.contains(pos))
            return i;
    }
    return -1;
}

void MaschineModePanel::mouseDown(const juce::MouseEvent& e)
{
    const int pad = hitPad(e.getPosition());
    if (pad < 0)
        return;

    auto& kit = appController.getWorkspace().getMaschineKit();
    auto& p = kit.getGroup(kit.getActiveGroup()).pads[static_cast<size_t>(pad)];
    p.hasSample = !p.hasSample;
    repaint();
}

void MaschineModePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff141418));
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.setFont(14.0f);
    g.drawText("Maschine — Groups, 16 pads, Scenes", getLocalBounds().removeFromTop(28), juce::Justification::centred);

    const auto& kit = appController.getWorkspace().getMaschineKit();
    const int cols = 4;
    const int pad = 8;
    const auto area = getLocalBounds().withTrimmedTop(80).withTrimmedBottom(48).reduced(24);
    const int w = (area.getWidth() - pad * (cols + 1)) / cols;
    const int h = w;

    for (int i = 0; i < 16; ++i)
    {
        const int col = i % cols;
        const int row = i / cols;
        auto r = juce::Rectangle<float>(static_cast<float>(area.getX() + pad + col * (w + pad)),
                                        static_cast<float>(area.getY() + pad + row * (h + pad)),
                                        static_cast<float>(w),
                                        static_cast<float>(h));
        const auto& p = kit.getGroup(kit.getActiveGroup()).pads[static_cast<size_t>(i)];
        g.setColour(p.hasSample ? juce::Colour(0xffe85d04) : juce::Colour(0xff2a2a32));
        g.fillRoundedRectangle(r, 6.0f);
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawRoundedRectangle(r, 6.0f, 1.0f);
    }

    g.setColour(juce::Colour(0xff6ec8e8));
    g.drawText("Scene " + juce::String(kit.getActiveScene() + 1), getLocalBounds().removeFromBottom(24), juce::Justification::centred);
}

void MaschineModePanel::resized()
{
    groupBtn.setBounds(getLocalBounds().removeFromTop(48).removeFromLeft(120).reduced(12));
    auto sceneRow = getLocalBounds().removeFromBottom(40).reduced(12);
    const int sw = sceneRow.getWidth() / juce::jmax(1, sceneButtons.size());
    for (auto* b : sceneButtons)
        b->setBounds(sceneRow.removeFromLeft(sw).reduced(2));
}
} // namespace resonance::view
