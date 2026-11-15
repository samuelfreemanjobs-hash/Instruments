#include "PluginEditor.h"

PlaySeriesTemplateAudioProcessorEditor::PlaySeriesTemplateAudioProcessorEditor(
    PlaySeriesTemplateAudioProcessor &proc)
    : AudioProcessorEditor(proc), processor_(proc) {
  addAndMakeVisible(diceButton_);
  diceButton_.onClick = [this] { processor_.randomizePatchBounded(); };

#if JUCE_WEB_BROWSER
  web_ = std::make_unique<juce::WebBrowserComponent>();
  addAndMakeVisible(web_.get());
  web_->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#endif

  setSize(900, 620);
}

void PlaySeriesTemplateAudioProcessorEditor::resized() {
  auto r = getLocalBounds().reduced(8);
  diceButton_.setBounds(r.removeFromTop(36).removeFromLeft(100));
  if (web_ != nullptr)
    web_->setBounds(r);
}