#include "PluginProcessor.h"
#include "FactoryPresets.h"
#include "PluginEditor.h"

namespace {
void setP(juce::AudioProcessorValueTreeState &a, const char *id, float v) {
  if (auto *p = a.getParameter(id))
    p->setValueNotifyingHost(p->convertTo0to1(v));
}
} // namespace

PlaySeriesTemplateAudioProcessor::PlaySeriesTemplateAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PLAYSERIES", createParameterLayout()) {
  applyFactoryPreset(0);
}

juce::AudioProcessorValueTreeState::ParameterLayout
PlaySeriesTemplateAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      play_series::kLayerBlend, "A/B Blend", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      play_series::kXyX, "XY X", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      play_series::kXyY, "XY Y", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterBool>(
      play_series::kDiceTrigger, "Dice", false));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      play_series::kMasterLevel, "Master", 0.0f, 1.0f, 0.75f));
  layout.add(std::make_unique<juce::AudioParameterBool>(
      play_series::kMacroLock, "Lock Macros", false));
  layout.add(std::make_unique<juce::AudioParameterBool>(
      play_series::kArpEnable, "Arp", false));
  layout.add(
      std::make_unique<juce::AudioParameterChoice>(play_series::kArpMode, "Arp Mode",
                                                   juce::StringArray{"Up", "Down", "UpDown", "Random"},
                                                   0));
  layout.add(
      std::make_unique<juce::AudioParameterChoice>(play_series::kArpRate, "Arp Rate",
                                                   juce::StringArray{"1/8", "1/16"}, 0));
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      play_series::kChordMode, "Chord",
      juce::StringArray{"Off", "Octave", "Minor7", "Minor9", "DarkTriad"}, 0));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      play_series::kGlideMs, "Glide", 0.0f, 500.0f, 80.0f));
  for (int i = 0; i < play_series::kMacroCount; ++i) {
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::String("macro") + juce::String(i + 1),
        juce::String("Macro ") + juce::String(i + 1), 0.0f, 1.0f, 0.5f));
  }
  return layout;
}

bool PlaySeriesTemplateAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PlaySeriesTemplateAudioProcessor::prepareToPlay(double, int) {
  scope_.reset();
}

float PlaySeriesTemplateAudioProcessor::effectiveBlend() const noexcept {
  const float xy =
      apvts_.getRawParameterValue(play_series::kXyX)->load(std::memory_order_relaxed);
  const float blend =
      apvts_.getRawParameterValue(play_series::kLayerBlend)->load(std::memory_order_relaxed);
  return juce::jlimit(0.0f, 1.0f, xy * 0.85f + blend * 0.15f);
}

void PlaySeriesTemplateAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                    juce::MidiBuffer &) {
  juce::ScopedNoDenormals nd;
  buffer.clear();
  const float gain =
      apvts_.getRawParameterValue(play_series::kMasterLevel)->load(std::memory_order_relaxed);
  juce::ignoreUnused(gain); // scaffold: engine replaces clear()+voice mix
  juce::ignoreUnused(effectiveBlend());
  float peak = 0.0f;
  for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    peak = std::max(peak, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));
  scope_.pushPeak(peak);
}

int PlaySeriesTemplateAudioProcessor::readScopePeaks(float *dest,
                                                     int maxItems) const noexcept {
  return scope_.readRecentPeaks(dest, maxItems);
}

void PlaySeriesTemplateAudioProcessor::setCurrentProgram(int index) {
  currentProgram_ =
      juce::jlimit(0, play_series::kFactoryPresetCount - 1, index);
  applyFactoryPreset(currentProgram_);
}

const juce::String PlaySeriesTemplateAudioProcessor::getProgramName(int index) {
  if (index < 0 || index >= play_series::kFactoryPresetCount)
    return {};
  return play_series::kFactoryPresets[static_cast<size_t>(index)].name;
}

void PlaySeriesTemplateAudioProcessor::applyFactoryPreset(int index) {
  const auto &p = play_series::kFactoryPresets[static_cast<size_t>(index)];
  const bool lockMacros =
      apvts_.getRawParameterValue(play_series::kMacroLock)->load(std::memory_order_relaxed) >=
      0.5f;
  float savedMacros[play_series::kMacroCount]{};
  if (lockMacros) {
    for (int i = 0; i < play_series::kMacroCount; ++i) {
      const auto id = (juce::String("macro") + juce::String(i + 1)).toRawUTF8();
      savedMacros[i] = apvts_.getRawParameterValue(id)->load(std::memory_order_relaxed);
    }
  }
  setP(apvts_, play_series::kLayerBlend, p.blend);
  for (int i = 0; i < play_series::kMacroCount; ++i) {
    const float v = lockMacros ? savedMacros[i] : p.macros[i];
    setP(apvts_, (juce::String("macro") + juce::String(i + 1)).toRawUTF8(), v);
  }
}

void PlaySeriesTemplateAudioProcessor::randomizePatchBounded() {
  setP(apvts_, play_series::kLayerBlend, rng_.nextFloat() * 0.7f + 0.15f);
  for (int i = 0; i < play_series::kMacroCount; ++i)
    setP(apvts_, (juce::String("macro") + juce::String(i + 1)).toRawUTF8(),
         rng_.nextFloat() * 0.7f + 0.15f);
  // master_level intentionally untouched
}

void PlaySeriesTemplateAudioProcessor::getStateInformation(juce::MemoryBlock &dest) {
  auto state = apvts_.copyState();
  if (auto xml = state.createXml()) {
    xml->setAttribute("program", currentProgram_);
    copyXmlToBinary(*xml, dest);
  }
}

void PlaySeriesTemplateAudioProcessor::setStateInformation(const void *data, int size) {
  if (auto xml = getXmlFromBinary(data, size))
    if (xml->hasTagName(apvts_.state.getType())) {
      apvts_.replaceState(juce::ValueTree::fromXml(*xml));
      currentProgram_ = xml->getIntAttribute("program", 0);
    }
}

juce::AudioProcessorEditor *PlaySeriesTemplateAudioProcessor::createEditor() {
  return new PlaySeriesTemplateAudioProcessorEditor(*this);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new PlaySeriesTemplateAudioProcessor();
}