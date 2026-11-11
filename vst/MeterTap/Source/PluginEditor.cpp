#include "PluginEditor.h"

namespace {
constexpr int kWinW = 480;
constexpr int kWinH = 440;

juce::String makeTooltipTitleBody(const juce::String& title, const juce::String& body)
{
  return title + juce::String(juce::NewLine()) + body;
}
} // namespace

juce::String MeterTapAudioProcessorEditor::middleDotUtf8()
{
  // U+00B7 MIDDLE DOT — never embed raw non-ASCII in source literals.
  return juce::String(juce::CharPointer_UTF8("\xC2\xB7"));
}

juce::String MeterTapAudioProcessorEditor::metersTitleUtf8(bool bypassed)
{
  return juce::String(juce::CharPointer_UTF8("METERS ")) + middleDotUtf8()
      + juce::String(juce::CharPointer_UTF8(bypassed ? " INPUT" : " OUTPUT"));
}

MeterTapAudioProcessorEditor::MeterTapAudioProcessorEditor(MeterTapAudioProcessor& p)
    : AudioProcessorEditor(p),
      proc_(p),
      header_("MeterTap", "1.0.0")
{
  setLookAndFeel(&lnf_);
  tooltipWindow_ = std::make_unique<juce::TooltipWindow>(this, fds::motion::tooltipDelayMs);

  addAndMakeVisible(header_);
  setupHeaderGlyph();
  header_.getPresetBrowser().setPresetList({"Init"});
  header_.getPresetBrowser().setPresetName("Init");

  meterSection_.setTitle(metersTitleUtf8(false));
  inputSection_.setTitle("INPUT");
  readoutSection_.setTitle("READOUT");
  addAndMakeVisible(meterSection_);
  addAndMakeVisible(inputSection_);
  addAndMakeVisible(readoutSection_);
  bypassCautionDot_.setVisible(false);
  bypassCautionDot_.setInterceptsMouseClicks(false, false);
  addAndMakeVisible(bypassCautionDot_);

  // Meters sit above the section so absolute design-spec rects apply.
  addAndMakeVisible(meterL_);
  addAndMakeVisible(meterR_);
  addAndMakeVisible(meterScale_);

  auto styleLegend = [](juce::Label& lab, const juce::String& text) {
    lab.setText(fds::gui::applyCase(text, fds::typography::style::legend.textCase),
                juce::dontSendNotification);
    lab.setFont(fds::gui::makeFont(fds::typography::style::legend));
    lab.setColour(juce::Label::textColourId, fds::gui::argb(fds::color::text::primary));
    lab.setJustificationType(juce::Justification::centred);
    lab.setInterceptsMouseClicks(false, false);
  };
  styleLegend(legendL_, "L");
  styleLegend(legendR_, "R");
  addAndMakeVisible(legendL_);
  addAndMakeVisible(legendR_);

  meterL_.setMouseCursor(juce::MouseCursor::PointingHandCursor);
  meterR_.setMouseCursor(juce::MouseCursor::PointingHandCursor);
  meterL_.setWantsKeyboardFocus(false);
  meterR_.setWantsKeyboardFocus(false);
  auto clearCb = [this] { clearUiHoldAndOver(); };
  meterL_.onClear = clearCb;
  meterR_.onClear = clearCb;

  trimKnob_.setBipolar(true, 24.0f / 36.0f); // 0 dB in −24…+12
  trimKnob_.setDefaultNormalised(24.0f / 36.0f);
  trimKnob_.valueFormatter = [](float norm) {
    const float db = -24.0f + norm * 36.0f;
    return formatDbNoUnit(db) + " dB";
  };
  // Manual APVTS sync — SliderAttachment would overwrite RotaryKnob's 0..1 range.
  trimKnob_.getSlider().onDragStart = [this] {
    if (auto* param = proc_.getAPVTS().getParameter(MeterTapAudioProcessor::kParamTrimDb))
      param->beginChangeGesture();
  };
  trimKnob_.getSlider().onDragEnd = [this] {
    if (auto* param = proc_.getAPVTS().getParameter(MeterTapAudioProcessor::kParamTrimDb))
      param->endChangeGesture();
  };
  trimKnob_.onValueChange = [this](float norm) {
    if (auto* param = proc_.getAPVTS().getParameter(MeterTapAudioProcessor::kParamTrimDb))
      param->setValueNotifyingHost(juce::jlimit(0.0f, 1.0f, norm));
  };
  trimKnob_.getSlider().setPopupDisplayEnabled(false, false, nullptr);
  trimKnob_.getSlider().setScrollWheelEnabled(true);
  addAndMakeVisible(trimKnob_);

  auto styleMicro = [](juce::Label& lab, const juce::String& text) {
    lab.setText(fds::gui::applyCase(text, fds::typography::style::micro.textCase),
                juce::dontSendNotification);
    lab.setFont(fds::gui::makeFont(fds::typography::style::micro));
    lab.setColour(juce::Label::textColourId, fds::gui::argb(fds::color::text::tertiary));
    lab.setJustificationType(juce::Justification::centred);
    lab.setInterceptsMouseClicks(false, false);
  };
  styleMicro(peakHeader_, "PEAK dBFS");
  styleMicro(rmsHeader_, "RMS dBFS");
  styleLegend(rowLegendL_, "L");
  styleLegend(rowLegendR_, "R");
  addAndMakeVisible(peakHeader_);
  addAndMakeVisible(rmsHeader_);
  addAndMakeVisible(rowLegendL_);
  addAndMakeVisible(rowLegendR_);

  for (auto* r : {&peakL_, &peakR_, &rmsL_, &rmsR_}) {
    r->setLarge(true);
    addAndMakeVisible(*r);
  }
  seedSilenceReadouts();

  addAndMakeVisible(holdToggle_);
  clearButton_.getProperties().set(fds::gui::FactoryLookAndFeel::buttonVariantProperty, "secondary");
  clearButton_.onClick = [this] { clearUiHoldAndOver(); };
  addAndMakeVisible(clearButton_);

  // ButtonAttachment owns onClick — do not overwrite (title/hold synced in timer).
  bypassAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      proc_.getAPVTS(), MeterTapAudioProcessor::kParamBypass, header_.getBypassToggle().getButton());
  holdAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      proc_.getAPVTS(), MeterTapAudioProcessor::kParamPeakHold, holdToggle_.getButton());

  // Seed UI from APVTS
  {
    const float trimNorm =
        proc_.getAPVTS().getParameter(MeterTapAudioProcessor::kParamTrimDb)->getValue();
    trimKnob_.setNormalisedValue(trimNorm, juce::dontSendNotification);
    syncTrimValueLabel();
  }
  {
    const bool holdOn =
        proc_.getAPVTS().getRawParameterValue(MeterTapAudioProcessor::kParamPeakHold)->load() >= 0.5f;
    for (auto& b : ballistics_)
      b.setPeakHoldEnabled(holdOn);
  }
  {
    const bool bypassed =
        proc_.getAPVTS().getRawParameterValue(MeterTapAudioProcessor::kParamBypass)->load() >= 0.5f;
    applyMeterTitle(bypassed);
  }

  setupTooltips();
  setupTabOrder();

  setSize(kWinW, kWinH);
  setResizable(false, false);
  setWantsKeyboardFocus(true);

  lastTimerMs_ = juce::Time::getMillisecondCounterHiRes();
  startTimerHz(fds::motion::meterRefreshHz);
}

MeterTapAudioProcessorEditor::~MeterTapAudioProcessorEditor()
{
  stopTimer();
  setLookAndFeel(nullptr);
}

void MeterTapAudioProcessorEditor::setupHeaderGlyph()
{
  // Simplified floppy + crown from design-system previews/disklordz.html (22×22 viewBox).
  juce::Path outline, fill, accent;
  outline.addRoundedRectangle(2.0f, 5.0f, 18.0f, 15.0f, 2.0f);
  fill.addRectangle(6.0f, 5.0f, 9.0f, 5.0f);
  accent.startNewSubPath(5.0f, 4.0f);
  accent.lineTo(8.0f, 1.0f);
  accent.lineTo(11.0f, 4.0f);
  accent.lineTo(14.0f, 1.0f);
  accent.lineTo(17.0f, 4.0f);
  header_.setGlyphPaths(std::move(outline), std::move(fill), std::move(accent));
}

void MeterTapAudioProcessorEditor::setupTooltips()
{
  // U+2212 minus, U+2026 ellipsis, U+00B7 middle dot — all via UTF-8 pointers.
  const auto trimRangeHint = juce::String(juce::CharPointer_UTF8("\xe2\x88\x92")) + "24"
      + juce::String(juce::CharPointer_UTF8("\xe2\x80\xa6")) + "+12 dB " + middleDotUtf8()
      + " default 0 dB " + middleDotUtf8() + " double-click to reset";
  trimKnob_.getSlider().setTooltip(makeTooltipTitleBody(
      "Input trim",
      juce::String("Gain applied before metering and output.") + juce::String(juce::NewLine())
          + trimRangeHint));
  header_.getBypassToggle().getButton().setTooltip(makeTooltipTitleBody(
      "Bypass", "Passes audio through untouched. Meters keep showing the input."));
  holdToggle_.getButton().setTooltip(makeTooltipTitleBody(
      "Peak hold", "Holds the highest peak for 1.5 s, then releases."));
  clearButton_.setTooltip(makeTooltipTitleBody("Clear", "Resets peak hold and over indicators."));
  meterL_.setTooltip(makeTooltipTitleBody(
      "Level", "Peak (narrow bar) and RMS (wide bar) in dBFS. Click to clear."));
  meterR_.setTooltip(meterL_.getTooltip());
  peakL_.setTooltip(makeTooltipTitleBody("Peak", "Highest recent sample level per channel."));
  peakR_.setTooltip(peakL_.getTooltip());
  rmsL_.setTooltip(makeTooltipTitleBody("RMS", "Average level over 300 ms."));
  rmsR_.setTooltip(rmsL_.getTooltip());
  header_.getPresetBrowser().getNameButton().setTooltip("Preset");
}

void MeterTapAudioProcessorEditor::setupTabOrder()
{
  // Tab: preset ‹ → name → › → BYPASS → Trim → HOLD → CLEAR
  auto& presets = header_.getPresetBrowser();
  presets.getPrevButton().setExplicitFocusOrder(1);
  presets.getNameButton().setExplicitFocusOrder(2);
  presets.getNextButton().setExplicitFocusOrder(3);
  header_.getBypassToggle().getButton().setExplicitFocusOrder(4);
  trimKnob_.getSlider().setExplicitFocusOrder(5);
  holdToggle_.getButton().setExplicitFocusOrder(6);
  clearButton_.setExplicitFocusOrder(7);
}

void MeterTapAudioProcessorEditor::applyMeterTitle(bool bypassed)
{
  showBypassDot_ = bypassed;
  meterSection_.setTitle(metersTitleUtf8(bypassed));
  // design-spec §4: 6 px caution dot, then 6 px gap, then title (shifts right).
  meterSection_.setTitleExtraLeft(bypassed ? 12 : 0);
  bypassCautionDot_.setVisible(bypassed);
  if (bypassed) {
    trimKnob_.getSlider().setTooltip(makeTooltipTitleBody(
        "Input trim",
        juce::String("Gain applied before metering and output.") + juce::String(juce::NewLine())
            + "No effect while bypassed."));
  } else {
    const auto trimRangeHint = juce::String(juce::CharPointer_UTF8("\xe2\x88\x92")) + "24"
        + juce::String(juce::CharPointer_UTF8("\xe2\x80\xa6")) + "+12 dB " + middleDotUtf8()
        + " default 0 dB " + middleDotUtf8() + " double-click to reset";
    trimKnob_.getSlider().setTooltip(makeTooltipTitleBody(
        "Input trim",
        juce::String("Gain applied before metering and output.") + juce::String(juce::NewLine())
            + trimRangeHint));
  }
  meterSection_.repaint();
}

void MeterTapAudioProcessorEditor::seedSilenceReadouts()
{
  const float floorDb = fds::meter::numericFloorDb;
  lastPeakLText_.clear();
  lastPeakRText_.clear();
  lastRmsLText_.clear();
  lastRmsRText_.clear();
  updateReadout(peakL_, floorDb, false, lastPeakLText_);
  updateReadout(peakR_, floorDb, false, lastPeakRText_);
  updateReadout(rmsL_, floorDb, false, lastRmsLText_);
  updateReadout(rmsR_, floorDb, false, lastRmsRText_);
}

void MeterTapAudioProcessorEditor::clearUiHoldAndOver()
{
  for (auto& b : ballistics_)
    b.reset();
  overLatched_ = {{false, false}};
  const bool holdOn = holdToggle_.getToggleState();
  for (auto& b : ballistics_)
    b.setPeakHoldEnabled(holdOn);

  const float floorDb = fds::meter::defaultFloorDb;
  meterL_.setLevels(floorDb, floorDb, floorDb, false);
  meterR_.setLevels(floorDb, floorDb, floorDb, false);
  updateReadout(peakL_, floorDb, false, lastPeakLText_);
  updateReadout(peakR_, floorDb, false, lastPeakRText_);
  updateReadout(rmsL_, floorDb, false, lastRmsLText_);
  updateReadout(rmsR_, floorDb, false, lastRmsRText_);
}

juce::String MeterTapAudioProcessorEditor::formatDbNoUnit(float db)
{
  // U+2212 MINUS SIGN, U+221E INFINITY — design-spec §4 / §2.3 (+ always for >= 0).
  if (db <= fds::meter::numericFloorDb || !std::isfinite(db))
    return juce::String(juce::CharPointer_UTF8("\xe2\x88\x92\xe2\x88\x9e"));
  if (db < 0.0f)
    return juce::String(juce::CharPointer_UTF8("\xe2\x88\x92")) + juce::String(std::abs(db), 1);
  return "+" + juce::String(db, 1);
}

void MeterTapAudioProcessorEditor::updateReadout(fds::gui::ValueReadout& readout, float db,
                                                 bool overLatched, juce::String& lastText)
{
  const auto text = formatDbNoUnit(db);
  if (overLatched || db > fds::meter::overThresholdDb)
    readout.setState(fds::gui::ValueReadout::State::over);
  else if (db <= fds::meter::numericFloorDb)
    readout.setState(fds::gui::ValueReadout::State::floor);
  else
    readout.setState(fds::gui::ValueReadout::State::normal);

  // Always push text so wells are never blank (repaint-on-change is ValueReadout's job).
  if (text != lastText) {
    lastText = text;
    readout.setValueText(text, {});
  } else if (readout.getValueText().isEmpty()) {
    readout.setValueText(text, {});
  }
}

void MeterTapAudioProcessorEditor::timerCallback()
{
  const auto now = juce::Time::getMillisecondCounterHiRes();
  double dt = (now - lastTimerMs_) * 0.001;
  lastTimerMs_ = now;
  if (!(dt > 0.0) || dt > 0.25)
    dt = 1.0 / static_cast<double>(fds::motion::meterRefreshHz);

  const bool holdOn = holdToggle_.getToggleState();
  for (auto& b : ballistics_)
    b.setPeakHoldEnabled(holdOn);

  auto& bridge = proc_.meterBridge();
  for (int ch = 0; ch < 2; ++ch) {
    const float peakLin = bridge.consumePeak(ch);
    const float rmsLin = bridge.loadRms(ch);
    ballistics_[static_cast<size_t>(ch)].process(peakLin, rmsLin, dt);
  }

  const float floorDb = fds::meter::defaultFloorDb;
  for (int ch = 0; ch < 2; ++ch) {
    auto& b = ballistics_[static_cast<size_t>(ch)];
    const float peakDb = b.peakDb();
    const float rmsDb = b.rmsDb();
    if (peakDb > fds::meter::overThresholdDb)
      overLatched_[static_cast<size_t>(ch)] = true;

    const float holdDb = holdOn ? peakDb : floorDb;
    auto& meter = (ch == 0) ? meterL_ : meterR_;
    meter.setLevels(peakDb, rmsDb, holdDb, overLatched_[static_cast<size_t>(ch)]);
  }

  // PEAK readouts follow held peak when hold on (ballistics already fold hold into peakDb)
  updateReadout(peakL_, ballistics_[0].peakDb(), overLatched_[0], lastPeakLText_);
  updateReadout(peakR_, ballistics_[1].peakDb(), overLatched_[1], lastPeakRText_);
  updateReadout(rmsL_, ballistics_[0].rmsDb(), false, lastRmsLText_);
  updateReadout(rmsR_, ballistics_[1].rmsDb(), false, lastRmsRText_);

  // Host automation → knob (norm 0..1 matches APVTS float encoding)
  if (auto* trimParam = proc_.getAPVTS().getParameter(MeterTapAudioProcessor::kParamTrimDb)) {
    const float hostNorm = trimParam->getValue();
    if (std::abs(hostNorm - trimKnob_.getNormalisedValue()) > 1.0e-5f)
      trimKnob_.setNormalisedValue(hostNorm, juce::dontSendNotification);
  }
  syncTrimValueLabel();

  // Bypass title follows parameter (automation / host)
  const bool bypassed =
      proc_.getAPVTS().getRawParameterValue(MeterTapAudioProcessor::kParamBypass)->load() >= 0.5f;
  if (bypassed != showBypassDot_)
    applyMeterTitle(bypassed);
}

void MeterTapAudioProcessorEditor::syncTrimValueLabel()
{
  const float db =
      proc_.getAPVTS().getRawParameterValue(MeterTapAudioProcessor::kParamTrimDb)->load();
  trimKnob_.setValueText(formatDbNoUnit(db) + " dB");
}

void MeterTapAudioProcessorEditor::paint(juce::Graphics& g)
{
  g.fillAll(fds::gui::argb(fds::color::bg::chassis));
}

juce::String MeterTapAudioProcessorEditor::getTrimValueText()
{
  // RotaryKnob value label is private; rebuild from APVTS (same formatter as sync).
  const float db =
      proc_.getAPVTS().getRawParameterValue(MeterTapAudioProcessor::kParamTrimDb)->load();
  return formatDbNoUnit(db) + " dB";
}

juce::String MeterTapAudioProcessorEditor::getPresetNameText()
{
  return header_.getPresetBrowser().getPresetName();
}

void MeterTapAudioProcessorEditor::resized()
{
  // Absolute layout per design-spec.md §2
  header_.setBounds(0, 0, 480, 40);
  meterSection_.setBounds(16, 56, 216, 368);
  inputSection_.setBounds(248, 56, 216, 156);
  readoutSection_.setBounds(248, 224, 216, 200);
  // 6 px caution dot at section title inset, then 6 px gap before title text (§4).
  // Section origin (16,56); title row y≈64; default title left = 16+12 = 28.
  bypassCautionDot_.setBounds(28, 66, 6, 6);
  bypassCautionDot_.toFront(false);

  // LevelMeter includes over cap + bar. Spec: over at y92 h6, bar y100 h240.
  // LevelMeter puts cap+2 at top then bar; total height = 8 + 240 = 248.
  meterL_.setBounds(64, 92, 36, 248);
  meterR_.setBounds(148, 92, 36, 248);
  meterScale_.setBounds(106, 100, 36, 240);
  legendL_.setBounds(64, 348, 36, 14);
  legendR_.setBounds(148, 348, 36, 14);

  // RotaryKnob packs legend+body+value; place to cover trim rects.
  // Spec: legend y92, knob ring 319,110,74,74, value y188 — use a stacked box.
  trimKnob_.setBounds(319, 84, 74, 120);

  peakHeader_.setBounds(280, 260, 80, 12);
  rmsHeader_.setBounds(368, 260, 80, 12);
  rowLegendL_.setBounds(260, 283, 12, 14);
  rowLegendR_.setBounds(260, 319, 12, 14);
  peakL_.setBounds(280, 276, 80, 28);
  rmsL_.setBounds(368, 276, 80, 28);
  peakR_.setBounds(280, 312, 80, 28);
  rmsR_.setBounds(368, 312, 80, 28);
  holdToggle_.setBounds(260, 360, 88, 24);
  clearButton_.setBounds(356, 360, 96, 24);
}

void MeterTapAudioProcessorEditor::visibilityChanged()
{
  if (isVisible() && !isTimerRunning())
    startTimerHz(fds::motion::meterRefreshHz);
}

void MeterTapAudioProcessorEditor::setTestBypass(bool on)
{
  if (auto* param = proc_.getAPVTS().getParameter(MeterTapAudioProcessor::kParamBypass)) {
    param->beginChangeGesture();
    param->setValueNotifyingHost(on ? 1.0f : 0.0f);
    param->endChangeGesture();
  }
  header_.getBypassToggle().setToggleState(on, juce::dontSendNotification);
  applyMeterTitle(on);
}

void MeterTapAudioProcessorEditor::setTestOverLatched(bool left, bool right)
{
  overLatched_ = {{left, right}};
  const float floorDb = fds::meter::defaultFloorDb;
  constexpr float kRmsDb = -10.0f; // must render as full "−10.0" (no ellipsis) in 80 px wells
  meterL_.setLevels(left ? 1.0f : -12.0f, kRmsDb, left ? 1.0f : floorDb, left);
  meterR_.setLevels(right ? 1.0f : -12.0f, kRmsDb, right ? 1.0f : floorDb, right);
  updateReadout(peakL_, left ? 1.0f : -12.0f, left, lastPeakLText_);
  updateReadout(peakR_, right ? 1.0f : -12.0f, right, lastPeakRText_);
  updateReadout(rmsL_, kRmsDb, false, lastRmsLText_);
  updateReadout(rmsR_, kRmsDb, false, lastRmsRText_);
}

void MeterTapAudioProcessorEditor::setTestLevels(int channel, float peakDb, float rmsDb,
                                                 float holdDb, bool over)
{
  if (channel == 0) {
    overLatched_[0] = over;
    meterL_.setLevels(peakDb, rmsDb, holdDb, over);
    updateReadout(peakL_, peakDb, over, lastPeakLText_);
    updateReadout(rmsL_, rmsDb, false, lastRmsLText_);
  } else if (channel == 1) {
    overLatched_[1] = over;
    meterR_.setLevels(peakDb, rmsDb, holdDb, over);
    updateReadout(peakR_, peakDb, over, lastPeakRText_);
    updateReadout(rmsR_, rmsDb, false, lastRmsRText_);
  }
}

void MeterTapAudioProcessorEditor::forcePaintForTest()
{
  resized();
  repaint();
}
