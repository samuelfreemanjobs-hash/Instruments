#include "EsxPartsMatrix.h"
#include "View/Design/StudioColours.h"

namespace vmpc::view
{
juce::Rectangle<int> EsxPartsMatrix::cellBounds(int index) const
{
    const int cols = 7;
    const int pad = 4;
    const int w = (getWidth() - pad * (cols + 1)) / cols;
    const int rows = (vmpc::model::ElectribeSong::kNumParts + cols - 1) / cols;
    const int h = (getHeight() - pad * (rows + 1)) / juce::jmax(1, rows);
    const int col = index % cols;
    const int row = index / cols;
    return { pad + col * (w + pad), pad + row * (h + pad), w, h };
}

int EsxPartsMatrix::hitTestPart(juce::Point<int> pos) const
{
    if (song == nullptr)
        return -1;
    for (int i = 0; i < vmpc::model::ElectribeSong::kNumParts; ++i)
        if (cellBounds(i).contains(pos))
            return i;
    return -1;
}

void EsxPartsMatrix::mouseDown(const juce::MouseEvent& e)
{
    const int idx = hitTestPart(e.getPosition());
    if (idx < 0 || song == nullptr)
        return;

    auto& part = song->getPart(idx);
    const auto cell = cellBounds(idx);
    const auto local = e.getPosition() - cell.getPosition();

    if (local.y > cell.getHeight() - 18)
    {
        if (local.x < cell.getWidth() / 2)
            part.solo = !part.solo;
        else
            part.muted = !part.muted;
    }
    else
    {
        song->setSelectedPart(idx);
        if (partChanged)
            partChanged(idx);
    }
    repaint();
}

void EsxPartsMatrix::paint(juce::Graphics& g)
{
    g.fillAll(studio::obsidian2());
    g.setColour(studio::walnutTrim());
    g.drawRect(getLocalBounds(), 1);

    if (song == nullptr)
        return;

    g.setFont(juce::FontOptions(9.0f));
    for (int i = 0; i < vmpc::model::ElectribeSong::kNumParts; ++i)
    {
        const auto& part = song->getPart(i);
        auto r = cellBounds(i);
        const bool sel = song->getSelectedPart() == i;
        g.setColour(sel ? studio::obsidian3().brighter(0.08f) : studio::obsidian3());
        g.fillRoundedRectangle(r.toFloat(), 4.0f);
        if (sel)
        {
            g.setColour(studio::iceOled());
            g.drawRoundedRectangle(r.toFloat().reduced(0.5f), 4.0f, 1.5f);
        }

        g.setColour(juce::Colours::white.withAlpha(0.55f));
        g.drawText(part.name, r.reduced(4).removeFromTop(12), juce::Justification::topLeft);
        g.setColour(studio::iceOled());
        g.drawText(part.sampleTag, r.reduced(4).withTrimmedTop(12).removeFromTop(14), juce::Justification::topLeft);

        auto ledRow = r.removeFromBottom(16).reduced(4, 2);
        g.setColour(part.solo ? studio::iceOled() : studio::obsidian1());
        g.fillRoundedRectangle(ledRow.removeFromLeft(ledRow.getWidth() / 2).reduced(1).toFloat(), 2.0f);
        g.setColour(part.muted ? juce::Colour(0xff802020) : studio::obsidian1());
        g.fillRoundedRectangle(ledRow.reduced(1).toFloat(), 2.0f);
    }
}
} // namespace vmpc::view
