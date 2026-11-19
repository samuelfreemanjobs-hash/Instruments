#include "PluginEditor.h"

namespace {
constexpr juce::Colour kBg{0xff121212};
constexpr juce::Colour kAccent{0xffff5500};
} // namespace

XDPatchArchitectAudioProcessorEditor::XDPatchArchitectAudioProcessorEditor(XDPatchArchitectAudioProcessor &p)
    : AudioProcessorEditor(&p), proc_(p) {
  setSize(420, 280);
  addAndMakeVisible(mutateSlider_);
  mutateSlider_.setRange(0.05, 0.5, 0.01);
  mutateSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
  mutateSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
  wireSlider(mutateSlider_, "mutate_amount");

  sendButton_.onClick = [this]() {
    if (auto *param = proc_.getAPVTS().getParameter("send_patch")) {
      param->setValueNotifyingHost(1.f);
    }
  };
  addAndMakeVisible(sendButton_);

  bassButton_.onClick = [this]() { applyProfile(0.35f, 0.4f, 0.08f, 0.35f); };
  padButton_.onClick = [this]() { applyProfile(0.55f, 0.3f, 0.55f, 0.7f); };
  leadButton_.onClick = [this]() { applyProfile(0.72f, 0.55f, 0.05f, 0.25f); };
  for (auto *b : {&bassButton_, &padButton_, &leadButton_}) {
    b->setColour(juce::TextButton::buttonColourId, kAccent);
    addAndMakeVisible(*b);
  }
}

void XDPatchArchitectAudioProcessorEditor::wireSlider(juce::Slider &slider, const juce::String &paramId) {
  slider.onValueChange = [this, &slider, paramId]() {
    if (auto *param = proc_.getAPVTS().getParameter(paramId)) {
      param->setValueNotifyingHost(static_cast<float>(proc_.getAPVTS().getParameterRange(paramId).convertTo0to1(
          static_cast<float>(slider.getValue()))));
    }
  };
}

void XDPatchArchitectAudioProcessorEditor::applyProfile(float cutoff, float reso, float attack, float release) {
  auto &apvts = proc_.getAPVTS();
  auto set = [&](const char *id, float v) {
    if (auto *p = apvts.getParameter(id)) {
      p->setValueNotifyingHost(v);
    }
  };
  set("filter_cutoff", cutoff);
  set("filter_resonance", reso);
  set("amp_attack", attack);
  set("amp_release", release);
}

void XDPatchArchitectAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(kBg);
  g.setColour(kAccent);
  g.setFont(20.f);
  g.drawText("XD Patch Architect", getLocalBounds().removeFromTop(36), juce::Justification::centred);
}

void XDPatchArchitectAudioProcessorEditor::resized() {
  auto area = getLocalBounds().reduced(16);
  area.removeFromTop(40);
  auto row = area.removeFromTop(32);
  bassButton_.setBounds(row.removeFromLeft(90).reduced(2));
  padButton_.setBounds(row.removeFromLeft(90).reduced(2));
  leadButton_.setBounds(row.removeFromLeft(90).reduced(2));
  area.removeFromTop(8);
  mutateSlider_.setBounds(area.removeFromTop(48));
  sendButton_.setBounds(area.removeFromTop(40).reduced(0, 4));
}
