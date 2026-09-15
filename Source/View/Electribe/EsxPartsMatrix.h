#pragma once

#include <JuceHeader.h>
#include "Model/Electribe/ElectribeSong.h"

namespace vmpc::view
{
class EsxPartsMatrix : public juce::Component
{
public:
    using PartChanged = std::function<void(int partIndex)>;

    void bindSong(vmpc::model::ElectribeSong* songPtr) { song = songPtr; repaint(); }
    void onPartChanged(PartChanged cb) { partChanged = std::move(cb); }

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    juce::Rectangle<int> cellBounds(int index) const;
    int hitTestPart(juce::Point<int> pos) const;

    vmpc::model::ElectribeSong* song = nullptr;
    PartChanged partChanged;
};
} // namespace vmpc::view
