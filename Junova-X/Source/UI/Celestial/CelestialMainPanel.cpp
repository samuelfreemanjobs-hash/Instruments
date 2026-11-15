#include "CelestialMainPanel.h"

#include "PluginProcessor.h"
#include "UI/JunovaLookAndFeel.h"

namespace junovax::ui::celestial
{
ModuleShell::ModuleShell (juce::String title, juce::Colour accent, juce::Colour fill)
    : title_ (std::move (title)), accent_ (accent), fill_ (fill)
{
    titleLabel_.setText (title_, juce::dontSendNotification);
    titleLabel_.setJustificationType (juce::Justification::centredLeft);
    titleLabel_.setColour (juce::Label::textColourId, Theme::textPrimary());
    titleLabel_.setFont (Theme::headerFont (11.0f));
    addAndMakeVisible (titleLabel_);
}

void ModuleShell::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour (fill_);
    g.fillRoundedRectangle (b, 8.0f);
    g.setColour (accent_.withAlpha (0.55f));
    g.drawRoundedRectangle (b.reduced (0.5f), 8.0f, 1.5f);
}

void ModuleShell::resized()
{
    titleLabel_.setBounds (10, 6, getWidth() - 20, 18);
}

juce::Rectangle<int> ModuleShell::getContentArea() const
{
    return getLocalBounds().reduced (8).withTrimmedTop (26);
}

CelestialMainPanel::CelestialMainPanel (JunovaXAudioProcessor& processor)
    : processor_ (processor), oscMonitor_ (processor), voiceLeds_ (processor)
{
    setLookAndFeel (&laf_);

    brand_.setFont (Theme::headerFont (24.0f));
    brand_.setColour (juce::Label::textColourId, Theme::electricCyan());
    tagline_.setFont (Theme::labelFont());
    tagline_.setColour (juce::Label::textColourId, Theme::textMuted());
    hostBar_.setFont (Theme::readoutFont (11.0f));
    hostBar_.setColour (juce::Label::textColourId, Theme::textMuted());
    oversampleBadge_.setFont (Theme::labelFont());
    oversampleBadge_.setColour (juce::Label::textColourId, Theme::ledInactive().brighter (0.4f));

    for (auto* b : { &voicePoly1_, &voicePoly2_, &voiceUnison_, &voiceMono_ })
    {
        b->setClickingTogglesState (false);
        addAndMakeVisible (*b);
    }
    constexpr float kVoiceSteps = 4.0f;
    voicePoly1_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (0.0f); };
    voicePoly2_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (1.0f / kVoiceSteps); };
    voiceUnison_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (2.0f / kVoiceSteps); };
    voiceMono_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (3.0f / kVoiceSteps); };

    addAndMakeVisible (brand_);
    addAndMakeVisible (tagline_);
    addAndMakeVisible (hostBar_);
    addAndMakeVisible (presetLcd_);
    addAndMakeVisible (oversampleBadge_);
    for (auto* b : { &saveBtn_, &initBtn_, &vaultBtn_, &abBtn_ })
        addAndMakeVisible (*b);

    saveBtn_.onClick = [this] { processor_.saveUserPresetToFile(); };
    initBtn_.onClick = [this] { processor_.initToDefaultPatch(); refreshPresetLabel(); };
    vaultBtn_.onClick = [this] { if (onOpenVault_) onOpenVault_(); };
    abBtn_.onClick = [this] { processor_.toggleAbSlot(); };

    presetLcd_.prev().onClick = [this]
    {
        const int n = processor_.getNumPrograms();
        if (n > 0)
            processor_.setCurrentProgram ((processor_.getCurrentProgram() + n - 1) % n);
        refreshPresetLabel();
    };
    presetLcd_.next().onClick = [this]
    {
        const int n = processor_.getNumPrograms();
        if (n > 0)
            processor_.setCurrentProgram ((processor_.getCurrentProgram() + 1) % n);
        refreshPresetLabel();
    };

    addAndMakeVisible (oscMonitor_);
    addAndMakeVisible (voiceLeds_);

    for (auto* m : { &lfoModule_, &dcoModule_, &hpfModule_, &vcfModule_, &envModule_,
                     &chorusModule_, &driftModule_, &arpModule_, &masterModule_ })
        addAndMakeVisible (*m);

    for (auto* b : { &lfoTri_, &lfoSqr_, &lfoSh_ })
        lfoModule_.addAndMakeVisible (*b);
    lfoModule_.addAndMakeVisible (lfoSync_);
    lfoSyncAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), junovax::ParameterIDs::lfoBpmSync, lfoSync_);
    wireChipRow ({ &lfoTri_, &lfoSqr_, &lfoSh_ }, junovax::ParameterIDs::lfoWaveform, 3);

    for (auto* b : { &dcoPulse_, &dcoSaw_, &dcoSub_, &dcoNoise_ })
        dcoModule_.addAndMakeVisible (*b);
    dcoPulse_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::dcoPulseOn)) p->setValueNotifyingHost (p->getValue() > 0.5f ? 0.0f : 1.0f); };
    dcoSaw_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::dcoSawOn)) p->setValueNotifyingHost (p->getValue() > 0.5f ? 0.0f : 1.0f); };
    dcoSub_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::dcoSubOn)) p->setValueNotifyingHost (p->getValue() > 0.5f ? 0.0f : 1.0f); };
    dcoNoise_.setEnabled (false);
    dcoNoise_.setTooltip ("Level via NOISE fader");

    for (auto* b : { &hpfOff_, &hpf1_, &hpf2_, &hpf3_ })
        hpfModule_.addAndMakeVisible (*b);
    hpfCaption_.setFont (Theme::labelFont());
    hpfCaption_.setColour (juce::Label::textColourId, Theme::textMuted());
    hpfCaption_.setJustificationType (juce::Justification::centred);
    hpfModule_.addAndMakeVisible (hpfCaption_);
    auto setHpf = [this] (float norm)
    {
        if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::hpfMode))
            p->setValueNotifyingHost (norm);
    };
    hpfOff_.onClick = [setHpf] { setHpf (0.0f); };
    hpf1_.onClick = [setHpf] { setHpf (0.33f); };
    hpf2_.onClick = [setHpf] { setHpf (0.66f); };
    hpf3_.onClick = [setHpf] { setHpf (1.0f); };

    lfoFaders_.push_back (&addFader (lfoModule_, junovax::ParameterIDs::lfoRate, "RATE", junovax::ui::JunovaLookAndFeel::Accent::cyan));
    lfoFaders_.push_back (&addFader (lfoModule_, junovax::ParameterIDs::lfoDelay, "DELAY", junovax::ui::JunovaLookAndFeel::Accent::cyan));
    lfoFaders_.push_back (&addFader (lfoModule_, junovax::ParameterIDs::glide, "GLIDE", junovax::ui::JunovaLookAndFeel::Accent::cyan));

    dcoFaders_.push_back (&addFader (dcoModule_, junovax::ParameterIDs::dcoLfoMod, "LFO", junovax::ui::JunovaLookAndFeel::Accent::cyan));
    dcoFaders_.push_back (&addFader (dcoModule_, junovax::ParameterIDs::dcoPwm, "PWM", junovax::ui::JunovaLookAndFeel::Accent::cyan));
    dcoFaders_.push_back (&addFader (dcoModule_, junovax::ParameterIDs::dcoSubLvl, "SUB", junovax::ui::JunovaLookAndFeel::Accent::cyan));
    dcoFaders_.push_back (&addFader (dcoModule_, junovax::ParameterIDs::dcoNoise, "NOISE", junovax::ui::JunovaLookAndFeel::Accent::cyan));

    vcfFaders_.push_back (&addFader (vcfModule_, junovax::ParameterIDs::filterCutoff, "FREQ", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    vcfFaders_.push_back (&addFader (vcfModule_, junovax::ParameterIDs::filterRes, "RES", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    vcfFaders_.push_back (&addFader (vcfModule_, junovax::ParameterIDs::vcfEnv, "ENV", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    vcfFaders_.push_back (&addFader (vcfModule_, junovax::ParameterIDs::vcfLfo, "LFO", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    vcfFaders_.push_back (&addFader (vcfModule_, junovax::ParameterIDs::vcfKey, "KYBD", junovax::ui::JunovaLookAndFeel::Accent::crimson));

    ampFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::ampAttack, "A", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    ampFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::ampDecay, "D", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    ampFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::ampSustain, "S", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    ampFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::ampRelease, "R", junovax::ui::JunovaLookAndFeel::Accent::crimson));

    filtFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::filtAttack, "FA", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    filtFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::filtDecay, "FD", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    filtFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::filtSustain, "FS", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    filtFaders_.push_back (&addFader (envModule_, junovax::ParameterIDs::filtRelease, "FR", junovax::ui::JunovaLookAndFeel::Accent::crimson));

    addChorusModeButtons (chorusModule_);

    driftFaders_.push_back (&addFader (driftModule_, junovax::ParameterIDs::drift, "DRIFT", junovax::ui::JunovaLookAndFeel::Accent::cyan));
    driftFaders_.push_back (&addFader (driftModule_, junovax::ParameterIDs::detune, "DETUNE", junovax::ui::JunovaLookAndFeel::Accent::crimson));
    driftFaders_.push_back (&addFader (driftModule_, junovax::ParameterIDs::width, "WIDTH", junovax::ui::JunovaLookAndFeel::Accent::chrome));

    arpModule_.addAndMakeVisible (arpOn_);
    arpModule_.addAndMakeVisible (arpLatch_);
    arpOnAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), junovax::ParameterIDs::arpEnabled, arpOn_);
    arpLatchAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), junovax::ParameterIDs::arpLatch, arpLatch_);

    arpFaders_.push_back (&addFader (arpModule_, junovax::ParameterIDs::arpRange, "RANGE", junovax::ui::JunovaLookAndFeel::Accent::cyan));
    arpFaders_.push_back (&addFader (arpModule_, junovax::ParameterIDs::arpRate, "RATE", junovax::ui::JunovaLookAndFeel::Accent::cyan));

    masterFader_ = &addFader (masterModule_, junovax::ParameterIDs::masterGain, "VOL", junovax::ui::JunovaLookAndFeel::Accent::cyan);
    masterModule_.addAndMakeVisible (peakMeter_);

    addAndMakeVisible (pitchWheel_);
    addAndMakeVisible (modWheel_);
    for (auto* b : { &transposeDown_, &transposeZero_, &transposeUp_ })
        addAndMakeVisible (*b);
    transposeDown_.onClick = [this]
    {
        processor_.setMidiTransposeSemitones (processor_.getMidiTransposeSemitones() - 12);
        refreshTransposeUi();
    };
    transposeZero_.onClick = [this]
    {
        processor_.setMidiTransposeSemitones (0);
        refreshTransposeUi();
    };
    transposeUp_.onClick = [this]
    {
        processor_.setMidiTransposeSemitones (processor_.getMidiTransposeSemitones() + 12);
        refreshTransposeUi();
    };

    keyboard_ = std::make_unique<juce::MidiKeyboardComponent> (
        processor_.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard_->setAvailableRange (36, 96);
    keyboard_->setColour (juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour (0xffe8ecf4));
    keyboard_->setColour (juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour (0xff0a0e16));
    keyboard_->setColour (juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (*keyboard_);

    transposeLabel_.setFont (Theme::labelFont());
    transposeLabel_.setColour (juce::Label::textColourId, Theme::textMuted());
    transposeLabel_.setJustificationType (juce::Justification::centred);
    ampEnvTag_.setFont (Theme::labelFont());
    ampEnvTag_.setColour (juce::Label::textColourId, Theme::electricCyan());
    filtEnvTag_.setFont (Theme::labelFont());
    filtEnvTag_.setColour (juce::Label::textColourId, Theme::stellarCrimson());
    envModule_.addAndMakeVisible (ampEnvTag_);
    envModule_.addAndMakeVisible (filtEnvTag_);
    addAndMakeVisible (transposeLabel_);
    ampEnvTag_.setText ("VCA ADSR", juce::dontSendNotification);
    filtEnvTag_.setText ("VCF ADSR", juce::dontSendNotification);
    transposeLabel_.setText ("TRANSPOSE", juce::dontSendNotification);

    refreshPresetLabel();
    refreshHostBar();
    refreshTransposeUi();
    startTimerHz (12);
}

CelestialMainPanel::~CelestialMainPanel()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void CelestialMainPanel::wireChipRow (const std::vector<juce::TextButton*>& buttons, const char* paramId, int numChoices)
{
    const float step = 1.0f / static_cast<float> (juce::jmax (1, numChoices - 1));
    int idx = 0;
    for (auto* b : buttons)
    {
        const float norm = step * static_cast<float> (idx++);
        b->onClick = [this, paramId, norm]
        {
            if (auto* p = processor_.getApvts().getParameter (paramId))
                p->setValueNotifyingHost (norm);
        };
    }
}

LabeledFader& CelestialMainPanel::addFader (juce::Component& parent, const char* paramId, const juce::String& label,
                                            junovax::ui::JunovaLookAndFeel::Accent accent)
{
    const auto labelColour = accent == junovax::ui::JunovaLookAndFeel::Accent::crimson ? Theme::stellarCrimson()
                                                                                        : Theme::electricCyan();
    auto f = std::make_unique<LabeledFader> (label, labelColour);
    junovax::ui::JunovaLookAndFeel::tagSlider (f->getSlider(), accent);
    parent.addAndMakeVisible (*f);
    sliderAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, f->getSlider()));
    auto& ref = *f;
    faders_.push_back (std::move (f));
    return ref;
}

void CelestialMainPanel::layoutFaderRow (juce::Rectangle<int> area, const std::vector<LabeledFader*>& row, int header)
{
    area = area.withTrimmedTop (header);
    if (row.empty())
        return;
    const int fw = juce::jmax (38, (area.getWidth() - (static_cast<int> (row.size()) - 1) * 4) / static_cast<int> (row.size()));
    int x = area.getX();
    for (auto* f : row)
    {
        f->setBounds (x, area.getY(), fw, area.getHeight());
        x += fw + 4;
    }
}

void CelestialMainPanel::timerCallback()
{
    refreshPresetLabel();
    refreshHostBar();
    syncPerformanceMeters();
}

void CelestialMainPanel::syncPerformanceMeters()
{
    peakLevel_ = processor_.getOutputPeak();
    voiceLeds_.setActiveMask (processor_.getActiveVoiceMask());
    peakMeter_.repaint();
}

int CelestialMainPanel::countWhiteKeysInRange (int lowNote, int highNote) noexcept
{
    static constexpr int kWhiteDeg[] = { 0, 2, 4, 5, 7, 9, 11 };
    int count = 0;
    for (int note = lowNote; note <= highNote; ++note)
    {
        const int deg = note % 12;
        for (int w : kWhiteDeg)
        {
            if (deg == w)
            {
                ++count;
                break;
            }
        }
    }
    return juce::jmax (1, count);
}

void CelestialMainPanel::refreshTransposeUi()
{
    transposeDisplay_ = processor_.getMidiTransposeSemitones();
    const bool active = transposeDisplay_ != 0;
    for (auto* b : { &transposeDown_, &transposeUp_ })
        b->setColour (juce::TextButton::buttonColourId, Theme::panelGrey().brighter (0.05f));
    transposeZero_.setColour (juce::TextButton::buttonColourId,
                              active ? Theme::electricCyan().withAlpha (0.25f) : Theme::panelGrey().brighter (0.05f));
    const juce::String suffix = active ? (" (" + juce::String (transposeDisplay_ > 0 ? "+" : "") + juce::String (transposeDisplay_) + " st)")
                                       : juce::String{};
    transposeLabel_.setText ("TRANSPOSE" + suffix, juce::dontSendNotification);
}

void CelestialMainPanel::refreshHostBar()
{
    const float bpm = processor_.getDisplayBpm();
    const int sr = static_cast<int> (processor_.getEffectiveSampleRate() + 0.5);
    hostBar_.setText ("BPM " + juce::String (bpm, 2) + "  |  4/4  |  SR " + juce::String (sr) + " Hz",
                      juce::dontSendNotification);
}

void CelestialMainPanel::refreshPresetLabel()
{
    const auto name = processor_.getProgramName (processor_.getCurrentProgram()).toUpperCase();
    presetLcd_.setPresetText (name);
}

void CelestialMainPanel::addChorusModeButtons (juce::Component& parent)
{
    for (auto* b : { &chorusOff_, &chorusI_, &chorusII_, &chorusBoth_ })
    {
        b->setClickingTogglesState (false);
        parent.addAndMakeVisible (*b);
    }
    auto setMode = [this] (float norm)
    {
        if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::chorusMode))
            p->setValueNotifyingHost (norm);
    };
    chorusOff_.onClick = [setMode] { setMode (0.0f); };
    chorusI_.onClick = [setMode] { setMode (0.33f); };
    chorusII_.onClick = [setMode] { setMode (0.66f); };
    chorusBoth_.onClick = [setMode] { setMode (1.0f); };
}

void CelestialMainPanel::paint (juce::Graphics& g)
{
    g.fillAll (Theme::background());

    using L = junovax::ui::Layout;
    g.setColour (Theme::panelGrey());
    g.fillRect (0, 0, getWidth(), L::topBarHeight);
    g.setColour (Theme::panelBlue().darker (0.35f));
    g.fillRect (0, L::topBarHeight, getWidth(), L::heroHeight);

    if (! peakMeter_.getBounds().isEmpty())
    {
        auto meter = peakMeter_.getBounds().toFloat();
        g.setColour (Theme::ledInactive());
        g.fillRoundedRectangle (meter, 4.0f);
        const float h = meter.getHeight() * juce::jlimit (0.0f, 1.0f, peakLevel_ * 4.0f);
        g.setColour (Theme::electricCyan());
        g.fillRoundedRectangle (meter.getX() + 4.0f, meter.getBottom() - h, meter.getWidth() - 8.0f, h, 3.0f);
    }

    auto kb = keyboard_ != nullptr ? keyboard_->getBounds() : juce::Rectangle<int>{};
    if (! kb.isEmpty())
    {
        g.setColour (Theme::panelGrey().darker (0.2f));
        g.fillRoundedRectangle (getLocalBounds().withBottom (kb.getBottom()).withTop (kb.getY() - 4).toFloat(), 6.0f);
    }
}

void CelestialMainPanel::resized()
{
    using L = junovax::ui::Layout;
    const int w = getWidth();
    const int m = L::margin;

    hostBar_.setBounds (m, 4, w - 2 * m - 140, 20);
    oversampleBadge_.setBounds (w - m - 130, 4, 130, 20);

    const int heroY = L::topBarHeight;
    brand_.setBounds (m, heroY + 8, 200, 28);
    tagline_.setBounds (m, heroY + 36, 280, 14);

    const int lcdW = juce::jmin (360, w / 3);
    presetLcd_.setBounds (w / 2 - lcdW / 2, heroY + 8, lcdW, 30);
    saveBtn_.setBounds (presetLcd_.getRight() + 6, heroY + 10, 44, 24);
    initBtn_.setBounds (saveBtn_.getRight() + 4, heroY + 10, 40, 24);
    vaultBtn_.setBounds (initBtn_.getRight() + 4, heroY + 10, 52, 24);
    abBtn_.setBounds (vaultBtn_.getRight() + 4, heroY + 10, 36, 24);

    if (abBtn_.getRight() > w - m)
    {
        const int shift = abBtn_.getRight() - (w - m);
        presetLcd_.setBounds (presetLcd_.getX() - shift, presetLcd_.getY(), presetLcd_.getWidth(), presetLcd_.getHeight());
        saveBtn_.setBounds (saveBtn_.getX() - shift, saveBtn_.getY(), saveBtn_.getWidth(), saveBtn_.getHeight());
        initBtn_.setBounds (initBtn_.getX() - shift, initBtn_.getY(), initBtn_.getWidth(), initBtn_.getHeight());
        vaultBtn_.setBounds (vaultBtn_.getX() - shift, vaultBtn_.getY(), vaultBtn_.getWidth(), vaultBtn_.getHeight());
        abBtn_.setBounds (abBtn_.getX() - shift, abBtn_.getY(), abBtn_.getWidth(), abBtn_.getHeight());
    }

    oscMonitor_.setBounds (w / 2 - 200, heroY + 44, 400, 54);
    voiceLeds_.setBounds (w - m - 200, heroY + 10, 190, 40);

    voicePoly1_.setBounds (w - m - 198, heroY + 58, 46, 22);
    voicePoly2_.setBounds (w - m - 148, heroY + 58, 46, 22);
    voiceUnison_.setBounds (w - m - 98, heroY + 58, 48, 22);
    voiceMono_.setBounds (w - m - 46, heroY + 58, 40, 22);

    const int kbH = L::keyboardBayHeight;
    auto kbArea = getLocalBounds().removeFromBottom (kbH).reduced (m, 4);
    const int wheelW = 30;
    pitchWheel_.setBounds (kbArea.removeFromLeft (wheelW));
    modWheel_.setBounds (kbArea.removeFromLeft (wheelW).reduced (0, 2));
    const int trW = 56;
    auto tr = kbArea.removeFromRight (trW);
    transposeLabel_.setBounds (tr.getX(), tr.getY(), trW, 12);
    tr.removeFromTop (14);
    const int btnH = juce::jmax (20, (tr.getHeight() - 4) / 3);
    transposeUp_.setBounds (tr.removeFromTop (btnH).reduced (1));
    transposeZero_.setBounds (tr.removeFromTop (btnH).reduced (1));
    transposeDown_.setBounds (tr.reduced (1));
    if (keyboard_ != nullptr)
    {
        constexpr int low = 36;
        int high = 96;
        const int targetW = kbArea.getWidth();

        auto layoutKeys = [&]()
        {
            const int whites = countWhiteKeysInRange (low, high);
            const float kw = static_cast<float> (targetW) / static_cast<float> (whites);
            keyboard_->setAvailableRange (low, high);
            keyboard_->setKeyWidth (juce::jmax (10.0f, kw));
            keyboard_->resized();
            return static_cast<int> (std::ceil (static_cast<double> (whites) * static_cast<double> (keyboard_->getKeyWidth())));
        };

        int stripW = layoutKeys();
        while (stripW + 4 < targetW && high < 119)
        {
            ++high;
            stripW = layoutKeys();
        }

        keyboard_->setBounds (kbArea.withWidth (juce::jmin (targetW, stripW)));
    }

    auto modules = getLocalBounds().withTrimmedTop (heroY + L::heroHeight).withTrimmedBottom (kbH + m);
    const int gap = m;
    const int row1H = static_cast<int> (modules.getHeight() * 0.58f);
    const int row2H = modules.getHeight() - row1H - gap;

    auto row1 = modules.removeFromTop (row1H);
    modules.removeFromTop (gap);
    auto row2 = modules;

    const int innerW = w - 2 * m;
    constexpr int kHpfW = 108;
    const int flexW = innerW - kHpfW - gap * 4;
    const int colW = flexW / 4;
    int x1 = m;
    lfoModule_.setBounds (x1, row1.getY(), colW + 16, row1.getHeight());
    x1 += lfoModule_.getWidth() + gap;
    dcoModule_.setBounds (x1, row1.getY(), colW + 8, row1.getHeight());
    x1 += dcoModule_.getWidth() + gap;
    hpfModule_.setBounds (x1, row1.getY(), kHpfW, row1.getHeight());
    x1 += kHpfW + gap;
    vcfModule_.setBounds (x1, row1.getY(), colW + 8, row1.getHeight());
    x1 += vcfModule_.getWidth() + gap;
    envModule_.setBounds (x1, row1.getY(), w - m - x1, row1.getHeight());

    const int extraCh = juce::jmax (0, colW - kHpfW);
    const int w4 = (innerW - gap * 3) / 4;
    int x2 = m;
    chorusModule_.setBounds (x2, row2.getY(), w4 + extraCh, row2.getHeight());
    x2 += chorusModule_.getWidth() + gap;
    driftModule_.setBounds (x2, row2.getY(), w4, row2.getHeight());
    x2 += w4 + gap;
    arpModule_.setBounds (x2, row2.getY(), w4, row2.getHeight());
    x2 += w4 + gap;
    masterModule_.setBounds (x2, row2.getY(), w - m - x2, row2.getHeight());

    auto lfoArea = lfoModule_.getContentArea();
    const int chipH = 22;
    lfoTri_.setBounds (lfoArea.getX(), lfoArea.getY(), 36, chipH);
    lfoSqr_.setBounds (lfoArea.getX() + 40, lfoArea.getY(), 36, chipH);
    lfoSh_.setBounds (lfoArea.getX() + 80, lfoArea.getY(), 44, chipH);
    const int syncW = juce::jmin (92, lfoArea.getWidth() - 130);
    lfoSync_.setBounds (lfoArea.getRight() - syncW - 2, lfoArea.getY(), syncW, chipH);
    layoutFaderRow (lfoArea.withTrimmedTop (chipH + 6), lfoFaders_, 0);

    auto dcoArea = dcoModule_.getContentArea();
    dcoPulse_.setBounds (dcoArea.getX(), dcoArea.getY(), 48, chipH);
    dcoSaw_.setBounds (dcoArea.getX() + 52, dcoArea.getY(), 40, chipH);
    dcoSub_.setBounds (dcoArea.getX() + 96, dcoArea.getY(), 40, chipH);
    dcoNoise_.setBounds (dcoArea.getX() + 140, dcoArea.getY(), 52, chipH);
    layoutFaderRow (dcoArea, dcoFaders_, chipH + 4);

    auto hpfArea = hpfModule_.getContentArea();
    const int halfW = (hpfArea.getWidth() - 4) / 2;
    hpfOff_.setBounds (hpfArea.getX(), hpfArea.getY(), halfW, chipH);
    hpf1_.setBounds (hpfArea.getX() + halfW + 4, hpfArea.getY(), halfW, chipH);
    hpf2_.setBounds (hpfArea.getX(), hpfArea.getY() + chipH + 4, halfW, chipH);
    hpf3_.setBounds (hpfArea.getX() + halfW + 4, hpfArea.getY() + chipH + 4, halfW, chipH);
    hpfCaption_.setBounds (hpfArea.getX(), hpfArea.getBottom() - 16, hpfArea.getWidth(), 14);

    layoutFaderRow (vcfModule_.getContentArea(), vcfFaders_, 0);

    auto envArea = envModule_.getContentArea();
    ampEnvTag_.setBounds (envArea.getX(), envArea.getY(), envArea.getWidth(), 12);
    filtEnvTag_.setBounds (envArea.getX(), envArea.getY() + envArea.getHeight() / 2, envArea.getWidth(), 12);
    auto ampRow = envArea.withTrimmedTop (14).removeFromTop (envArea.getHeight() / 2 - 18);
    auto filtRow = envArea.withTrimmedTop (envArea.getHeight() / 2 + 2);
    layoutFaderRow (ampRow, ampFaders_, 0);
    layoutFaderRow (filtRow, filtFaders_, 14);

    auto chArea = chorusModule_.getContentArea();
    const int cbw = (chArea.getWidth() - 12) / 4;
    chorusOff_.setBounds (chArea.getX(), chArea.getY(), cbw, 24);
    chorusI_.setBounds (chArea.getX() + cbw + 4, chArea.getY(), cbw, 24);
    chorusII_.setBounds (chArea.getX() + (cbw + 4) * 2, chArea.getY(), cbw, 24);
    chorusBoth_.setBounds (chArea.getX() + (cbw + 4) * 3, chArea.getY(), cbw, 24);

    auto arpArea = arpModule_.getContentArea();
    arpOn_.setBounds (arpArea.getX(), arpArea.getY(), 56, chipH);
    arpLatch_.setBounds (arpArea.getX() + 60, arpArea.getY(), 56, chipH);
    layoutFaderRow (arpArea, arpFaders_, chipH + 4);

    layoutFaderRow (driftModule_.getContentArea(), driftFaders_, 0);

    if (masterFader_ != nullptr)
    {
        auto mArea = masterModule_.getContentArea();
        const int fW = juce::jmin (56, mArea.getWidth() / 2);
        masterFader_->setBounds (mArea.getX() + 4, mArea.getY(), fW, mArea.getHeight());
        peakMeter_.setBounds (mArea.getRight() - 16, mArea.getY() + 4, 10, mArea.getHeight() - 8);
    }
}

} // namespace junovax::ui::celestial