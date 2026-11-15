#include "PluginEditor.h"
#include "Parameters/ParameterIds.h"
#include "Presets/FactoryPresets.h"
#include "UI/DesignTokens.h"
#include "UI/UiChrome.h"

namespace
{
using PID = wave9090::ParameterIDs;

juce::String tooltip (const juce::String& title, const juce::String& body)
{
    return title + "\n" + body;
}

float mapDb (float norm, float lo, float hi)
{
    return lo + norm * (hi - lo);
}

float readNorm (Wave9090AudioProcessor& proc, const char* id)
{
    if (auto* p = proc.getApvts().getParameter (id))
        return p->getValue();
    return 0.0f;
}
} // namespace

const std::vector<Wave9090AudioProcessorEditor::ParamUiSpec>& Wave9090AudioProcessorEditor::allUiSpecs()
{
    static const std::vector<ParamUiSpec> kSpecs = {
        { PID::wtPosition, "Position", "OSC 1 scope bay", "osc1.position", "cyan", "Wavetable morph index." },
        { PID::wtScan, "Scan", "OSC 1", "osc1.scan", "cyan", "Position LFO depth." },
        { PID::subLevel, "Sub", "OSC 1", "osc1.sub", "cyan", "Sub osc level." },
        { PID::detune, "Detune", "OSC 1", "osc1.detune", "cyan", "Fine tune cents." },
        { PID::glide, "Glide", "Performance row", "perf.glide", "cyan", "Portamento ms." },
        { PID::filterCutoff, "Cutoff", "VCF 1", "vcf1.cutoff", "amber", "Main lowpass cutoff." },
        { PID::filterRes, "Res", "VCF 1", "vcf1.res", "amber", "Resonance." },
        { PID::drive, "Drive", "VCF 1", "vcf1.drive", "amber", "Preamp drive." },
        { PID::crush, "Crush", "VCF 1", "vcf1.crush", "amber", "Destructive crush depth." },
        { PID::ampAttack, "Attack", "ENV 1 Amp", "env1.attack", "cyan", "Amp attack." },
        { PID::ampDecay, "Decay", "ENV 1 Amp", "env1.decay", "cyan", "Amp decay." },
        { PID::ampSustain, "Sustain", "ENV 1 Amp", "env1.sustain", "cyan", "Amp sustain." },
        { PID::ampRelease, "Release", "ENV 1 Amp", "env1.release", "cyan", "Amp release." },
        { PID::filtAttack, "Attack", "ENV 2 Filter", "env2.attack", "amber", "Filter attack." },
        { PID::filtDecay, "Decay", "ENV 2 Filter", "env2.decay", "amber", "Filter decay." },
        { PID::filtSustain, "Sustain", "ENV 2 Filter", "env2.sustain", "amber", "Filter sustain." },
        { PID::filtRelease, "Release", "ENV 2 Filter", "env2.release", "amber", "Filter release." },
        { PID::filtEnvAmt, "F Env", "ENV 2 Filter", "env2.amount", "amber", "Filter env amount." },
        { PID::tapeWobble, "Tape", "Hardware macro", "macro.m1Tape", "amber", "Tape wobble macro." },
        { PID::stereoWidth, "Width", "Output wing", "out.width", "blue", "Stereo spread." },
        { PID::outputGain, "Output", "Header meter", "master.output", "blue", "Master gain dB." },
    };
    return kSpecs;
}

Wave9090AudioProcessorEditor::Wave9090AudioProcessorEditor (Wave9090AudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setLookAndFeel (&laf_);
    tooltipWindow_ = std::make_unique<juce::TooltipWindow> (this, 450);
    setResizable (true, true);
    setResizeLimits (1040, 580, 1600, 920);

    brandTitle_.setText ("WAVE-9090", juce::dontSendNotification);
    brandTitle_.setFont (juce::Font (juce::FontOptions (20.0f, juce::Font::bold)));
    brandTitle_.setColour (juce::Label::textColourId, wave9090::ui::tokens::cyanGlow());
    addAndMakeVisible (brandTitle_);

    brandSub_.setText ("CROSS-WAVE  |  PHYSICAL HYBRID", juce::dontSendNotification);
    brandSub_.setFont (juce::Font (juce::FontOptions (9.5f)));
    brandSub_.setColour (juce::Label::textColourId, wave9090::ui::tokens::textDim());
    addAndMakeVisible (brandSub_);

    presetLcd_.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    presetLcd_.setColour (juce::Label::textColourId, wave9090::ui::tokens::cyanGlow());
    presetLcd_.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (presetLcd_);

    refreshPresetCombo();
    presetBox_.onChange = [this] {
        processor_.applyFactoryPreset (presetBox_.getSelectedId() - 1);
        updatePresetLcd();
    };
    addAndMakeVisible (presetBox_);
    addAndMakeVisible (presetPrev_);
    addAndMakeVisible (presetNext_);
    presetPrev_.onClick = [this] {
        processor_.applyFactoryPreset (juce::jmax (0, processor_.getCurrentProgram() - 1));
        refreshPresetCombo();
        updatePresetLcd();
    };
    presetNext_.onClick = [this] {
        processor_.applyFactoryPreset ((processor_.getCurrentProgram() + 1) % processor_.getNumPrograms());
        refreshPresetCombo();
        updatePresetLcd();
    };
    updatePresetLcd();

    tabSynthMain_.setText ("SYNTH MAIN", juce::dontSendNotification);
    tabSynthMain_.setColour (juce::Label::textColourId, wave9090::ui::tokens::cyanGlow());
    tabSynthMain_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (tabSynthMain_);
    for (auto* tab : { &tabFuture1_, &tabFuture2_ })
    {
        tab->setColour (juce::Label::textColourId, wave9090::ui::tokens::textDim());
        tab->setJustificationType (juce::Justification::centred);
        addAndMakeVisible (*tab);
    }
    tabFuture1_.setText ("MOD / FX (v2)", juce::dontSendNotification);
    tabFuture2_.setText ("SEQ (v2)", juce::dontSendNotification);

    addAndMakeVisible (wtScope_);
    addAndMakeVisible (ledMeter_);
    buildControls();

    circuitButton_.setClickingTogglesState (true);
    addAndMakeVisible (circuitButton_);
    circuitAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), PID::filterCircuit, circuitButton_);

    filterTypeBox_.addItem ("LP 12", 1);
    filterTypeBox_.addItem ("LP 24", 2);
    filterTypeBox_.addItem ("BP 12", 3);
    filterTypeBox_.addItem ("BP 24", 4);
    filterTypeBox_.addItem ("HP 12", 5);
    filterTypeBox_.addItem ("HP 24", 6);
    filterTypeBox_.setTooltip (tooltip ("Filter type", "Zero-delay-feedback multimode ladder."));
    addAndMakeVisible (filterTypeBox_);
    filterTypeAttachment_ = std::make_unique<ComboBoxAttachment> (processor_.getApvts(), PID::filterType, filterTypeBox_);

    keyboard_.setColour (juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour (0xfff1efe7));
    keyboard_.setColour (juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour (0xff1a1e26));
    keyboard_.setColour (juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colours::transparentBlack);
    keyboard_.setScrollButtonsVisible (false);
    // FL Studio: MIDI note 60 = C5 → JUCE octaveForMiddleC 5 (see skin map).
    keyboard_.setOctaveForMiddleC (wave9090::ui::Layout::midiKeyboardMiddleCOctave);
    addAndMakeVisible (keyboard_);
    keyboard_.toBack();

    const std::array<const char*, 5> octLbl { { "-2", "-1", "0", "+1", "+2" } };
    for (std::size_t i = 0; i < octaveButtons_.size(); ++i)
    {
        octaveButtons_[i].setButtonText (octLbl[i]);
        octaveButtons_[i].onClick = [this, shift = static_cast<int> (i) - 2] { applyOctaveShift (shift); };
        addAndMakeVisible (octaveButtons_[i]);
    }
    setSize (wave9090::ui::Layout::editorWidth, wave9090::ui::Layout::editorHeight);
    resized();

    startTimerHz (30);
}

void Wave9090AudioProcessorEditor::syncFactoryProgramUi()
{
    refreshPresetCombo();
    updatePresetLcd();
}

void Wave9090AudioProcessorEditor::resetOutputMeterForIdleShot()
{
    processor_.resetOutputPeak();
    ledMeter_.setLevelDb (-100.0f);
}

void Wave9090AudioProcessorEditor::refreshPresetCombo()
{
    presetBox_.clear();
    const auto& presets = wave9090::presets::getFactoryPresets();
    juce::String lastCategory;
    for (std::size_t i = 0; i < presets.size(); ++i)
    {
        const auto& preset = presets[i];
        const juce::String category = juce::String::fromUTF8 (wave9090::presets::categoryUiHeading (preset.category).data());
        if (category != lastCategory)
        {
            presetBox_.addSectionHeading (category);
            lastCategory = category;
        }
        presetBox_.addItem (juce::String::fromUTF8 (preset.name.data()), static_cast<int> (i) + 1);
    }
    presetBox_.setSelectedId (processor_.getCurrentProgram() + 1, juce::dontSendNotification);
}

void Wave9090AudioProcessorEditor::updatePresetLcd()
{
    const auto& presets = wave9090::presets::getFactoryPresets();
    const int index = processor_.getCurrentProgram();
    if (index >= 0 && index < static_cast<int> (presets.size()))
    {
        const auto& preset = presets[static_cast<std::size_t> (index)];
        const juce::String line = juce::String::fromUTF8 (wave9090::presets::categoryUiHeading (preset.category).data()) + "  ·  "
                                  + juce::String::fromUTF8 (preset.name.data());
        presetLcd_.setText (line, juce::dontSendNotification);
    }
}

void Wave9090AudioProcessorEditor::applyOctaveShift (int shift)
{
    octaveShift_ = juce::jlimit (-2, 2, shift);
    layoutControls();
}

void Wave9090AudioProcessorEditor::buildControls()
{
    for (const auto& spec : allUiSpecs())
    {
        auto knob = std::make_unique<wave9090::ui::LabeledKnob> (spec.label);
        knob->setKnobDomain (spec.knobDomain);
        const auto tip = tooltip (spec.label, spec.tooltip + " APVTS: " + spec.paramId
                                               + " — mockup: " + spec.mockupSection);
        knob->getSlider().setTooltip (tip);
        knob->getLegend().setTooltip (tip);
        knob->formatValue = [id = juce::String (spec.paramId)] (float n)
        {
            return formatParamValue (id.toRawUTF8(), n);
        };
        knob->getSlider().onValueChange = [k = knob.get()]
        {
            if (k->formatValue)
                k->setValueText (k->formatValue (static_cast<float> (k->getSlider().getValue())));
        };
        addAndMakeVisible (*knob);
        sliderAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), spec.paramId, knob->getSlider()));
        if (knob->formatValue)
        {
            const float v = processor_.getApvts().getRawParameterValue (spec.paramId) != nullptr
                                ? processor_.getApvts().getRawParameterValue (spec.paramId)->load()
                                : static_cast<float> (knob->getSlider().getValue());
            knob->setValueText (knob->formatValue (v));
        }
        knobs_.push_back (std::move (knob));
    }
}

juce::String Wave9090AudioProcessorEditor::formatParamValue (const char* id, float value)
{
    const juce::String sid (id);

    // APVTS sliders use real parameter units (not 0…1 normalised).
    if (sid == PID::outputGain)
        return juce::String (value, 1) + " dB";
    if (sid == PID::detune)
        return juce::String (value, 1) + " ct";
    if (sid == PID::glide)
        return juce::String (value, 0) + " ms";

    if (sid == PID::ampSustain || sid == PID::filtSustain)
        return juce::String (juce::roundToInt (value * 100.0f)) + "%";

    if (sid.contains ("Attack") || sid.contains ("Decay") || sid.contains ("Release"))
        return juce::String (value, 2) + " s";

    return juce::String (juce::roundToInt (value * 100.0f)) + "%";
}

wave9090::ui::LabeledKnob* Wave9090AudioProcessorEditor::knobForId (const char* id) noexcept
{
    const auto& specs = allUiSpecs();
    for (std::size_t i = 0; i < specs.size() && i < knobs_.size(); ++i)
        if (juce::String (specs[i].paramId) == id)
            return knobs_[i].get();
    return nullptr;
}

void Wave9090AudioProcessorEditor::paintEnvCurve (juce::Graphics& g, juce::Rectangle<int> area, juce::Colour c,
                                                   float a, float d, float s, float r) const
{
    juce::ignoreUnused (r);
    juce::Path p;
    const float w = static_cast<float> (area.getWidth());
    const float h = static_cast<float> (area.getHeight());
    const float ax = w * juce::jlimit (0.03f, 0.35f, a);
    const float dx = w * juce::jlimit (0.03f, 0.35f, d);
    p.startNewSubPath (0.0f, static_cast<float> (area.getBottom()));
    p.lineTo (ax, static_cast<float> (area.getY()) + 2.0f);
    p.lineTo (ax + dx, static_cast<float> (area.getY()) + h * (1.0f - s) + 2.0f);
    p.lineTo (w, static_cast<float> (area.getBottom()));
    g.setColour (c.withAlpha (0.2f));
    g.fillPath (p, juce::AffineTransform::translation (static_cast<float> (area.getX()), 0.0f));
    g.setColour (c);
    g.strokePath (p, juce::PathStrokeType (1.4f), juce::AffineTransform::translation (static_cast<float> (area.getX()), 0.0f));
}

void Wave9090AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (wave9090::ui::tokens::bgChassis());
    wave9090::ui::paintWalnutCheeks (g, getLocalBounds(), wave9090::ui::Layout::woodCheekW);

    auto face = wave9090::ui::Regions::faceplate (getLocalBounds());
    g.setColour (wave9090::ui::tokens::surfacePanel());
    g.fillRect (face);

    wave9090::ui::paintModuleBay (g, bayOsc1_, "OSC 1: CYAN-MORPH", wave9090::ui::tokens::cyanGlow());
    wave9090::ui::paintModuleBay (g, bayVcf1_, "VCF 1: LADDER", wave9090::ui::tokens::amberGlow());
    wave9090::ui::paintModuleBay (g, bayEnvAmp_, "ENV 1: AMP (VCA)", wave9090::ui::tokens::cyanGlow());
    wave9090::ui::paintModuleBay (g, bayEnvFilt_, "ENV 2: FILTER", wave9090::ui::tokens::amberGlow());
    wave9090::ui::paintModuleBay (g, bayMacro_, "TAPE / OUT", wave9090::ui::tokens::amberGlow());

    auto ampCurve = bayEnvAmp_.reduced (8, 30).removeFromTop (bayEnvAmp_.getHeight() / 3);
    paintEnvCurve (g, ampCurve, wave9090::ui::tokens::cyanGlow(),
                   readNorm (processor_, PID::ampAttack), readNorm (processor_, PID::ampDecay),
                   readNorm (processor_, PID::ampSustain), readNorm (processor_, PID::ampRelease));
    auto filtCurve = bayEnvFilt_.reduced (8, 30).removeFromTop (bayEnvFilt_.getHeight() / 3);
    paintEnvCurve (g, filtCurve, wave9090::ui::tokens::amberGlow(),
                   readNorm (processor_, PID::filtAttack), readNorm (processor_, PID::filtDecay),
                   readNorm (processor_, PID::filtSustain), readNorm (processor_, PID::filtRelease));

    auto keybed = wave9090::ui::Regions::keyboardKeybed (getLocalBounds());
    g.setColour (wave9090::ui::tokens::feltCrimson());
    g.fillRect (keybed.removeFromTop (wave9090::ui::Layout::feltStripHeight));
}

void Wave9090AudioProcessorEditor::layoutControls()
{
    auto header = wave9090::ui::Regions::header (getLocalBounds());
    brandTitle_.setBounds (header.getX() + 12, header.getY() + 8, 260, 26);
    brandSub_.setBounds (header.getX() + 14, header.getY() + 32, 340, 14);

    const int presetW = 200;
    presetNext_.setBounds (header.getRight() - 26, header.getY() + 10, 22, 22);
    presetBox_.setBounds (header.getRight() - 26 - presetW, header.getY() + 8, presetW, 26);
    presetPrev_.setBounds (presetBox_.getX() - 24, header.getY() + 10, 22, 22);
    presetLcd_.setBounds (presetPrev_.getX() - 280, header.getY() + 6, 270, 28);

    ledMeter_.setBounds (presetLcd_.getX() - 158, header.getY() + 12, 150, 22);

    auto tabs = wave9090::ui::Regions::tabs (header);
    const int tabW = tabs.getWidth() / 3;
    tabSynthMain_.setBounds (tabs.removeFromLeft (tabW));
    tabFuture1_.setBounds (tabs.removeFromLeft (tabW));
    tabFuture2_.setBounds (tabs);

    const auto body = wave9090::ui::Regions::body (getLocalBounds());
    const int gap = wave9090::ui::Layout::bayGap;
    // 2:1:1:1:1 split → six units across the body width (OSC spans two).
    const int unitW = (body.getWidth() - 4 * gap) / 6;
    int x = body.getX();
    bayOsc1_ = { x, body.getY(), unitW * 2, body.getHeight() };
    x += unitW * 2 + gap;
    bayVcf1_ = { x, body.getY(), unitW, body.getHeight() };
    x += unitW + gap;
    bayEnvAmp_ = { x, body.getY(), unitW, body.getHeight() };
    x += unitW + gap;
    bayEnvFilt_ = { x, body.getY(), unitW, body.getHeight() };
    x += unitW + gap;
    bayMacro_ = { x, body.getY(), unitW, body.getHeight() };

    auto osc = bayOsc1_.reduced (10, 26);
    wtScope_.setBounds (osc.removeFromTop (juce::jmax (90, osc.getHeight() / 2)));
    osc.removeFromTop (6);

    auto placeGrid = [&] (juce::Rectangle<int> area, std::initializer_list<const char*> ids, int cols)
    {
        const int count = static_cast<int> (ids.size());
        const int rows = (count + cols - 1) / cols;
        const int cw = area.getWidth() / cols;
        const int ch = area.getHeight() / juce::jmax (1, rows);
        int idx = 0;
        for (const char* id : ids)
        {
            if (auto* k = knobForId (id))
            {
                const int col = idx % cols;
                const int row = idx / cols;
                auto cell = juce::Rectangle<int> (area.getX() + col * cw, area.getY() + row * ch, cw, ch);
                const int side = juce::jmin (cell.getWidth(), cell.getHeight());
                k->setBounds (cell.withSizeKeepingCentre (side, side));
            }
            ++idx;
        }
    };

    placeGrid (osc, { PID::wtPosition, PID::wtScan, PID::subLevel, PID::detune }, 2);

    auto vcf = bayVcf1_.reduced (10, 28);
    vcf.removeFromTop (bayVcf1_.getHeight() / 4);
    placeGrid (vcf, { PID::filterCutoff, PID::filterRes, PID::drive, PID::crush }, 2);
    auto filterFooter = vcf.removeFromBottom (56);
    filterTypeBox_.setBounds (filterFooter.removeFromTop (26).reduced (4, 2));
    circuitButton_.setBounds (filterFooter.withSizeKeepingCentre (filterFooter.getWidth() - 8, 26));

    auto envAmp = bayEnvAmp_.reduced (8, 28);
    envAmp.removeFromTop (bayEnvAmp_.getHeight() / 3 + 6);
    placeGrid (envAmp, { PID::ampAttack, PID::ampDecay, PID::ampSustain, PID::ampRelease }, 2);

    auto envFilt = bayEnvFilt_.reduced (8, 28);
    envFilt.removeFromTop (bayEnvFilt_.getHeight() / 3 + 6);
    placeGrid (envFilt, { PID::filtAttack, PID::filtDecay, PID::filtSustain, PID::filtRelease, PID::filtEnvAmt }, 2);

    auto macro = bayMacro_.reduced (10, 28);
    placeGrid (macro, { PID::tapeWobble, PID::stereoWidth, PID::outputGain }, 1);

    const auto octCol = wave9090::ui::Regions::octaveColumn (getLocalBounds());
    const int obw = juce::jmax (28, octCol.getWidth() / static_cast<int> (octaveButtons_.size()) - 3);
    for (std::size_t i = 0; i < octaveButtons_.size(); ++i)
    {
        octaveButtons_[i].setBounds (octCol.getX() + static_cast<int> (i) * (obw + 3),
                                      octCol.getY(),
                                      obw,
                                      octCol.getHeight());
        const bool active = static_cast<int> (i) - 2 == octaveShift_;
        octaveButtons_[i].setColour (juce::TextButton::textColourOffId,
                                     active ? wave9090::ui::tokens::cyanGlow()
                                            : wave9090::ui::tokens::textSecondary());
    }

    auto foot = wave9090::ui::Regions::footer (getLocalBounds());
    auto glideSlot = foot;
    glideSlot.removeFromLeft (wave9090::ui::Layout::octaveColumnWidth);
    glideSlot = glideSlot.removeFromLeft (wave9090::ui::Layout::glideSlotWidth).reduced (4, 8);
    if (auto* glide = knobForId (PID::glide))
        glide->setBounds (glideSlot);

    auto keybed = wave9090::ui::Regions::keyboardKeybed (getLocalBounds());
    keyboard_.setBounds (keybed.withTrimmedTop (wave9090::ui::Layout::feltStripHeight));
    const int kbLow = wave9090::ui::Layout::octaveZeroLowestMidi + octaveShift_ * 12;
    const int kbHigh = kbLow + (wave9090::ui::Layout::visibleWhiteKeys * 12) / 7 + 12;
    keyboard_.setOctaveForMiddleC (wave9090::ui::Layout::midiKeyboardMiddleCOctave);
    keyboard_.setAvailableRange (kbLow, juce::jmin (127, kbHigh));
    keyboard_.setLowestVisibleKey (kbLow);
    layoutKeyboardKeyWidth();
}

void Wave9090AudioProcessorEditor::layoutKeyboardKeyWidth()
{
    auto keyArea = keyboard_.getBounds();
    if (keyArea.getWidth() < 40)
        return;

    const float whiteKeys = static_cast<float> (wave9090::ui::Layout::visibleWhiteKeys);
    keyboard_.setKeyWidth (static_cast<float> (keyArea.getWidth()) / whiteKeys);
}

void Wave9090AudioProcessorEditor::resized()
{
    layoutControls();
}

void Wave9090AudioProcessorEditor::timerCallback()
{
    ledMeter_.setLevelDb (processor_.getOutputPeakDb());
    if (auto* raw = processor_.getApvts().getRawParameterValue (PID::wtPosition))
        wtScope_.setWavetablePosition (raw->load());
}

void Wave9090AudioProcessorEditor::renderEditorPng (const juce::File& outFile) const
{
    auto* self = const_cast<Wave9090AudioProcessorEditor*> (this);
    self->resized();
    const auto scale = 1.0f;
    auto img = self->createComponentSnapshot (getLocalBounds(), true, scale);
    outFile.getParentDirectory().createDirectory();
    outFile.deleteFile();
    juce::FileOutputStream stream (outFile);
    if (stream.openedOk())
        juce::PNGImageFormat().writeImageToStream (img, stream);
}

Wave9090AudioProcessorEditor::~Wave9090AudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}