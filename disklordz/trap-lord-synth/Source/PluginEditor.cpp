#include "PluginEditor.h"

TrapLordSynthAudioProcessorEditor::TrapLordSynthAudioProcessorEditor (TrapLordSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    constexpr const char* ids[] = { "attack", "decay", "sustain", "release", "cutoff", "gain",
                                    "drive", "detune", "reverb", "delay", "arpRate", "arpGate" };
    constexpr const char* names[] = { "ATTACK", "DECAY", "SUSTAIN", "RELEASE", "CUTOFF", "LEVEL",
                                      "DRIVE", "DETUNE", "REVERB", "DELAY", "ARP DIV", "GATE" };
    for (int i = 0; i < count; ++i)
    {
        knobs[static_cast<size_t> (i)].setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knobs[static_cast<size_t> (i)].setTextBoxStyle (juce::Slider::TextBoxBelow, false, 75, 18);
        addAndMakeVisible (knobs[static_cast<size_t> (i)]);
        labels[static_cast<size_t> (i)].setText (names[i], juce::dontSendNotification);
        labels[static_cast<size_t> (i)].setJustificationType (juce::Justification::centred);
        labels[static_cast<size_t> (i)].setColour (juce::Label::textColourId, juce::Colours::orange);
        addAndMakeVisible (labels[static_cast<size_t> (i)]);
        attachments[static_cast<size_t> (i)] = std::make_unique<SA> (p.state, ids[i], knobs[static_cast<size_t> (i)]);
    }

    constexpr const char* macroIds[] = { "macroDarkness", "macroDirt", "macroMovement", "macroSpace" };
    constexpr const char* macroNames[] = { "DARKNESS", "DIRT", "MOVEMENT", "SPACE" };
    for (int i = 0; i < macroCount; ++i)
    {
        macroKnobs[static_cast<size_t> (i)].setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        macroKnobs[static_cast<size_t> (i)].setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
        macroKnobs[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::gold);
        addAndMakeVisible (macroKnobs[static_cast<size_t> (i)]);
        macroLabels[static_cast<size_t> (i)].setText (macroNames[i], juce::dontSendNotification);
        macroLabels[static_cast<size_t> (i)].setJustificationType (juce::Justification::centred);
        macroLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId, juce::Colours::gold);
        addAndMakeVisible (macroLabels[static_cast<size_t> (i)]);
        macroAttachments[static_cast<size_t> (i)] = std::make_unique<SA> (p.state, macroIds[i], macroKnobs[static_cast<size_t> (i)]);
    }

    engine.addItemList ({ "Brass", "Organ", "Strings", "FM Bell", "Bass", "Choir", "Lead", "Multisample",
                           "Analog Pulse", "Bright EP", "Haunted FM" },
                        1);
    chord.addItemList ({ "Off", "Minor", "Minor 7", "Diminished", "Sus 2", "Minor 9" }, 1);
    bankMode.addItemList ({ "Church Keys", "Dark Brass", "Trap Orchestra" }, 1);
    engineAttachment = std::make_unique<CA> (p.state, "engine", engine);
    chordAttachment = std::make_unique<CA> (p.state, "chord", chord);
    bankAttachment = std::make_unique<CA> (p.state, "bankMode", bankMode);
    addAndMakeVisible (engine);
    addAndMakeVisible (chord);
    addAndMakeVisible (bankMode);

    arpAttachment = std::make_unique<BA> (p.state, "arp", arp);
    addAndMakeVisible (arp);

    bankMode.onChange = [this] { refreshPresetMenu(); };
    refreshPresetMenu();
    presets.onChange = [this]
    {
        const int id = presets.getSelectedId();
        if (id >= 2 && id - 2 < (int) visiblePresetIndices.size())
            processor.loadFactoryPreset (visiblePresetIndices[static_cast<size_t> (id - 2)]);
    };
    addAndMakeVisible (presets);

    load.onClick = [this]
    {
        auto chooser = std::make_shared<juce::FileChooser> ("Open keygroup.json", juce::File {}, "*.json");
        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this, chooser] (const juce::FileChooser& fc)
                              {
                                  status.setText (processor.loadSampleMap (fc.getResult()) ? "Loaded sample map"
                                                                                            : "Invalid sample map",
                                                  juce::dontSendNotification);
                              });
    };
    exportButton.onClick = [this]
    {
        auto chooser = std::make_shared<juce::FileChooser> ("Export keygroup folder");
        chooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectDirectories,
                              [this, chooser] (const juce::FileChooser& fc)
                              {
                                  status.setText (processor.exportKeygroup (fc.getResult()) ? "WAV keygroup exported"
                                                                                              : "Export failed",
                                                  juce::dontSendNotification);
                              });
    };
    addAndMakeVisible (load);
    addAndMakeVisible (exportButton);
    addAndMakeVisible (status);
    status.setColour (juce::Label::textColourId, juce::Colours::lightgrey);
    setSize (1020, 560);
}

void TrapLordSynthAudioProcessorEditor::refreshPresetMenu()
{
    presets.clear();
    visiblePresetIndices.clear();
    presets.addItem ("SELECT ATL CLASSICS PATCH", 1);
    const int bank = juce::jlimit (0, 2, bankMode.getSelectedItemIndex());
    const auto& names = TrapLordSynthAudioProcessor::factoryNames();
    for (int i = 0; i < TrapLordSynthAudioProcessor::factoryPresetCount(); ++i)
    {
        if (TrapLordSynthAudioProcessor::presetBank (i) == bank)
        {
            visiblePresetIndices.push_back (i);
            presets.addItem (names[i], (int) visiblePresetIndices.size() + 1);
        }
    }
}

void TrapLordSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff101015));
    g.setColour (juce::Colours::orange);
    g.setFont (juce::Font (juce::FontOptions (28.f, juce::Font::bold)));
    g.drawText ("DISKLORDZ  /  TRAP LORD SYNTH", 22, 10, 900, 36, juce::Justification::centredLeft);
    g.setColour (juce::Colours::grey);
    g.setFont (13.f);
    g.drawText ("ATL CLASSICS SOUND BANK  |  CHURCH KEYS  ·  DARK BRASS  ·  TRAP ORCHESTRA", 24, 48, 900, 22,
                juce::Justification::centredLeft);
    g.setColour (juce::Colours::gold);
    g.drawText ("GLOBAL MACROS", 24, 395, 200, 18, juce::Justification::centredLeft);
}

void TrapLordSynthAudioProcessorEditor::resized()
{
    for (int i = 0; i < count; ++i)
    {
        const int x = 18 + (i % 6) * 151;
        const int y = 78 + (i / 6) * 145;
        labels[static_cast<size_t> (i)].setBounds (x, y, 140, 22);
        knobs[static_cast<size_t> (i)].setBounds (x, y + 24, 140, 110);
    }
    for (int i = 0; i < macroCount; ++i)
    {
        const int x = 18 + i * 155;
        macroLabels[static_cast<size_t> (i)].setBounds (x, 415, 140, 20);
        macroKnobs[static_cast<size_t> (i)].setBounds (x, 435, 140, 100);
    }
    bankMode.setBounds (20, 348, 165, 28);
    engine.setBounds (195, 348, 165, 28);
    chord.setBounds (370, 348, 140, 28);
    presets.setBounds (520, 348, 220, 28);
    arp.setBounds (750, 348, 85, 28);
    load.setBounds (520, 380, 145, 28);
    exportButton.setBounds (675, 380, 160, 28);
    status.setBounds (20, 522, 970, 22);
}
