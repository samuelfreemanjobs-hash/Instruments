#include "SynthEditor.h"

namespace
{
const juce::Colour kPanelBlue { 0xff152238 };
const juce::Colour kPanelInset { 0xff0d1628 };
const juce::Colour kPanelWood { 0xff5c3d22 };
const juce::Colour kAccent { 0xfff0d060 };
const juce::Colour kSectionText { 0xffe8eef8 };

void drawSection(juce::Graphics& g, juce::Rectangle<int> r, const juce::String& title)
{
    g.setColour(kPanelInset);
    g.fillRoundedRectangle(r.toFloat(), 8.0f);
    g.setColour(kPanelWood);
    g.drawRoundedRectangle(r.toFloat(), 8.0f, 2.0f);
    g.setColour(kSectionText);
    g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    g.drawText(title, r.removeFromTop(22).reduced(8, 0), juce::Justification::centredLeft);
}

void layoutKnobRow(juce::Rectangle<int> area, std::initializer_list<juce::Slider*> knobs)
{
    const int n = static_cast<int>(knobs.size());
    const int w = area.getWidth() / n;
    int i = 0;
    for (auto* k : knobs)
    {
        k->setBounds(area.removeFromLeft(w).reduced(6));
        ++i;
        juce::ignoreUnused(i);
    }
}
} // namespace

void MoogVoyagerAudioProcessorEditor::styleKnob(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, kAccent);
    s.setColour(juce::Slider::thumbColourId, kAccent.brighter(0.2f));
}

void MoogVoyagerAudioProcessorEditor::bindCombo(juce::ComboBox& box, const juce::StringArray& items)
{
    box.addItemList(items, 1);
}

void MoogVoyagerAudioProcessorEditor::sectionLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, kAccent);
    label.setFont(juce::FontOptions(13.0f, juce::Font::bold));
}

MoogVoyagerAudioProcessorEditor::MoogVoyagerAudioProcessorEditor(MoogVoyagerAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p)
{
    setOpaque(true);

    titleLabel.setText("V VOYAGER", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, kAccent);
    titleLabel.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    for (auto* lab : { &oscSectionLabel, &filterSectionLabel, &envSectionLabel, &modSectionLabel })
    {
        addAndMakeVisible(*lab);
    }
    sectionLabel(oscSectionLabel, "OSCILLATORS");
    sectionLabel(filterSectionLabel, "LADDER FILTER");
    sectionLabel(envSectionLabel, "ENVELOPES");
    sectionLabel(modSectionLabel, "LFO / GLIDE");

    const juce::StringArray waves { "Saw", "Square", "Triangle", "Pulse" };
    const juce::StringArray octaves { "-2", "-1", "0", "+1", "+2" };

    for (int i = 0; i < 3; ++i)
    {
        oscIndexLabel[i].setText("OSC " + juce::String(i + 1), juce::dontSendNotification);
        oscIndexLabel[i].setJustificationType(juce::Justification::centred);
        oscIndexLabel[i].setColour(juce::Label::textColourId, kSectionText);
        oscOctLabel[i].setText("Octave", juce::dontSendNotification);
        oscWaveLabel[i].setText("Wave", juce::dontSendNotification);
        for (auto* l : { &oscIndexLabel[i], &oscOctLabel[i], &oscWaveLabel[i] })
        {
            l->setJustificationType(juce::Justification::centred);
            l->setColour(juce::Label::textColourId, kSectionText.withAlpha(0.9f));
            addAndMakeVisible(*l);
        }

        styleKnob(oscLevel[i], "Level");
        styleKnob(oscFine[i], "Fine");
        bindCombo(oscOctave[i], octaves);
        bindCombo(oscWave[i], waves);
        addAndMakeVisible(oscLevel[i]);
        addAndMakeVisible(oscFine[i]);
        addAndMakeVisible(oscOctave[i]);
        addAndMakeVisible(oscWave[i]);
    }

    styleKnob(noiseSlider, "Noise");
    addAndMakeVisible(noiseSlider);

    styleKnob(filterCutoff, "Cutoff");
    styleKnob(filterRes, "Resonance");
    styleKnob(filterDrive, "Drive");
    styleKnob(filterEnvAmt, "F.Env");
    styleKnob(filterKb, "KB Track");
    styleKnob(fAttack, "F.A");
    styleKnob(fDecay, "F.D");
    styleKnob(fSustain, "F.S");
    styleKnob(fRelease, "F.R");
    styleKnob(aAttack, "A.A");
    styleKnob(aDecay, "A.D");
    styleKnob(aSustain, "A.S");
    styleKnob(aRelease, "A.R");
    styleKnob(lfoRate, "LFO Hz");
    styleKnob(lfoPitch, "→ Pitch");
    styleKnob(lfoFilter, "→ Filter");
    styleKnob(glide, "Glide");
    styleKnob(masterSlider, "Master");

    for (auto* s : { &filterCutoff, &filterRes, &filterDrive, &filterEnvAmt, &filterKb,
                     &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease,
                     &lfoRate, &lfoPitch, &lfoFilter, &glide, &masterSlider })
        addAndMakeVisible(*s);

    auto& apvts = processorRef.getApvts();
    masterAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::outputGainDb), masterSlider);

    const std::string_view levelIds[3] = { SynthParamIDs::osc1Level, SynthParamIDs::osc2Level, SynthParamIDs::osc3Level };
    const std::string_view fineIds[3] = { SynthParamIDs::osc1Fine, SynthParamIDs::osc2Fine, SynthParamIDs::osc3Fine };
    const std::string_view octIds[3] = { SynthParamIDs::osc1Octave, SynthParamIDs::osc2Octave, SynthParamIDs::osc3Octave };
    const std::string_view waveIds[3] = { SynthParamIDs::osc1Wave, SynthParamIDs::osc2Wave, SynthParamIDs::osc3Wave };

    for (int i = 0; i < 3; ++i)
    {
        oscLevelAttachments[static_cast<size_t>(i)] =
            std::make_unique<SliderAttachment>(apvts, std::string(levelIds[i]), oscLevel[i]);
        oscFineAttachments[static_cast<size_t>(i)] =
            std::make_unique<SliderAttachment>(apvts, std::string(fineIds[i]), oscFine[i]);
        oscOctaveAttachments[static_cast<size_t>(i)] =
            std::make_unique<ComboAttachment>(apvts, std::string(octIds[i]), oscOctave[i]);
        oscWaveAttachments[static_cast<size_t>(i)] =
            std::make_unique<ComboAttachment>(apvts, std::string(waveIds[i]), oscWave[i]);
    }

    noiseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::noiseLevel), noiseSlider);
    filterCutoffAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterCutoff), filterCutoff);
    filterResAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterResonance), filterRes);
    filterDriveAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterDrive), filterDrive);
    filterEnvAmtAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterEnvAmount), filterEnvAmt);
    filterKbAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterKeyTrack), filterKb);

    fAttackAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterAttack), fAttack);
    fDecayAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterDecay), fDecay);
    fSustainAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterSustain), fSustain);
    fReleaseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterRelease), fRelease);

    aAttackAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampAttack), aAttack);
    aDecayAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampDecay), aDecay);
    aSustainAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampSustain), aSustain);
    aReleaseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampRelease), aRelease);

    lfoRateAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::lfoRate), lfoRate);
    lfoPitchAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::lfoToPitch), lfoPitch);
    lfoFilterAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::lfoToFilter), lfoFilter);
    glideAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::glideTime), glide);

    setResizeLimits(900, 480, 1400, 800);
    setSize(1040, 580);
}

MoogVoyagerAudioProcessorEditor::~MoogVoyagerAudioProcessorEditor() = default;

void MoogVoyagerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(kPanelBlue);
    g.setColour(kPanelWood);
    g.fillRect(0, 0, getWidth(), 52);
    g.setColour(kAccent.withAlpha(0.35f));
    g.fillRect(0, 52, getWidth(), 2);

    auto bounds = getLocalBounds().reduced(12).withTrimmedTop(58);
    auto oscBox = bounds.removeFromTop(175);
    auto filterBox = bounds.removeFromTop(115);
    auto envBox = bounds.removeFromTop(115);
    auto modBox = bounds.removeFromTop(95);
    juce::ignoreUnused(modBox);

    drawSection(g, oscBox, "Oscillator bank");
    drawSection(g, filterBox, "Moog ladder filter");
    drawSection(g, envBox, "Filter & amplifier envelopes");
    drawSection(g, bounds, "Modulation");
}

void MoogVoyagerAudioProcessorEditor::resized()
{
    titleLabel.setBounds(0, 8, getWidth(), 36);

    auto bounds = getLocalBounds().reduced(12).withTrimmedTop(58);
    auto oscBox = bounds.removeFromTop(175);
    oscSectionLabel.setBounds(oscBox.removeFromTop(20));
    auto oscInner = oscBox.reduced(10, 4);

    for (int i = 0; i < 3; ++i)
    {
        auto col = oscInner.removeFromLeft(oscInner.getWidth() / 3).reduced(4);
        oscIndexLabel[i].setBounds(col.removeFromTop(18));
        auto knobs = col.removeFromTop(col.getHeight() - 52);
        oscLevel[i].setBounds(knobs.removeFromLeft(knobs.getWidth() / 2).reduced(2));
        oscFine[i].setBounds(knobs.reduced(2));
        oscOctLabel[i].setBounds(col.removeFromTop(14));
        oscOctave[i].setBounds(col.removeFromTop(22));
        oscWaveLabel[i].setBounds(col.removeFromTop(14));
        oscWave[i].setBounds(col.removeFromTop(22));
    }

    auto filterBox = bounds.removeFromTop(115);
    filterSectionLabel.setBounds(filterBox.removeFromTop(20));
    auto fIn = filterBox.reduced(10, 4);
    layoutKnobRow(fIn.removeFromTop(78), { &filterCutoff, &filterRes, &filterDrive, &filterEnvAmt, &filterKb });

    auto envBox = bounds.removeFromTop(115);
    envSectionLabel.setBounds(envBox.removeFromTop(20));
    auto eIn = envBox.reduced(10, 4);
    layoutKnobRow(eIn.removeFromTop(38), { &fAttack, &fDecay, &fSustain, &fRelease });
    layoutKnobRow(eIn, { &aAttack, &aDecay, &aSustain, &aRelease });

    auto modBox = bounds.removeFromTop(95);
    modSectionLabel.setBounds(modBox.removeFromTop(20));
    auto mIn = modBox.reduced(10, 4);
    layoutKnobRow(mIn, { &lfoRate, &lfoPitch, &lfoFilter, &glide, &noiseSlider, &masterSlider });
}
