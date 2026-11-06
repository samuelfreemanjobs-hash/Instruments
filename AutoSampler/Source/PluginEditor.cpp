#include "PluginEditor.h"

namespace
{
constexpr int kW = 640;
constexpr int kH = 420;

juce::String stateToString (autosampler::AutoSampleState st)
{
    switch (st)
    {
        case autosampler::AutoSampleState::idle: return "Idle";
        case autosampler::AutoSampleState::waitingGap: return "Waiting...";
        case autosampler::AutoSampleState::recordingNote: return "Recording";
        case autosampler::AutoSampleState::finished: return "Finished";
        case autosampler::AutoSampleState::aborted: return "Aborted";
    }
    return {};
}
} // namespace

AutoSamplerAudioProcessorEditor::AutoSamplerAudioProcessorEditor (AutoSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setSize (kW, kH);

    titleLabel_.setFont (juce::Font (24.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel_);
    addAndMakeVisible (statusLabel_);

    for (int i = 0; i < p.getNumPrograms(); ++i)
        programBox_.addItem (p.getProgramName (i), i + 1);
    programBox_.setSelectedId (p.getCurrentProgram() + 1, juce::dontSendNotification);
    programBox_.onChange = [this]
    {
        processor_.setCurrentProgram (programBox_.getSelectedId() - 1);
    };
    addAndMakeVisible (programBox_);

    lowNoteSlider_.setRange (0, 127, 1);
    highNoteSlider_.setRange (0, 127, 1);
    recordSecSlider_.setRange (0.25, 30.0, 0.01);
    outputGainSlider_.setRange (-24.0, 12.0, 0.01);

    for (auto* s : { &lowNoteSlider_, &highNoteSlider_, &recordSecSlider_, &outputGainSlider_ })
    {
        s->setSliderStyle (juce::Slider::LinearHorizontal);
        s->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
        addAndMakeVisible (*s);
    }

    lowNoteAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::lowNote, lowNoteSlider_);
    highNoteAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::highNote, highNoteSlider_);
    recordSecAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::recordSeconds, recordSecSlider_);
    outputGainAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::outputGain, outputGainSlider_);
    startAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::startAutoSample, startButton_);
    trimAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::trimSilence, trimButton_);
    normalizeAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::normalizeCapture, normalizeButton_);
    loopAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::sustainLoop, loopButton_);

    addAndMakeVisible (startButton_);
    for (auto* b : { &trimButton_, &normalizeButton_, &loopButton_ })
        addAndMakeVisible (*b);
    addAndMakeVisible (exportPresetButton_);
    addAndMakeVisible (exportBankButton_);
    addAndMakeVisible (chooseFolderButton_);

    exportPresetButton_.onClick = [this]
    {
        if (auto* param = processor_.getApvts().getParameter (autosampler::ParameterIDs::exportPreset))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost (1.0f);
            param->setValueNotifyingHost (0.0f);
            param->endChangeGesture();
        }
    };

    exportBankButton_.onClick = [this]
    {
        const auto result = processor_.exportEntireBank();
        statusLabel_.setText (result.ok ? "Bank exported: " + result.presetJson.getFullPathName()
                                        : "Export failed: " + result.error,
                              juce::dontSendNotification);
    };

    chooseFolderButton_.onClick = [this]
    {
        auto chooser = std::make_shared<juce::FileChooser> ("Export folder", processor_.getExportRootDirectory());
        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                              [this, chooser] (const juce::FileChooser& fc)
                              {
                                  if (fc.getResult().isDirectory())
                                      processor_.setExportRootDirectory (fc.getResult());
                              });
    };

    startTimerHz (10);
}

AutoSamplerAudioProcessorEditor::~AutoSamplerAudioProcessorEditor() = default;

void AutoSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawFittedText ("Route source audio to Input. Enable MIDI Thru to drive the source while sampling.",
                      getLocalBounds().reduced (12).removeFromBottom (28),
                      juce::Justification::centred,
                      2);
}

void AutoSamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    titleLabel_.setBounds (area.removeFromTop (28));
    statusLabel_.setBounds (area.removeFromTop (22));
    programBox_.setBounds (area.removeFromTop (28).removeFromLeft (220));

    auto row = area.removeFromTop (36);
    chooseFolderButton_.setBounds (row.removeFromRight (140));
    exportBankButton_.setBounds (row.removeFromRight (120));
    exportPresetButton_.setBounds (row.removeFromRight (120));
    startButton_.setBounds (row.removeFromLeft (160));

    auto toggles = area.removeFromTop (28);
    trimButton_.setBounds (toggles.removeFromLeft (90));
    normalizeButton_.setBounds (toggles.removeFromLeft (110));
    loopButton_.setBounds (toggles.removeFromLeft (120));

    auto sliders = area.removeFromTop (160);
    lowNoteSlider_.setBounds (sliders.removeFromTop (40));
    highNoteSlider_.setBounds (sliders.removeFromTop (40));
    recordSecSlider_.setBounds (sliders.removeFromTop (40));
    outputGainSlider_.setBounds (sliders.removeFromTop (40));
}

void AutoSamplerAudioProcessorEditor::timerCallback()
{
    const auto& prog = processor_.getProgram();
    juce::String loopInfo;
    if (prog.zoneCount() > 0)
    {
        const auto& z = prog.zones().front();
        loopInfo = z.hasLoop() ? (" | Loop: " + juce::String (z.loopStart) + "-" + juce::String (z.loopEnd))
                               : juce::String { " | Loop: none" };
    }

    statusLabel_.setText ("State: " + stateToString (processor_.autoSampleState())
                              + " | Zones: " + juce::String (static_cast<int> (prog.zoneCount()))
                              + loopInfo
                              + " | Export: " + processor_.getExportRootDirectory().getFullPathName(),
                          juce::dontSendNotification);
}
