#include "PluginEditor.h"
#include "Parameters/ParameterIds.h"
#include "UI/SkinAssets.h"

namespace
{
using PID = wave9090::ParameterIDs;

juce::String tooltip (const juce::String& title, const juce::String& body)
{
    return title + juce::String (juce::NewLine()) + body;
}

float mapDb (float norm, float lo, float hi)
{
    return lo + norm * (hi - lo);
}
} // namespace

const std::vector<Wave9090AudioProcessorEditor::ParamUiSpec>& Wave9090AudioProcessorEditor::allUiSpecs()
{
    static const std::vector<ParamUiSpec> kSpecs = {
        { PID::outputGain, "Output", "master.output", "Master output level (dB)." },
        { PID::wtPosition, "Wavetable", "osc.wtPosition", "Wavetable morph position." },
        { PID::wtScan, "Scan", "osc.wtScan", "Wavetable scan LFO depth." },
        { PID::subLevel, "Sub", "osc.sub", "Sub oscillator level." },
        { PID::detune, "Detune", "osc.detune", "Fine detune (cents)." },
        { PID::glide, "Glide", "osc.glide", "Portamento glide time (ms)." },
        { PID::filterCutoff, "Cutoff", "filter.cutoff", "Lowpass cutoff." },
        { PID::filterRes, "Res", "filter.res", "Filter resonance." },
        { PID::drive, "Drive", "filter.drive", "Saturation drive." },
        { PID::crush, "Crush", "filter.crush", "Bit/sample crush (destructive path)." },
        { PID::tapeWobble, "Tape", "macro.tape", "Wow/flutter + hiss macro." },
        { PID::ampAttack, "Amp A", "env.ampAttack", "Amp envelope attack." },
        { PID::ampDecay, "Amp D", "env.ampDecay", "Amp envelope decay." },
        { PID::ampSustain, "Amp S", "env.ampSustain", "Amp envelope sustain." },
        { PID::ampRelease, "Amp R", "env.ampRelease", "Amp envelope release." },
        { PID::filtAttack, "Filt A", "env.filtAttack", "Filter envelope attack." },
        { PID::filtDecay, "Filt D", "env.filtDecay", "Filter envelope decay." },
        { PID::filtSustain, "Filt S", "env.filtSustain", "Filter envelope sustain." },
        { PID::filtRelease, "Filt R", "env.filtRelease", "Filter envelope release." },
        { PID::filtEnvAmt, "F Env", "env.filtAmt", "Filter envelope amount." },
        { PID::stereoWidth, "Width", "out.width", "Stereo spread." },
    };
    return kSpecs;
}

Wave9090AudioProcessorEditor::Wave9090AudioProcessorEditor (Wave9090AudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor_ (p),
      titleLabel_ ({}, "WAVE-9090"),
      meterBar_ (meterNorm_)
{
    setLookAndFeel (&laf_);
    tooltipWindow_ = std::make_unique<juce::TooltipWindow> (this, 450);
    setResizable (true, true);
    setResizeLimits (wave9090::ui::Layout::editorWidth - 200, 520,
                     wave9090::ui::Layout::editorWidth + 240, 900);
    setSize (wave9090::ui::Layout::editorWidth, wave9090::ui::Layout::editorHeight);

    titleLabel_.setFont (juce::Font (20.0f, juce::Font::bold));
    titleLabel_.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel_);

    for (int i = 0; i < p.getNumPrograms(); ++i)
        presetBox_.addItem (p.getProgramName (i), i + 1);
    presetBox_.setSelectedId (p.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox_.setTooltip (tooltip ("Preset", "Factory program — design slot: header.preset"));
    presetBox_.onChange = [this]
    {
        processor_.applyFactoryPreset (presetBox_.getSelectedId() - 1);
    };
    addAndMakeVisible (presetBox_);

    buildControls();

    circuitButton_.setClickingTogglesState (true);
    circuitButton_.setTooltip (tooltip ("Destructive Circuit", "APVTS id filterCircuit — design slot: filter.circuitToggle"));
    addAndMakeVisible (circuitButton_);
    circuitAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), PID::filterCircuit, circuitButton_);

    keyboard_.setKeyWidth (15.0);
    addAndMakeVisible (keyboard_);

    meterLabel_.setText ("OUTPUT", juce::dontSendNotification);
    meterLabel_.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (meterLabel_);
    addAndMakeVisible (meterBar_);

    startTimerHz (24);
}

void Wave9090AudioProcessorEditor::buildControls()
{
    for (const auto& spec : allUiSpecs())
    {
        auto knob = std::make_unique<wave9090::ui::LabeledKnob> (spec.label);
        const auto tip = tooltip (spec.label, spec.tooltip + " APVTS: " + spec.paramId
                                               + " — design slot: " + spec.designSlot);
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
        knobs_.push_back (std::move (knob));
    }
}

juce::String Wave9090AudioProcessorEditor::formatParamValue (const char* id, float norm)
{
    if (juce::String (id) == PID::outputGain)
        return juce::String (mapDb (norm, -24.0f, 12.0f), 1) + " dB";
    if (juce::String (id) == PID::detune)
        return juce::String (mapDb (norm, -50.0f, 50.0f), 1) + " ct";
    if (juce::String (id) == PID::glide)
        return juce::String (norm * 500.0f, 0) + " ms";

    const bool isTime = juce::String (id).contains ("Attack") || juce::String (id).contains ("Decay")
                     || juce::String (id).contains ("Release");
    if (isTime)
        return juce::String (norm * 2.0f, 2) + " s";

    return juce::String (juce::roundToInt (norm * 100.0f)) + "%";
}

wave9090::ui::LabeledKnob* Wave9090AudioProcessorEditor::knobForId (const char* id) noexcept
{
    const auto& specs = allUiSpecs();
    for (std::size_t i = 0; i < specs.size() && i < knobs_.size(); ++i)
        if (juce::String (specs[i].paramId) == id)
            return knobs_[i].get();
    return nullptr;
}

void Wave9090AudioProcessorEditor::paint (juce::Graphics& g)
{
    auto full = getLocalBounds().toFloat();
    const auto header = wave9090::ui::headerArea (getLocalBounds());
    const auto headerImg = wave9090::ui::SkinAssets::loadHeaderStrip();
    if (headerImg.isValid())
        g.drawImage (headerImg, header.toFloat(), juce::RectanglePlacement::stretchToFit);
    else
        g.fillAll (juce::Colour (0xff101018));

    const auto panel = laf_.getPanelBackground();
    auto body = getLocalBounds();
    body.removeFromTop (wave9090::ui::Layout::headerHeight);
    body.removeFromBottom (wave9090::ui::Layout::keyboardHeight + wave9090::ui::Layout::footerMeterHeight);
    if (panel.isValid())
        g.drawImage (panel, body.toFloat(), juce::RectanglePlacement::fillDestination);
    else
    {
        g.setColour (juce::Colour (0xff14141c));
        g.fillRect (body);
    }

    g.setColour (juce::Colour (0xff3a3a48));
    g.drawRoundedRectangle (full.reduced (4.0f), 8.0f, 1.2f);

    const char* sections[] = { "OSCILLATOR", "FILTER", "ENVELOPES", "MACRO / OUTPUT" };
    juce::Rectangle<int> sectionBounds = wave9090::ui::contentArea (getLocalBounds());
    const int colW = sectionBounds.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
    {
        auto r = sectionBounds.withX (sectionBounds.getX() + i * colW).withWidth (colW).reduced (6, 4);
        g.setColour (juce::Colour (0xff252532));
        g.fillRoundedRectangle (r.toFloat(), 6.0f);
        g.setColour (juce::Colour (0xff9898a8));
        g.setFont (juce::Font (11.0f, juce::Font::bold));
        g.drawText (sections[i], r.removeFromTop (20), juce::Justification::centred);
    }
}

void Wave9090AudioProcessorEditor::layoutControls()
{
    titleLabel_.setBounds (wave9090::ui::Layout::margin, 14, 220, 28);
    presetBox_.setBounds (getWidth() - 280, 12, 260, 28);

    auto content = wave9090::ui::contentArea (getLocalBounds());
    const int colW = content.getWidth() / 4;

    auto layoutColumn = [&] (int column, std::initializer_list<const char*> ids)
    {
        auto col = content.withX (content.getX() + column * colW).withWidth (colW).reduced (12, 28);
        const int rows = static_cast<int> (ids.size());
        const int cellH = col.getHeight() / juce::jmax (1, rows);
        int row = 0;
        for (const char* id : ids)
        {
            if (auto* k = knobForId (id))
                k->setBounds (col.getX(), col.getY() + row * cellH, col.getWidth(), cellH);
            ++row;
        }
    };

    layoutColumn (0, { PID::wtPosition, PID::wtScan, PID::subLevel, PID::detune, PID::glide });
    layoutColumn (1, { PID::filterCutoff, PID::filterRes, PID::drive, PID::crush });
    circuitButton_.setBounds (content.getX() + colW + 20, content.getY() + content.getHeight() - 56, colW - 40, 32);

    layoutColumn (2, { PID::ampAttack, PID::ampDecay, PID::ampSustain, PID::ampRelease,
                     PID::filtAttack, PID::filtDecay, PID::filtSustain, PID::filtRelease, PID::filtEnvAmt });
    layoutColumn (3, { PID::tapeWobble, PID::stereoWidth, PID::outputGain });

    auto footer = wave9090::ui::footerArea (getLocalBounds());
    meterLabel_.setBounds (footer.getX() + 12, footer.getY() + 8, 80, 24);
    meterBar_.setBounds (footer.getX() + 100, footer.getY() + 14, 260, 18);

    keyboard_.setBounds (wave9090::ui::keyboardArea (getLocalBounds()));
}

void Wave9090AudioProcessorEditor::resized()
{
    layoutControls();
}

void Wave9090AudioProcessorEditor::timerCallback()
{
    const float db = processor_.getOutputPeakDb();
    meterNorm_ = static_cast<double> (juce::jlimit (0.0f, 1.0f, juce::jmap (db, -48.0f, 0.0f, 0.0f, 1.0f)));
}

void Wave9090AudioProcessorEditor::renderEditorPng (const juce::File& outFile) const
{
    juce::Image img (juce::Image::ARGB, getWidth(), getHeight(), true);
    juce::Graphics g (img);
    const_cast<Wave9090AudioProcessorEditor*> (this)->paintEntireComponent (g, true);
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