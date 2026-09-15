#include "EsxTransportHeader.h"
#include "View/Design/StudioColours.h"

namespace vmpc::view
{
EsxTransportHeader::EsxTransportHeader(controller::AppController& controller)
    : appController(controller)
{
    brand.setColour(juce::Label::textColourId, studio::iceOled());
    brand.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    addAndMakeVisible(brand);

    telemetry.setFont(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::plain));
    telemetry.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.75f));
    addAndMakeVisible(telemetry);

    playBtn.onClick = [this]() { appController.toggleTransport(); };
    stopBtn.onClick = [this]() { appController.setTransportPlaying(false); };
    recBtn.setClickingTogglesState(true);
    recBtn.onClick = [this]() {
        appController.getWorkspace().getElectribeSong().setMotionRecArmed(recBtn.getToggleState());
        refreshLabels();
    };
    addAndMakeVisible(playBtn);
    addAndMakeVisible(stopBtn);
    addAndMakeVisible(recBtn);

    swingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    swingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 36, 18);
    swingSlider.setRange(50, 75, 1);
    swingSlider.setValue(appController.getProject().getTree().getProperty("swing", 62));
    swingSlider.onValueChange = [this]() {
        const int swing = static_cast<int>(swingSlider.getValue());
        appController.getProject().getTree().setProperty("swing", swing, nullptr);
        appController.syncProjectToSequencer();
        refreshLabels();
    };
    addChildComponent(swingSlider);

    appController.addListener(this);
    refreshLabels();
}

EsxTransportHeader::~EsxTransportHeader()
{
    appController.removeListener(this);
}

void EsxTransportHeader::transportStateChanged(bool playing)
{
    playBtn.setButtonText(playing ? "Pause" : "Play");
}

void EsxTransportHeader::refreshLabels()
{
    const auto& tree = appController.getProject().getTree();
    const auto& song = appController.getWorkspace().getElectribeSong();
    telemetry.setText("FUTURE BEATS — REV. 3  |  "
                      + juce::String(static_cast<double>(tree.getProperty("bpm", 92.4)), 2) + " BPM  |  LINN "
                      + juce::String(static_cast<int>(tree.getProperty("swing", 62))) + "%  |  "
                      + song.getPatternName() + "  |  "
                      + (song.isMotionRecArmed() ? "MOTION REC *" : "MOTION REC"),
                      juce::dontSendNotification);
    recBtn.setToggleState(song.isMotionRecArmed(), juce::dontSendNotification);
}

void EsxTransportHeader::paint(juce::Graphics& g)
{
    g.fillAll(studio::obsidian2());
    g.setColour(studio::walnutTrim());
    g.fillRect(0, getHeight() - 2, getWidth(), 2);
}

void EsxTransportHeader::resized()
{
    auto r = getLocalBounds().reduced(8, 4);
    brand.setBounds(r.removeFromLeft(100));
    playBtn.setBounds(r.removeFromRight(52).reduced(2));
    stopBtn.setBounds(r.removeFromRight(48).reduced(2));
    recBtn.setBounds(r.removeFromRight(48).reduced(2));
    telemetry.setBounds(r.reduced(4, 0));
}
} // namespace vmpc::view
