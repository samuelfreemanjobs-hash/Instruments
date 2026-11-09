#include "SynthEditor.h"
#include "MoogStackLineage.h"
#include "SynthParameterIds.h"
#include "VoyagerPresetBank.h"
#include "VoyagerUiTokens.h"

namespace
{
namespace c = voyagerui::colours;
const juce::Colour& kPanelMetal = c::panelMetal;
const juce::Colour& kPanelMetalHi = c::panelMetalHi;
const juce::Colour& kPanelInset = c::panelInset;
const juce::Colour& kPanelWood = c::panelWood;
const juce::Colour& kPanelWoodDark = c::panelWoodDark;
const juce::Colour& kAccent = c::defaultAccent;
const juce::Colour& kSectionText = c::sectionText;

void drawSection(juce::Graphics& g, juce::Rectangle<int> r, const juce::String& title)
{
    g.setColour(kPanelInset);
    g.fillRoundedRectangle(r.toFloat(), 8.0f);
    g.setColour(juce::Colours::black.withAlpha(0.25f));
    g.drawRoundedRectangle(r.toFloat().reduced(1.0f), 7.0f, 1.0f);
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

void VoyagerPanelLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                               float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                                               juce::Slider& slider)
{
    const float radius = juce::jmin(width, height) * 0.5f - 4.0f;
    const float centreX = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
    const float centreY = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;

    g.setColour(kPanelInset.darker(0.15f));
    g.fillEllipse(rx, ry, rw, rw);
    g.setColour(kPanelMetalHi.withAlpha(0.85f));
    g.drawEllipse(rx, ry, rw, rw, 1.6f);

    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(accent.withAlpha(0.92f));
    g.strokePath(valueArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    pointer.addRectangle(-2.0f, -radius + 6.0f, 4.0f, radius * 0.42f);
    g.setColour(accent.brighter(0.25f));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(kPanelMetal);
    g.fillEllipse(centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
}

void VoyagerPanelLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                               float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                               const juce::Slider::SliderStyle style, juce::Slider& slider)
{
  {
    const bool vertical = style == juce::Slider::LinearVertical || style == juce::Slider::LinearBarVertical;
    const auto track = vertical ? juce::Rectangle<float>(static_cast<float>(x) + width * 0.38f, static_cast<float>(y),
                                                         static_cast<float>(width) * 0.24f, static_cast<float>(height))
                                : juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y) + height * 0.38f,
                                                         static_cast<float>(width), static_cast<float>(height) * 0.24f);
    g.setColour(kPanelMetal);
    g.fillRoundedRectangle(track, 3.0f);
    g.setColour(kPanelInset.darker(0.05f));
    g.drawRoundedRectangle(track, 3.0f, 1.0f);

    if (vertical)
    {
        const float thumbY = sliderPos - 7.0f;
        g.setColour(accent);
        g.fillRoundedRectangle(track.getX() - 3.0f, thumbY, track.getWidth() + 6.0f, 14.0f, 3.0f);
        g.setColour(accent.brighter(0.35f));
        g.drawRoundedRectangle(track.getX() - 3.0f, thumbY, track.getWidth() + 6.0f, 14.0f, 3.0f, 1.0f);
    }
    else
    {
        const float thumbX = sliderPos - 7.0f;
        g.setColour(accent);
        g.fillRoundedRectangle(thumbX, track.getY() - 3.0f, 14.0f, track.getHeight() + 6.0f, 3.0f);
    }
    juce::ignoreUnused(slider);
  }
}

void WaveformSelectorStrip::paintWaveGlyph(juce::Graphics& g, juce::Rectangle<float> area, int waveIndex,
                                           juce::Colour colour)
{
    g.setColour(colour);
    area = area.reduced(4.0f, 6.0f);
    juce::Path p;
    switch (waveIndex)
    {
        case 0:
            p.startNewSubPath(area.getX(), area.getBottom());
            p.lineTo(area.getRight(), area.getY());
            break;
        case 1:
            p.addRectangle(area.getX(), area.getCentreY() - 3.0f, area.getWidth(), 6.0f);
            break;
        case 2:
            p.startNewSubPath(area.getX(), area.getBottom());
            p.lineTo(area.getCentreX(), area.getY());
            p.lineTo(area.getRight(), area.getBottom());
            break;
        default:
            p.addRectangle(area.getX(), area.getCentreY() - 3.0f, area.getWidth() * 0.45f, 6.0f);
            break;
    }
    g.strokePath(p, juce::PathStrokeType(1.5f));
}

void WaveformSelectorStrip::bind(MoogVoyagerAudioProcessor& processor, std::string_view waveParamId, int oscIndex)
{
    proc = &processor;
    paramKey = std::string(waveParamId);
    const juce::StringArray labels { "SAW", "SQR", "TRI", "PLS" };
    for (int i = 0; i < 4; ++i)
    {
        waveButtons[static_cast<size_t>(i)].setButtonText(labels[i]);
        waveButtons[static_cast<size_t>(i)].setRadioGroupId(voyagerui::components::waveSelectorRadioBaseId + oscIndex);
        waveButtons[static_cast<size_t>(i)].setClickingTogglesState(true);
        waveButtons[static_cast<size_t>(i)].setColour(juce::TextButton::buttonColourId, kPanelInset);
        waveButtons[static_cast<size_t>(i)].setColour(juce::TextButton::textColourOffId, kSectionText.withAlpha(0.8f));
        waveButtons[static_cast<size_t>(i)].onClick = [this, i]() { selectWave(i); };
        addAndMakeVisible(waveButtons[static_cast<size_t>(i)]);
    }
    syncFromParameter();
}

void WaveformSelectorStrip::setAccentColour(juce::Colour c) noexcept
{
    accent = c;
    for (auto& b : waveButtons)
        b.setColour(juce::TextButton::buttonOnColourId, accent);
    repaint();
}

void WaveformSelectorStrip::syncFromParameter()
{
    if (proc == nullptr)
        return;
    const int idx = static_cast<int>(
        proc->getApvts().getRawParameterValue(paramKey)->load());
    for (int i = 0; i < 4; ++i)
        waveButtons[static_cast<size_t>(i)].setToggleState(i == idx, juce::dontSendNotification);
    repaint();
}

void WaveformSelectorStrip::selectWave(int index)
{
    if (proc == nullptr)
        return;
    index = juce::jlimit(0, 3, index);
    if (auto* param = proc->getApvts().getParameter(paramKey))
        param->setValueNotifyingHost(index / 3.0f);
    syncFromParameter();
}

void WaveformSelectorStrip::resized()
{
    auto b = getLocalBounds();
    const int w = b.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
        waveButtons[static_cast<size_t>(i)].setBounds(b.removeFromLeft(w).reduced(1, 0));
}

bool ModBusMatrixPanel::hitCell(juce::Point<float> pos, int& rowOut, int& colOut) const
{
    auto r = getLocalBounds().toFloat().reduced(2.0f);
    const float halfW = r.getWidth() * 0.5f;
    for (int col = 0; col < 2; ++col)
    {
        auto bus = r.withWidth(halfW).withX(r.getX() + col * halfW);
        const float cx = bus.getCentreX();
        for (int row = 0; row < 4; ++row)
        {
            const float y = bus.getY() + 18.0f + row * 22.0f;
            juce::Rectangle<float> dot(cx - 10.0f, y, 20.0f, 20.0f);
            if (dot.contains(pos))
            {
                rowOut = row;
                colOut = col;
                return true;
            }
        }
    }
    return false;
}

void ModBusMatrixPanel::mouseDown(const juce::MouseEvent& e)
{
    int row = 0;
    int col = 0;
    if (!hitCell(e.position, row, col))
        return;
    routes[static_cast<size_t>(row)][static_cast<size_t>(col)] =
        !routes[static_cast<size_t>(row)][static_cast<size_t>(col)];
    repaint();
}

void ModBusMatrixPanel::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced(2.0f);
    g.setColour(kPanelInset.darker(0.05f));
    g.fillRoundedRectangle(r, 4.0f);

    const juce::String sources[4] = { "LFO", "NOISE", "ENV", "OSC 3" };
    const juce::String busTitles[2] = { "MOD BUS A", "MOD BUS B" };

    g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    g.setColour(kSectionText.withAlpha(0.85f));
    const float halfW = r.getWidth() * 0.5f;
    for (int col = 0; col < 2; ++col)
    {
        auto bus = r.withWidth(halfW).withX(r.getX() + col * halfW);
        g.drawText(busTitles[col], bus.removeFromTop(14.0f).toNearestInt(), juce::Justification::centred);
        g.setColour(kPanelWood.withAlpha(0.55f));
        if (col == 0)
            g.drawVerticalLine(static_cast<int>(bus.getRight()), static_cast<int>(r.getY()), static_cast<int>(r.getBottom()));
        for (int row = 0; row < 4; ++row)
        {
            const float y = bus.getY() + 18.0f + row * 22.0f;
            g.setColour(kSectionText.withAlpha(0.75f));
            g.drawText(sources[row], (int) bus.getX() + 4, (int) y, (int) (halfW * 0.55f), 18,
                       juce::Justification::centredRight);
            const float cx = bus.getCentreX() + halfW * 0.18f;
            const bool on = routes[static_cast<size_t>(row)][static_cast<size_t>(col)];
            g.setColour(on ? accent : kPanelMetalHi);
            g.fillEllipse(cx, y + 4.0f, 10.0f, 10.0f);
            if (on)
            {
                g.setColour(accent.brighter(0.35f));
                g.drawEllipse(cx, y + 4.0f, 10.0f, 10.0f, 1.2f);
            }
        }
    }
}

namespace
{
void setApvtsFloat(juce::AudioProcessorValueTreeState& apvts, std::string_view id, float value)
{
    if (auto* param = apvts.getParameter(std::string(id)))
        param->setValueNotifyingHost(param->convertTo0to1(value));
}
} // namespace

VoyagerPerformanceStrip::VoyagerPerformanceStrip(MoogVoyagerAudioProcessor& p, juce::MidiKeyboardComponent& kb)
    : processor(p),
      keyboard(kb)
{
    addAndMakeVisible(keyboard);
    keyboard.setKeyWidth(15.5f);
    keyboard.setScrollButtonsVisible(false);
    keyboard.setLowestVisibleKey(36);
    keyboard.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colours::white);
    keyboard.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff1a1a1a));
    keyboard.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colours::black.withAlpha(0.55f));
}

void VoyagerPerformanceStrip::paint(juce::Graphics& g)
{
    g.fillAll(kPanelInset.darker(0.08f));
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    auto xy = bounds.removeFromLeft(92.0f).reduced(2.0f);
    xyBounds = xy;
    g.setColour(kPanelMetal);
    g.fillRoundedRectangle(xy, 4.0f);
    g.setColour(accent.withAlpha(0.35f));
    const float px = xy.getX() + xyX * xy.getWidth();
    const float py = xy.getY() + (1.0f - xyY) * xy.getHeight();
    g.fillEllipse(px - 5.0f, py - 5.0f, 10.0f, 10.0f);
    g.setColour(kSectionText.withAlpha(0.7f));
    g.setFont(juce::FontOptions(9.0f));
  {
    auto xyLabel = xy.toNearestInt();
    g.drawText("TOUCH", xyLabel.removeFromTop(10), juce::Justification::centred);
    g.drawText("SURFACE", xyLabel.removeFromTop(10), juce::Justification::centred);
  }

    auto wheels = bounds.removeFromLeft(52.0f);
    const auto drawWheel = [&](float x, float norm, const char* name, juce::Rectangle<float>& store) {
        juce::Rectangle<float> wr(x, wheels.getY() + 14.0f, 20.0f, wheels.getHeight() - 22.0f);
        store = wr;
        g.setColour(kPanelMetal);
        g.fillRoundedRectangle(wr, 3.0f);
        g.setColour(accent);
        const float y = wr.getY() + (1.0f - norm) * (wr.getHeight() - 6.0f);
        g.fillRoundedRectangle(wr.getX() + 4.0f, y, wr.getWidth() - 8.0f, 6.0f, 2.0f);
        g.setColour(kSectionText.withAlpha(0.65f));
        g.drawText(name, (int) wr.getX() - 2, (int) wheels.getY(), 26, 12, juce::Justification::centred);
    };
    drawWheel(wheels.getX() + 4.0f, pitchWheel, "PITCH", pitchWheelBounds);
    drawWheel(wheels.getX() + 28.0f, modWheel, "MOD", modWheelBounds);
}

void VoyagerPerformanceStrip::applyPerformanceToEngine()
{
    auto& apvts = processor.getApvts();
    setApvtsFloat(apvts, SynthParamIDs::lfoToFilter, xyX * 0.65f);
    setApvtsFloat(apvts, SynthParamIDs::lfoToPitch, xyY * 0.35f);
    setApvtsFloat(apvts, SynthParamIDs::filterEnvAmount, juce::jlimit(0.0f, 1.0f, modWheel * 1.1f));
    setApvtsFloat(apvts, SynthParamIDs::osc1Fine, (pitchWheel - 0.5f) * 80.0f);
}

void VoyagerPerformanceStrip::resized()
{
    auto b = getLocalBounds().reduced(6, 4);
    b.removeFromLeft(148);
    keyboard.setBounds(b);
}

void VoyagerPerformanceStrip::mouseDown(const juce::MouseEvent& e) { mouseDrag(e); }

void VoyagerPerformanceStrip::mouseDrag(const juce::MouseEvent& e)
{
    if (xyBounds.contains(e.position))
    {
        xyX = juce::jlimit(0.0f, 1.0f, (e.position.x - xyBounds.getX()) / xyBounds.getWidth());
        xyY = juce::jlimit(0.0f, 1.0f, 1.0f - (e.position.y - xyBounds.getY()) / xyBounds.getHeight());
        applyPerformanceToEngine();
        repaint();
        return;
    }
    if (pitchWheelBounds.contains(e.position))
    {
        pitchWheel = juce::jlimit(0.0f, 1.0f, 1.0f - (e.position.y - pitchWheelBounds.getY()) / pitchWheelBounds.getHeight());
        applyPerformanceToEngine();
        repaint();
        return;
    }
    if (modWheelBounds.contains(e.position))
    {
        modWheel = juce::jlimit(0.0f, 1.0f, 1.0f - (e.position.y - modWheelBounds.getY()) / modWheelBounds.getHeight());
        applyPerformanceToEngine();
        repaint();
    }
}

void VoyagerPerformanceStrip::mouseUp(const juce::MouseEvent&) { applyPerformanceToEngine(); }

void MeterScopePanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    g.setColour(kPanelInset);
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(kSectionText.withAlpha(0.75f));
    g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    g.drawText("OUTPUT", bounds.removeFromTop(12.0f).toNearestInt(), juce::Justification::centredLeft);

    const float peakDb = processor.getOutputPeakDb();
    const float peakNorm = juce::jmap(juce::jlimit(-48.0f, 6.0f, peakDb), -48.0f, 6.0f, 0.0f, 1.0f);
    auto meter = bounds.removeFromRight(bounds.getWidth() * 0.22f).reduced(3.0f);
    g.setColour(kPanelMetal);
    g.fillRoundedRectangle(meter, 2.0f);
    g.setColour(accent);
    const float h = meter.getHeight() * peakNorm;
    g.fillRoundedRectangle(meter.getX(), meter.getBottom() - h, meter.getWidth(), h, 2.0f);
    g.setColour(kSectionText.withAlpha(0.8f));
    g.setFont(juce::FontOptions(9.0f));
    g.drawText("OUT", meter.removeFromTop(12), juce::Justification::centred);

    std::array<float, 256> scope {};
    processor.getScopeSnapshot(scope);
    juce::Path wave;
    const float midY = bounds.getCentreY();
    for (int i = 0; i < 256; ++i)
    {
        const float x = bounds.getX() + (static_cast<float>(i) / 255.0f) * bounds.getWidth();
        const float y = midY - scope[static_cast<size_t>(i)] * bounds.getHeight() * 0.42f;
        if (i == 0)
            wave.startNewSubPath(x, y);
        else
            wave.lineTo(x, y);
    }
    g.setColour(accent.withAlpha(0.9f));
    g.strokePath(wave, juce::PathStrokeType(1.2f));
    const int w1 = static_cast<int>(
        processor.getApvts().getRawParameterValue(std::string(SynthParamIDs::osc1Wave))->load());
    static const char* waveCaptions[] = { "Sawtooth", "Square", "Triangle", "Pulse width" };
    const int idx = juce::jlimit(0, 3, w1);
    g.setColour(kSectionText.withAlpha(0.55f));
    g.setFont(juce::FontOptions(8.5f));
    g.drawText(juce::String(waveCaptions[idx]) + " · live", bounds.removeFromBottom(14).toNearestInt(),
               juce::Justification::centred);
}

void MoogVoyagerAudioProcessorEditor::styleKnob(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, kAccent);
    s.setColour(juce::Slider::thumbColourId, kAccent.brighter(0.2f));
}

void MoogVoyagerAudioProcessorEditor::styleMixFader(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::LinearVertical);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setColour(juce::Slider::backgroundColourId, kPanelInset.darker(0.15f));
    s.setColour(juce::Slider::trackColourId, kPanelMetalHi);
    s.setColour(juce::Slider::thumbColourId, kAccent);
}

void MoogVoyagerAudioProcessorEditor::styleEnvSlider(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::LinearVertical);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 52, 14);
    s.setColour(juce::Slider::backgroundColourId, kPanelInset.darker(0.1f));
    s.setColour(juce::Slider::trackColourId, kPanelMetalHi.withAlpha(0.6f));
    s.setColour(juce::Slider::thumbColourId, kAccent);
}

void MoogVoyagerAudioProcessorEditor::applyAccentToSlider(juce::Slider& s, juce::Colour accent, bool isFader) const
{
    if (isFader)
        s.setColour(juce::Slider::thumbColourId, accent);
    else
    {
        s.setColour(juce::Slider::rotarySliderFillColourId, accent);
        s.setColour(juce::Slider::thumbColourId, accent.brighter(0.2f));
    }
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
      processorRef(p),
      meterScope(p),
      midiKeyboard(p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard),
      performanceStrip(p, midiKeyboard)
{
    setOpaque(true);

    titleLabel.setText("VOYAGER-V", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, voyagerui::colours::titleGold);
    titleLabel.setFont(juce::FontOptions(voyagerui::type::titlePt, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("FOUR-STACK MOOG ENGINE  ·  SYSTEMS ENGINEERING AUDIO", juce::dontSendNotification);
    subtitleLabel.setJustificationType(juce::Justification::centredLeft);
    subtitleLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.75f));
    subtitleLabel.setFont(juce::FontOptions(11.0f, juce::Font::plain));
    addAndMakeVisible(subtitleLabel);

    patchReadoutLabel.setJustificationType(juce::Justification::centred);
    patchReadoutLabel.setColour(juce::Label::textColourId, kAccent);
    patchReadoutLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    addAndMakeVisible(patchReadoutLabel);

    stackBadgeLabel.setJustificationType(juce::Justification::centredLeft);
    stackBadgeLabel.setFont(juce::FontOptions(15.0f, juce::Font::bold));
    addAndMakeVisible(stackBadgeLabel);

    stackSwitchLabel.setText("STACK MODE", juce::dontSendNotification);
    stackSwitchLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    stackSwitchLabel.setJustificationType(juce::Justification::centredLeft);
    stackSwitchLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    addAndMakeVisible(stackSwitchLabel);

    const juce::String shortNames[4] = { "VOYAGER", "SUB PHATTY", "SUB 37", "MODEL D" };
    for (int i = 0; i < moogstack::lineageCount(); ++i)
    {
        auto& btn = stackModeSwitch[static_cast<size_t>(i)];
        btn.setButtonText(shortNames[i]);
        btn.setRadioGroupId(voyagerui::components::stackModeRadioGroupId);
        btn.setClickingTogglesState(true);
        btn.setColour(juce::TextButton::buttonColourId, kPanelInset);
        btn.setColour(juce::TextButton::textColourOffId, kSectionText);
        btn.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        btn.onClick = [this, i]() { selectStackMode(i, true); };
        addAndMakeVisible(btn);
    }

    processorRef.getApvts().addParameterListener(std::string(SynthParamIDs::moogStack), this);

    presetLabel.setText("Factory preset", juce::dontSendNotification);
    presetLabel.setColour(juce::Label::textColourId, kSectionText);
    presetLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(presetLabel);

    for (int i = 0; i < voyager::getFactoryPresetCount(); ++i)
        presetBox.addItem(juce::String(voyager::getFactoryPresetName(i).data()), i + 1);
    presetBox.setSelectedItemIndex(processorRef.getCurrentProgram(), juce::dontSendNotification);
    presetBox.onChange = [this]() {
        processorRef.setCurrentProgram(presetBox.getSelectedItemIndex());
        refreshPatchReadout();
        refreshStackChrome();
    };
    addAndMakeVisible(presetBox);

    auto stepPreset = [this](int delta) {
        const int n = voyager::getFactoryPresetCount();
        int p = processorRef.getCurrentProgram() + delta;
        if (p < 0)
            p = n - 1;
        if (p >= n)
            p = 0;
        processorRef.setCurrentProgram(p);
        presetBox.setSelectedItemIndex(p, juce::dontSendNotification);
        refreshPatchReadout();
    };
    presetPrevButton.setButtonText("◀");
    presetNextButton.setButtonText("▶");
    presetPrevButton.setColour(juce::TextButton::buttonColourId, kPanelInset);
    presetPrevButton.setColour(juce::TextButton::textColourOffId, kSectionText);
    presetNextButton.setColour(juce::TextButton::buttonColourId, kPanelInset);
    presetNextButton.setColour(juce::TextButton::textColourOffId, kSectionText);
    presetPrevButton.onClick = [stepPreset]() { stepPreset(-1); };
    presetNextButton.onClick = [stepPreset]() { stepPreset(1); };
    addAndMakeVisible(presetPrevButton);
    addAndMakeVisible(presetNextButton);

    filterEnvRowLabel.setText("FILTER", juce::dontSendNotification);
    ampEnvRowLabel.setText("AMP", juce::dontSendNotification);
    for (auto* l : { &filterEnvRowLabel, &ampEnvRowLabel })
    {
        l->setJustificationType(juce::Justification::centredLeft);
        l->setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
        l->setFont(juce::FontOptions(11.0f, juce::Font::bold));
        addAndMakeVisible(*l);
    }

    oversampleLabel.setText("OS", juce::dontSendNotification);
    oversampleLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    addAndMakeVisible(oversampleLabel);
    bindCombo(oversampleBox, { "Off", "2×", "4×" });
    oversampleBox.setVisible(false);

    const juce::StringArray osLabels { "OFF", "2×", "4×" };
    for (int i = 0; i < 3; ++i)
    {
        oversampleSwitch[static_cast<size_t>(i)].setButtonText(osLabels[i]);
        oversampleSwitch[static_cast<size_t>(i)].setRadioGroupId(voyagerui::components::oversampleRadioGroupId);
        oversampleSwitch[static_cast<size_t>(i)].setClickingTogglesState(true);
        oversampleSwitch[static_cast<size_t>(i)].setColour(juce::TextButton::buttonColourId, kPanelInset);
        oversampleSwitch[static_cast<size_t>(i)].onClick = [this, i]() {
            if (auto* param = processorRef.getApvts().getParameter(std::string(SynthParamIDs::oversample)))
            {
                const float norm = i / 2.0f;
                param->setValueNotifyingHost(norm);
            }
        };
        addAndMakeVisible(oversampleSwitch[static_cast<size_t>(i)]);
    }

    addAndMakeVisible(modBusMatrix);

    osc23SyncButton.setButtonText("OSC 2/3 SYNC");
    osc23SyncButton.setColour(juce::ToggleButton::textColourId, kSectionText);
    addAndMakeVisible(osc23SyncButton);

    addAndMakeVisible(meterScope);

    outputSectionLabel.setText("OUTPUT & VISUALIZER", juce::dontSendNotification);
    outputSectionLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    outputSectionLabel.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    addAndMakeVisible(outputSectionLabel);

    cpuStatusLabel.setText("CPU", juce::dontSendNotification);
    cpuStatusLabel.setJustificationType(juce::Justification::centred);
    cpuStatusLabel.setFont(juce::FontOptions(8.0f));
    addAndMakeVisible(cpuStatusLabel);

    addAndMakeVisible(performanceStrip);

    modLfoLabel.setText("LFO", juce::dontSendNotification);
    modLfoLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    modLfoLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    addAndMakeVisible(modLfoLabel);

    modWheelPitchLabel.setText("CONTROL WHEEL · PITCH", juce::dontSendNotification);
    modWheelFilterLabel.setText("CONTROL WHEEL · FILTER", juce::dontSendNotification);
    for (auto* l : { &modWheelPitchLabel, &modWheelFilterLabel })
    {
        l->setColour(juce::Label::textColourId, kSectionText.withAlpha(0.75f));
        l->setFont(juce::FontOptions(8.5f, juce::Font::bold));
        l->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*l);
    }

    for (auto* tb : { &noisePinkButton, &mixerOdButton, &filterOdButton })
    {
        tb->setClickingTogglesState(true);
        tb->setColour(juce::ToggleButton::textColourId, kSectionText);
        tb->setColour(juce::ToggleButton::tickColourId, kAccent);
        tb->setColour(juce::ToggleButton::tickDisabledColourId, kPanelMetalHi);
        addAndMakeVisible(*tb);
    }
    noisePinkButton.setButtonText("White / Pink");
    mixerOdButton.setButtonText("Mix OD");
    filterOdButton.setButtonText("Filt OD");

    refreshPatchReadout();
    refreshStackChrome();

    for (auto* lab : { &oscSectionLabel, &filterSectionLabel, &envSectionLabel, &modSectionLabel })
    {
        addAndMakeVisible(*lab);
    }
    sectionLabel(oscSectionLabel, "OSCILLATOR BANK");
    sectionLabel(filterSectionLabel, "FILTER");
    sectionLabel(envSectionLabel, "FILTER & AMP ENVELOPES");
    sectionLabel(modSectionLabel, "MODULATION BUS");

    const juce::StringArray waves { "Saw", "Square", "Triangle", "Pulse" };
    const juce::StringArray octaves { "-2", "-1", "0", "+1", "+2" };
    const std::string_view waveIds[3] = { SynthParamIDs::osc1Wave, SynthParamIDs::osc2Wave, SynthParamIDs::osc3Wave };

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

        styleMixFader(oscLevel[i], "Mix");
        styleKnob(oscFine[i], "Fine");
        bindCombo(oscOctave[i], octaves);
        bindCombo(oscWave[i], waves);
        oscWave[i].setVisible(false);
        waveSelectors[static_cast<size_t>(i)].bind(processorRef, waveIds[i], i);
        addAndMakeVisible(waveSelectors[static_cast<size_t>(i)]);
        addAndMakeVisible(oscLevel[i]);
        addAndMakeVisible(oscFine[i]);
        addAndMakeVisible(oscOctave[i]);
        addAndMakeVisible(oscWave[i]);
    }

    styleKnob(noiseSlider, "Noise");
    styleKnob(subOscSlider, "Sub −1");
    addAndMakeVisible(noiseSlider);
    addAndMakeVisible(subOscSlider);

    styleKnob(filterCutoff, "Cutoff");
    styleKnob(filterRes, "Resonance");
    styleKnob(filterDrive, "Drive");
    styleKnob(filterEnvAmt, "F.Env");
    styleKnob(filterKb, "KB Track");
    styleEnvSlider(fAttack, "A");
    styleEnvSlider(fDecay, "D");
    styleEnvSlider(fSustain, "S");
    styleEnvSlider(fRelease, "R");
    styleEnvSlider(aAttack, "A");
    styleEnvSlider(aDecay, "D");
    styleEnvSlider(aSustain, "S");
    styleEnvSlider(aRelease, "R");
    styleKnob(lfoRate, "LFO Hz");
    styleKnob(lfoPitch, "→ Pitch");
    styleKnob(lfoFilter, "→ Filter");
    styleKnob(glide, "Glide");
    styleKnob(masterSlider, "Master");

    styleKnob(mixerDriveSlider, "Mixer");
    for (auto* s : { &filterCutoff, &filterRes, &filterDrive, &filterEnvAmt, &filterKb, &mixerDriveSlider,
                     &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease,
                     &lfoRate, &lfoPitch, &lfoFilter, &glide, &masterSlider })
        addAndMakeVisible(*s);

    auto& apvts = processorRef.getApvts();
    masterAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::outputGainDb), masterSlider);

    const std::string_view levelIds[3] = { SynthParamIDs::osc1Level, SynthParamIDs::osc2Level, SynthParamIDs::osc3Level };
    const std::string_view fineIds[3] = { SynthParamIDs::osc1Fine, SynthParamIDs::osc2Fine, SynthParamIDs::osc3Fine };
    const std::string_view octIds[3] = { SynthParamIDs::osc1Octave, SynthParamIDs::osc2Octave, SynthParamIDs::osc3Octave };

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
    mixerDriveAttachment =
        std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::mixerDrive), mixerDriveSlider);
    subOscAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::subOscLevel), subOscSlider);
    noisePinkAttachment = std::make_unique<ButtonAttachment>(apvts, std::string(SynthParamIDs::noisePink), noisePinkButton);
    mixerOdAttachment = std::make_unique<ButtonAttachment>(apvts, std::string(SynthParamIDs::mixerOd), mixerOdButton);
    filterOdAttachment = std::make_unique<ButtonAttachment>(apvts, std::string(SynthParamIDs::filterOd), filterOdButton);
    oversampleAttachment = std::make_unique<ComboAttachment>(apvts, std::string(SynthParamIDs::oversample), oversampleBox);
    osc23SyncAttachment = std::make_unique<ButtonAttachment>(apvts, std::string(SynthParamIDs::osc23Sync), osc23SyncButton);

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

    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
    filterCutoff.setPopupDisplayEnabled(true, false, this);
    filterKb.setVisible(false);

    filterKbStepLabel.setText("KEYBOARD TRACK", juce::dontSendNotification);
    filterKbStepLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    filterKbStepLabel.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    addAndMakeVisible(filterKbStepLabel);
    const juce::StringArray kbLabels { "OFF", "1/3", "2/3", "FULL" };
    for (int i = 0; i < 4; ++i)
    {
        filterKbStepSwitch[static_cast<size_t>(i)].setButtonText(kbLabels[i]);
        filterKbStepSwitch[static_cast<size_t>(i)].setRadioGroupId(voyagerui::components::filterKbTrackRadioGroupId);
        filterKbStepSwitch[static_cast<size_t>(i)].setClickingTogglesState(true);
        filterKbStepSwitch[static_cast<size_t>(i)].setColour(juce::TextButton::buttonColourId, kPanelInset);
        filterKbStepSwitch[static_cast<size_t>(i)].onClick = [this, i]() { setFilterKbStep(i); };
        addAndMakeVisible(filterKbStepSwitch[static_cast<size_t>(i)]);
    }
    syncFilterKbStepsFromParameter();

    panelLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, kPanelMetal);
    applyLookAndFeelToControls();

    setResizeLimits(voyagerui::layout::minEditorWidthPx, voyagerui::layout::minEditorHeightPx,
                    voyagerui::layout::maxEditorWidthPx, voyagerui::layout::maxEditorHeightPx);
    setSize(voyagerui::layout::defaultEditorWidthPx, voyagerui::layout::defaultEditorHeightPx);
    startTimerHz(28);
}

void MoogVoyagerAudioProcessorEditor::applyLookAndFeelToControls()
{
    const auto setLaf = [this](juce::Slider& s) {
        s.setLookAndFeel(&panelLookAndFeel);
    };
    for (auto& s : oscLevel)
        setLaf(s);
    for (auto& s : oscFine)
        setLaf(s);
    setLaf(noiseSlider);
    setLaf(subOscSlider);
    for (auto* s : { &filterCutoff, &filterRes, &filterDrive, &filterEnvAmt, &filterKb, &mixerDriveSlider,
                     &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease, &lfoRate,
                     &lfoPitch, &lfoFilter, &glide, &masterSlider })
        setLaf(*s);
}

void MoogVoyagerAudioProcessorEditor::setFilterKbStep(int stepIndex)
{
    static const float values[4] = { 0.0f, 0.33f, 0.66f, 1.0f };
    stepIndex = juce::jlimit(0, 3, stepIndex);
    if (auto* param = processorRef.getApvts().getParameter(std::string(SynthParamIDs::filterKeyTrack)))
        param->setValueNotifyingHost(param->convertTo0to1(values[stepIndex]));
    syncFilterKbStepsFromParameter();
}

void MoogVoyagerAudioProcessorEditor::syncFilterKbStepsFromParameter()
{
    const float v = processorRef.getApvts().getRawParameterValue(std::string(SynthParamIDs::filterKeyTrack))->load();
    int best = 0;
    float bestDist = 10.0f;
    static const float values[4] = { 0.0f, 0.33f, 0.66f, 1.0f };
    for (int i = 0; i < 4; ++i)
    {
        const float d = std::abs(v - values[i]);
        if (d < bestDist)
        {
            bestDist = d;
            best = i;
        }
    }
    const auto accent = accentForCurrentStack();
    for (int i = 0; i < 4; ++i)
    {
        filterKbStepSwitch[static_cast<size_t>(i)].setColour(juce::TextButton::buttonOnColourId, accent);
        filterKbStepSwitch[static_cast<size_t>(i)].setToggleState(i == best, juce::dontSendNotification);
    }
}

void MoogVoyagerAudioProcessorEditor::timerCallback()
{
    ++ledPhase;
    meterScope.repaint();
    performanceStrip.repaint();
    repaint(0, 0, 40, getHeight());
    repaint(getWidth() - 40, 0, 40, getHeight());
}

MoogVoyagerAudioProcessorEditor::~MoogVoyagerAudioProcessorEditor()
{
    processorRef.getApvts().removeParameterListener(std::string(SynthParamIDs::moogStack), this);
    const auto clearLaf = [](juce::Slider& s) { s.setLookAndFeel(nullptr); };
    for (auto& s : oscLevel)
        clearLaf(s);
    for (auto& s : oscFine)
        clearLaf(s);
    clearLaf(noiseSlider);
    clearLaf(subOscSlider);
    for (auto* s : { &filterCutoff, &filterRes, &filterDrive, &filterEnvAmt, &filterKb, &mixerDriveSlider,
                     &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease, &lfoRate,
                     &lfoPitch, &lfoFilter, &glide, &masterSlider })
        clearLaf(*s);
}

juce::Colour MoogVoyagerAudioProcessorEditor::accentForLineage(moogstack::Lineage lineage)
{
    return voyagerui::colours::accentForLineage(lineage);
}

juce::Colour MoogVoyagerAudioProcessorEditor::accentForCurrentStack() const
{
    const int idx = static_cast<int>(
        processorRef.getApvts().getRawParameterValue(std::string(SynthParamIDs::moogStack))->load());
    return accentForLineage(moogstack::lineageFromChoiceIndex(idx));
}

void MoogVoyagerAudioProcessorEditor::selectStackMode(int index, bool applyTemplate)
{
    index = juce::jlimit(0, moogstack::lineageCount() - 1, index);
    if (auto* param = processorRef.getApvts().getParameter(std::string(SynthParamIDs::moogStack)))
    {
        const int n = moogstack::lineageCount();
        const float norm = n > 1 ? static_cast<float>(index) / static_cast<float>(n - 1) : 0.0f;
        param->setValueNotifyingHost(norm);
    }
    if (applyTemplate)
        moogstack::applyLineageTemplate(processorRef, moogstack::lineageFromChoiceIndex(index));
    refreshStackChrome();
}

void MoogVoyagerAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused(newValue);
    if (parameterID == "moogStack")
        refreshStackChrome();
}

void MoogVoyagerAudioProcessorEditor::refreshStackChrome()
{
    const int idx = static_cast<int>(
        processorRef.getApvts().getRawParameterValue(std::string(SynthParamIDs::moogStack))->load());
    const auto lineage = moogstack::lineageFromChoiceIndex(idx);
    const auto accent = accentForCurrentStack();

    stackBadgeLabel.setText(juce::String(moogstack::lineageShortLabel(lineage).data()), juce::dontSendNotification);
    stackBadgeLabel.setColour(juce::Label::textColourId, accent);

    juce::String oscTitle;
    switch (lineage)
    {
        case moogstack::Lineage::voyager:
            oscTitle = "VOYAGER OSC BANK (3)";
            break;
        case moogstack::Lineage::subPhatty:
            oscTitle = "SUB PHATTY OSC + SUB";
            break;
        case moogstack::Lineage::subsequent37:
            oscTitle = "SUBSEQUENT 37 DUAL/SUB";
            break;
        case moogstack::Lineage::modelD:
            oscTitle = "MODEL D OSCILLATORS";
            break;
    }
    sectionLabel(oscSectionLabel, oscTitle);

    for (int i = 0; i < 3; ++i)
    {
        applyAccentToSlider(oscLevel[i], accent, true);
        applyAccentToSlider(oscFine[i], accent, false);
    }
    for (auto* s : { &filterCutoff, &filterRes, &subOscSlider, &mixerDriveSlider })
        applyAccentToSlider(*s, accent, false);
    for (auto* s : { &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease })
        applyAccentToSlider(*s, accent, true);
    for (auto* s : { &lfoRate, &lfoPitch, &lfoFilter, &glide, &noiseSlider, &masterSlider })
        applyAccentToSlider(*s, accent, false);

    patchReadoutLabel.setColour(juce::Label::textColourId, accent);

    for (int i = 0; i < moogstack::lineageCount(); ++i)
    {
        const auto lin = moogstack::lineageFromChoiceIndex(i);
        auto& btn = stackModeSwitch[static_cast<size_t>(i)];
        btn.setColour(juce::TextButton::buttonOnColourId, accentForLineage(lin));
        btn.setToggleState(i == idx, juce::dontSendNotification);
    }

    for (auto* tb : { &noisePinkButton, &mixerOdButton, &filterOdButton })
        tb->setColour(juce::ToggleButton::tickColourId, accent);

    meterScope.setAccentColour(accent);
    performanceStrip.setAccentColour(accent);
    modBusMatrix.setAccentColour(accent);
  {
    panelLookAndFeel.accent = accent;
    for (auto& strip : waveSelectors)
        strip.setAccentColour(accent);
    syncFilterKbStepsFromParameter();
    for (int i = 0; i < 3; ++i)
        waveSelectors[static_cast<size_t>(i)].syncFromParameter();
  }

    const int osIdx =
        static_cast<int>(processorRef.getApvts().getRawParameterValue(std::string(SynthParamIDs::oversample))->load());
    for (int i = 0; i < 3; ++i)
    {
        oversampleSwitch[static_cast<size_t>(i)].setColour(juce::TextButton::buttonOnColourId, accent);
        oversampleSwitch[static_cast<size_t>(i)].setToggleState(i == osIdx, juce::dontSendNotification);
    }
    meterScope.repaint();

    repaint();
}

void MoogVoyagerAudioProcessorEditor::refreshPatchReadout()
{
    const int prog = processorRef.getCurrentProgram();
    const auto name = voyager::getFactoryPresetName(prog);
    const juce::String display =
        name.empty() ? juce::String("PATCH: —")
                     : juce::String("PATCH: PROG ") + juce::String(prog + 1).paddedLeft('0', 3) + " \""
                         + juce::String(name.data()) + "\"";
    patchReadoutLabel.setText(display, juce::dontSendNotification);
}

void MoogVoyagerAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto accent = accentForCurrentStack();
    const int woodW = voyagerui::layout::woodRailWidthPx;
    g.fillAll(kPanelWoodDark);
    g.setColour(kPanelWood);
    g.fillRect(0, 0, woodW, getHeight());
    g.fillRect(getWidth() - woodW, 0, woodW, getHeight());
    const int ledCount = getHeight() / 28;
    for (int side = 0; side < 2; ++side)
    {
        const int x0 = side == 0 ? 8 : getWidth() - woodW + 8;
        for (int i = 0; i < ledCount; ++i)
        {
            const float phase = static_cast<float>((i + ledPhase) % ledCount) / static_cast<float>(ledCount);
            g.setColour(accent.withAlpha(0.15f + 0.65f * (1.0f - std::abs(phase * 2.0f - 1.0f))));
            g.fillEllipse(static_cast<float>(x0), 18.0f + static_cast<float>(i * 26), 6.0f, 6.0f);
        }
    }

    juce::Rectangle<int> panel(woodW, 0, getWidth() - 2 * woodW, getHeight());
    juce::ColourGradient metalGrad(kPanelMetalHi, panel.getTopLeft().toFloat(), kPanelMetal,
                                   panel.getBottomRight().toFloat(), false);
    g.setGradientFill(metalGrad);
    g.fillRect(panel);

    g.setColour(kPanelInset);
    g.fillRect(panel.getX(), panel.getY(), panel.getWidth(), 112);
    g.setColour(accent.withAlpha(0.55f));
    g.fillRect(panel.getX(), panel.getY() + 110, panel.getWidth(), 2);

    auto bounds = getLocalBounds().reduced(woodW + 10, 12).withTrimmedTop(122);
    const int perfH = 94;
    auto upper = bounds.withTrimmedBottom(perfH);
    auto perfBox = bounds.removeFromBottom(perfH);

    auto oscBox = upper.removeFromTop(195);
    auto filterBox = upper.removeFromTop(148);
    auto envBox = upper.removeFromTop(158);

    drawSection(g, oscBox, "Oscillator 1 · 2 · 3");
    drawSection(g, filterBox, "FILTER");
    drawSection(g, envBox, "Filter & amplifier envelopes");
    drawSection(g, upper, "MODULATION BUS");
    drawSection(g, perfBox, "Performance — touch · wheels · keyboard");

    g.setColour(juce::Colours::limegreen.withAlpha(0.9f));
    g.fillEllipse(static_cast<float>(getWidth() - woodW - 108), 10.0f, 7.0f, 7.0f);
}

void MoogVoyagerAudioProcessorEditor::resized()
{
    const int woodW = voyagerui::layout::woodRailWidthPx;
    auto panel = getLocalBounds().withTrimmedLeft(woodW).withTrimmedRight(woodW);

    titleLabel.setBounds(panel.getX() + 14, 8, 280, 32);
    subtitleLabel.setBounds(panel.getX() + 14, 38, 420, 16);
    stackBadgeLabel.setBounds(panel.getX() + 14, 58, 200, 20);
    const int patchCy = 34;
    presetPrevButton.setBounds(panel.getCentreX() - 228, patchCy - 4, 28, 26);
    patchReadoutLabel.setBounds(panel.getCentreX() - 196, patchCy - 2, 392, 22);
    presetNextButton.setBounds(panel.getCentreX() + 200, patchCy - 4, 28, 26);
    oversampleLabel.setBounds(panel.getRight() - 198, 8, 18, 16);
    for (int i = 0; i < 3; ++i)
        oversampleSwitch[static_cast<size_t>(i)].setBounds(panel.getRight() - 176 + i * 38, 6, 36, 22);
    cpuStatusLabel.setBounds(panel.getRight() - 118, 8, 32, 14);
    outputSectionLabel.setBounds(panel.getRight() - 248, 26, 130, 12);
    meterScope.setBounds(panel.getRight() - 248, 40, 228, 72);
    masterSlider.setBounds(panel.getRight() - 64, 28, 56, 72);

  {
    auto stackRow = panel.reduced(14, 0).withTrimmedTop(52).removeFromTop(30);
    stackSwitchLabel.setBounds(stackRow.removeFromLeft(88).reduced(0, 4));
    const int btnW = stackRow.getWidth() / moogstack::lineageCount();
    for (int i = 0; i < moogstack::lineageCount(); ++i)
        stackModeSwitch[static_cast<size_t>(i)].setBounds(stackRow.removeFromLeft(btnW).reduced(i == 0 ? 0 : 2, 2));
  }

    auto presetRow = panel.reduced(14, 0).withTrimmedTop(86).removeFromTop(26);
    presetLabel.setBounds(presetRow.removeFromRight(100));
    presetBox.setBounds(presetRow.removeFromRight(juce::jmin(360, presetRow.getWidth())).reduced(4, 0));

    auto bounds = panel.reduced(12).withTrimmedTop(122);
    auto oscBox = bounds.removeFromTop(195);
    auto oscTitle = oscBox.removeFromTop(20);
    oscSectionLabel.setBounds(oscTitle.removeFromLeft(oscTitle.getWidth() - 120));
    osc23SyncButton.setBounds(oscTitle.reduced(2));
    auto oscInner = oscBox.reduced(10, 4);

    for (int i = 0; i < 3; ++i)
    {
        auto col = oscInner.removeFromLeft(oscInner.getWidth() / 3).reduced(4);
        oscIndexLabel[i].setBounds(col.removeFromTop(18));
        const int faderW = juce::jmin(36, col.getWidth() / 3);
        auto faderCol = col.removeFromLeft(faderW);
        oscLevel[i].setBounds(faderCol.reduced(2, 0));
        auto ctrlCol = col;
        oscFine[i].setBounds(ctrlCol.removeFromTop(juce::jmin(72, ctrlCol.getHeight() / 2)).reduced(4));
        oscOctLabel[i].setBounds(ctrlCol.removeFromTop(14));
        oscOctave[i].setBounds(ctrlCol.removeFromTop(22));
        oscWaveLabel[i].setBounds(ctrlCol.removeFromTop(14));
        waveSelectors[static_cast<size_t>(i)].setBounds(ctrlCol.removeFromTop(24));
        oscWave[i].setBounds(ctrlCol.removeFromTop(0));
    }

    auto filterBox = bounds.removeFromTop(148);
    filterSectionLabel.setBounds(filterBox.removeFromTop(20));
    auto fIn = filterBox.reduced(10, 4);
    auto fRow = fIn.removeFromTop(88);
  {
    const int bigW = juce::jmin(148, fRow.getWidth() / 3);
    filterCutoff.setBounds(fRow.removeFromLeft(bigW).reduced(2));
    const int n = 5;
    const int w = fRow.getWidth() / n;
    for (auto* k : { &filterRes, &filterDrive, &filterEnvAmt, &mixerDriveSlider, &subOscSlider })
        k->setBounds(fRow.removeFromLeft(w).reduced(6));
  }
    auto odRow = fIn.removeFromTop(26);
    mixerOdButton.setBounds(odRow.removeFromLeft(72).reduced(2));
    filterOdButton.setBounds(odRow.removeFromLeft(72).reduced(2));
    filterKbStepLabel.setBounds(odRow.removeFromLeft(88).reduced(0, 4));
    const int kbW = juce::jmin(44, odRow.getWidth() / 4);
    for (int i = 0; i < 4; ++i)
        filterKbStepSwitch[static_cast<size_t>(i)].setBounds(odRow.removeFromLeft(kbW).reduced(1, 2));

    auto envBox = bounds.removeFromTop(158);
    envSectionLabel.setBounds(envBox.removeFromTop(20));
    auto eIn = envBox.reduced(10, 4);
    filterEnvRowLabel.setBounds(eIn.removeFromTop(16));
    auto fEnvRow = eIn.removeFromTop(58);
    layoutKnobRow(fEnvRow, { &fAttack, &fDecay, &fSustain, &fRelease });
    ampEnvRowLabel.setBounds(eIn.removeFromTop(16));
    auto aEnvRow = eIn;
    layoutKnobRow(aEnvRow, { &aAttack, &aDecay, &aSustain, &aRelease });

    bounds = bounds.withTrimmedBottom(94);
    auto modBox = bounds.removeFromTop(128);
    modSectionLabel.setBounds(modBox.removeFromTop(20));
    auto mIn = modBox.reduced(10, 4);
    auto mLeft = mIn.removeFromLeft(juce::jmin(196, mIn.getWidth() / 2));
    modBusMatrix.setBounds(mLeft.removeFromTop(98));
    auto mRight = mIn;
    modLfoLabel.setBounds(mRight.removeFromTop(14));
    auto lfoRow = mRight.removeFromTop(58);
    lfoRate.setBounds(lfoRow.removeFromLeft(lfoRow.getWidth() / 3).reduced(4));
    auto wheelCols = lfoRow;
    auto pitchCol = wheelCols.removeFromLeft(wheelCols.getWidth() / 2);
    modWheelPitchLabel.setBounds(pitchCol.removeFromTop(12));
    lfoPitch.setBounds(pitchCol.reduced(2));
    modWheelFilterLabel.setBounds(wheelCols.removeFromTop(12));
    lfoFilter.setBounds(wheelCols.reduced(2));
    auto m2 = mRight;
    noisePinkButton.setBounds(m2.removeFromLeft(56).removeFromTop(22).reduced(0, 2));
    layoutKnobRow(m2, { &glide, &noiseSlider });

    auto perf = panel.reduced(12).removeFromBottom(100);
    performanceStrip.setBounds(perf.reduced(8, 6));
}
