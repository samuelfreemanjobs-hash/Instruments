#include "ElectribeModePanel.h"
#include "View/Design/StudioColours.h"

namespace vmpc::view
{
ElectribeModePanel::ElectribeModePanel(controller::AppController& controller)
    : appController(controller)
    , transport(controller)
{
    oledBanner.setText("ELECTRIBE ESX HYBRID", juce::dontSendNotification);
    oledBanner.setJustificationType(juce::Justification::centred);
    oledBanner.setFont(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::bold));
    oledBanner.setColour(juce::Label::textColourId, studio::iceOled());
    addAndMakeVisible(oledBanner);
    addAndMakeVisible(transport);
    addAndMakeVisible(valveForce);
    addAndMakeVisible(partsMatrix);

    for (auto* s : { &cutoff, &resonance, &egInt, &drive })
    {
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s->setRange(0.0, 1.0, 0.001);
        s->setValue(0.5);
        addAndMakeVisible(s);
    }

    addAndMakeVisible(motionLanes);
    stepRow.setLayout(StepSequencerGrid::Layout::Row16Electribe);
    addAndMakeVisible(stepRow);

    addAndMakeVisible(touchRibbon);

    partsMatrix.bindSong(&appController.getWorkspace().getElectribeSong());
    partsMatrix.onPartChanged([this](int) { refreshFromModel(); });

    stepRow.onStepToggled([this](int step, bool active) {
        auto& song = appController.getWorkspace().getElectribeSong();
        auto& part = song.getPart(song.getSelectedPart());
        part.steps[static_cast<size_t>(step)].active = active;
    });

    stepRow.onStepAccentToggled([this](int step, bool accent) {
        auto& song = appController.getWorkspace().getElectribeSong();
        song.getPart(song.getSelectedPart()).steps[static_cast<size_t>(step)].accent = accent;
    });

    appController.setTransportPlaying(false);
    refreshFromModel();
}

void ElectribeModePanel::syncStepGridFromPart()
{
    const auto& song = appController.getWorkspace().getElectribeSong();
    const auto& part = song.getPart(song.getSelectedPart());
    vmpc::model::SixteenStepPattern pattern;
    for (int i = 0; i < 16; ++i)
    {
        pattern.getStep(i).active = part.steps[static_cast<size_t>(i)].active;
        pattern.getStep(i).velocity = part.steps[static_cast<size_t>(i)].velocity;
        pattern.getStep(i).accent = part.steps[static_cast<size_t>(i)].accent;
    }
    stepRow.setPattern(pattern);
}

void ElectribeModePanel::refreshFromModel()
{
    syncStepGridFromPart();
    partsMatrix.repaint();
}

void ElectribeModePanel::updateTransportUi(int playingStep)
{
    stepRow.setPlayingStep(playingStep);
}

void ElectribeModePanel::paint(juce::Graphics& g)
{
    g.fillAll(studio::obsidian1());

    auto filterArea = getLocalBounds().withTrimmedTop(getHeight() * 42 / 100).withHeight(getHeight() * 14 / 100);
    filterArea = filterArea.reduced(10, 0);
    g.setColour(studio::obsidian2());
    g.fillRoundedRectangle(filterArea.toFloat(), 4.0f);

    g.setColour(studio::iceOled().withAlpha(0.8f));
    juce::Path curve;
    const auto c = filterArea.reduced(120, 8).toFloat();
    curve.startNewSubPath(c.getX(), c.getBottom() - 8);
    curve.quadraticTo(c.getCentreX(), c.getY() + 4, c.getRight(), c.getY() + 18);
    g.strokePath(curve, juce::PathStrokeType(2.0f));
}

void ElectribeModePanel::resized()
{
    auto bounds = getLocalBounds();
    transport.setBounds(bounds.removeFromTop(36));
    oledBanner.setBounds(bounds.removeFromTop(24));
    valveForce.setBounds(bounds.removeFromTop(100).reduced(8, 4));
    partsMatrix.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.32f).reduced(8, 4));

    auto filterRow = bounds.removeFromTop(72).reduced(8, 4);
    const int kw = filterRow.getWidth() / 5;
    cutoff.setBounds(filterRow.removeFromLeft(kw));
    resonance.setBounds(filterRow.removeFromLeft(kw));
    egInt.setBounds(filterRow.removeFromLeft(kw));
    drive.setBounds(filterRow.removeFromLeft(kw));

    auto deck = bounds.reduced(8);
    touchRibbon.setBounds(deck.removeFromLeft(48));
    motionLanes.setBounds(deck.removeFromTop(deck.getHeight() * 0.45f));
    stepRow.setBounds(deck);
}

} // namespace vmpc::view
