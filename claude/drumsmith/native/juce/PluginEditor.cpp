#include "PluginEditor.h"
DrumsmithEditor::DrumsmithEditor(DrumsmithProcessor& p):AudioProcessorEditor(p),processor(p){
    setSize(680,320);
    heading.setText("DRUMSMITH 2.0  |  DISKLORDZ AUDIO",juce::dontSendNotification);
    heading.setFont(juce::Font(juce::FontOptions(22.f,juce::Font::bold)));
    heading.setColour(juce::Label::textColourId,juce::Colours::white);
    addAndMakeVisible(heading);
    kind.addItemList({"808","Kick","Snare","Clap","Hat","Cowbell","Perc","FX"},1);
    addAndMakeVisible(kind);
    kindAttach=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,"kind",kind);
    const std::array<const char*,6> ids{"pitch","decay","drive","tone","noise","duration"};
    const std::array<const char*,6> names{"Pitch","Decay","Drive","Tone","Noise","Length"};
    for(int i=0;i<6;++i){
        auto& s=knobs[i];s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,84,22);
        s.setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(0xffe78c3e));
        addAndMakeVisible(s);labels[i].setText(names[i],juce::dontSendNotification);
        labels[i].setJustificationType(juce::Justification::centred);addAndMakeVisible(labels[i]);
        attaches[i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,ids[i],s);
    }
}
void DrumsmithEditor::paint(juce::Graphics& g){
    g.fillAll(juce::Colour(0xff151821));g.setColour(juce::Colour(0xff353b47));
    g.drawRoundedRectangle(getLocalBounds().reduced(12).toFloat(),12.f,1.f);
    g.setColour(juce::Colour(0xffa1aab8));g.setFont(13.f);
    g.drawText("MIDI notes  |  pure synthesis  |  one-shot voice engine",22,280,630,24,juce::Justification::centred);
}
void DrumsmithEditor::resized(){
    heading.setBounds(22,16,630,34);kind.setBounds(22,60,220,30);
    for(int i=0;i<6;++i){const int x=18+i*110;labels[i].setBounds(x,113,104,24);knobs[i].setBounds(x,140,104,128);}
}
