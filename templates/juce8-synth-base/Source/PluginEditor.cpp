#include "PluginEditor.h"

#if defined(PLAY_SYNTH_HAS_BINARY_ASSETS)
 #include "BinaryData.h"
#endif

PlaySeriesTemplateAudioProcessorEditor::PlaySeriesTemplateAudioProcessorEditor(
    PlaySeriesTemplateAudioProcessor &proc)
    : AudioProcessorEditor(proc), processor_(proc) {
  addAndMakeVisible(diceButton_);
  diceButton_.onClick = [this] { processor_.randomizePatchBounded(); };

  addAndMakeVisible(macroLock_);
  macroLock_.setClickingTogglesState(true);
  macroLockAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      processor_.getAPVTS(), play_series::kMacroLock, macroLock_);

  addAndMakeVisible(midiExport_);
  midiExport_.onClick = [this] { processor_.exportMidiPattern(); };

  for (int i = 0; i < play_series::kMacroCount; ++i) {
    auto slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag,
                                                 juce::Slider::TextBoxBelow);
    slider->setRange(0.0, 1.0, 0.001);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider->setName("macro" + juce::String(i + 1));
    slider->setDoubleClickReturnValue(true, 0.5);
    addAndMakeVisible(slider.get());
    macroSliders_[static_cast<size_t>(i)] = std::move(slider);
    macroAttachments_[static_cast<size_t>(i)].reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            processor_.getAPVTS(), "macro" + juce::String(i + 1),
            *macroSliders_[static_cast<size_t>(i)]));
  }

  uiScaleBox_.addItemList(juce::StringArray{"75%", "100%", "125%", "150%"}, 1);
  addAndMakeVisible(uiScaleBox_);
  uiScaleAttachment_.reset(
      new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
          processor_.getAPVTS(), play_series::kUiScale, uiScaleBox_));
  uiScaleBox_.onChange = [this] { applyUiScale(); };

#if JUCE_WEB_BROWSER && !defined(PLAY_SYNTH_HEADLESS_CI)
  web_ = std::make_unique<juce::WebBrowserComponent>();
  addAndMakeVisible(web_.get());
  loadWebHtml();
#endif

  applyUiScale();
  startTimerHz(30);
}

PlaySeriesTemplateAudioProcessorEditor::~PlaySeriesTemplateAudioProcessorEditor() = default;

void PlaySeriesTemplateAudioProcessorEditor::loadWebHtml() {
#if JUCE_WEB_BROWSER && !defined(PLAY_SYNTH_HEADLESS_CI)
 #if defined(PLAY_SYNTH_HAS_BINARY_ASSETS)
  const auto html = juce::String::createStringFromData(BinaryData::index_html, BinaryData::index_htmlSize);
  const auto css = juce::String::createStringFromData(BinaryData::style_css, BinaryData::style_cssSize);
  const auto js = juce::String::createStringFromData(BinaryData::app_js, BinaryData::app_jsSize);
  const auto injected = html.replace("</head>", "<style>" + css + "</style></head>")
                            .replace("</body>", "<script>" + js + "</script></body>");
  web_->goToURL("data:text/html;charset=utf-8," + juce::URL::addEscapeChars(injected, true));
 #else
  web_->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
 #endif
#endif
}

void PlaySeriesTemplateAudioProcessorEditor::applyUiScale() {
  const int idx = uiScaleBox_.getSelectedItemIndex();
  const float scales[] = {0.75f, 1.0f, 1.25f, 1.5f};
  const float s = scales[juce::jlimit(0, 3, idx)];
  setSize(static_cast<int>(920 * s), static_cast<int>(640 * s));
}

void PlaySeriesTemplateAudioProcessorEditor::paint(juce::Graphics &g) {
  auto scope = getLocalBounds();
  scope = scope.removeFromTop(48).reduced(12, 4);
  g.setColour(juce::Colours::black.withAlpha(0.55f));
  g.fillRoundedRectangle(scope.toFloat(), 6.0f);
  float peaks[256];
  const int n = processor_.readScopePeaks(peaks, 256);
  if (n < 2)
    return;
  juce::Path wave;
  for (int i = 0; i < n; ++i) {
    const float x = scope.getX() + (static_cast<float>(i) / static_cast<float>(n - 1)) * scope.getWidth();
    const float y = scope.getCentreY() - peaks[i] * scope.getHeight() * 0.45f;
    if (i == 0)
      wave.startNewSubPath(x, y);
    else
      wave.lineTo(x, y);
  }
  g.setColour(juce::Colour(0xff38e8ff));
  g.strokePath(wave, juce::PathStrokeType(1.5f));
}

void PlaySeriesTemplateAudioProcessorEditor::timerCallback() {
  repaint();
}

void PlaySeriesTemplateAudioProcessorEditor::resized() {
  auto r = getLocalBounds().reduced(8);
  auto top = r.removeFromTop(36);
  diceButton_.setBounds(top.removeFromLeft(72));
  macroLock_.setBounds(top.removeFromLeft(120));
  midiExport_.setBounds(top.removeFromLeft(110));
  uiScaleBox_.setBounds(top.removeFromLeft(90));
#if JUCE_WEB_BROWSER
  if (web_ != nullptr)
    web_->setBounds(r);
#endif
}