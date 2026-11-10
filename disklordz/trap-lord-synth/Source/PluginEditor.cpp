#include "PluginEditor.h"
TrapLordSynthAudioProcessorEditor::TrapLordSynthAudioProcessorEditor(TrapLordSynthAudioProcessor& p)
    :AudioProcessorEditor(&p),processor(p) {
    constexpr const char* ids[]={"attack","decay","sustain","release","cutoff","gain","drive","detune","reverb","delay","arpRate","arpGate"};
    constexpr const char* names[]={"ATTACK","DECAY","SUSTAIN","RELEASE","CUTOFF","LEVEL","DRIVE","DETUNE","REVERB","DELAY","ARP RATE","GATE"};
    for(int i=0;i<count;++i) {
        knobs[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knobs[i].setTextBoxStyle(juce::Slider::TextBoxBelow,false,75,18);
        addAndMakeVisible(knobs[i]);labels[i].setText(names[i],juce::dontSendNotification);
        labels[i].setJustificationType(juce::Justification::centred);
        labels[i].setColour(juce::Label::textColourId,juce::Colours::orange);
        addAndMakeVisible(labels[i]);attachments[i]=std::make_unique<SA>(p.state,ids[i],knobs[i]);
    }
    engine.addItemList({"Brass","Organ","Strings","FM Bell","Bass","Choir","Lead","Multisample"},1);
    chord.addItemList({"Off","Minor","Minor 7","Diminished","Sus 2","Minor 9"},1);
    engineAttachment=std::make_unique<CA>(p.state,"engine",engine);
    chordAttachment=std::make_unique<CA>(p.state,"chord",chord);
    addAndMakeVisible(engine);addAndMakeVisible(chord);
    arpAttachment=std::make_unique<BA>(p.state,"arp",arp);addAndMakeVisible(arp);
    presets.addItem("SELECT FACTORY PATCH",1);
    for(int i=0;i<p.factoryNames().size();++i)presets.addItem(p.factoryNames()[i],i+2);
    presets.onChange=[this] {if(presets.getSelectedId()>=2)processor.loadFactoryPreset(presets.getSelectedId()-2);};
    addAndMakeVisible(presets);
    load.onClick=[this] {
        auto chooser=std::make_shared<juce::FileChooser>("Open keygroup.json",juce::File{},"*.json");
        chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,
            [this,chooser](const juce::FileChooser& fc) {
                status.setText(processor.loadSampleMap(fc.getResult())?"Loaded sample map":"Invalid sample map",juce::dontSendNotification);
            });
    };
    exportButton.onClick=[this] {
        auto chooser=std::make_shared<juce::FileChooser>("Export keygroup folder");
        chooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectDirectories,
            [this,chooser](const juce::FileChooser& fc) {
                status.setText(processor.exportKeygroup(fc.getResult())?"WAV keygroup exported":"Export failed",juce::dontSendNotification);
            });
    };
    addAndMakeVisible(load);addAndMakeVisible(exportButton);addAndMakeVisible(status);
    status.setColour(juce::Label::textColourId,juce::Colours::lightgrey);
    setSize(930,480);
}
void TrapLordSynthAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff101015));g.setColour(juce::Colours::orange);
    g.setFont(juce::Font(juce::FontOptions(29.f,juce::Font::bold)));
    g.drawText("DISKLORDZ  /  TRAP LORD SYNTH",22,12,850,40,juce::Justification::centredLeft);
    g.setColour(juce::Colours::grey);g.setFont(13.f);
    g.drawText("HYBRID SOUTHERN TRAP WORKSTATION  |  M1-M5 SOURCE EDITION",24,53,850,22,juce::Justification::centredLeft);
}
void TrapLordSynthAudioProcessorEditor::resized() {
    for(int i=0;i<count;++i) {
        const int x=18+(i%6)*151,y=95+(i/6)*145;
        labels[i].setBounds(x,y,140,22);knobs[i].setBounds(x,y+24,140,110);
    }
    engine.setBounds(20,400,155,28);chord.setBounds(185,400,140,28);
    presets.setBounds(335,400,175,28);arp.setBounds(520,400,85,28);
    load.setBounds(610,400,145,28);exportButton.setBounds(760,400,155,28);
    status.setBounds(20,439,890,22);
}
