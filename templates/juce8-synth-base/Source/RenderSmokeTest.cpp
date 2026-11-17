#include "RenderSmokeTest.h"

#include "PluginProcessor.h"

namespace play_series {

RenderSmokeResult runRenderSmokeTest(PlaySeriesTemplateAudioProcessor &processor,
                                     const juce::File &wavOut) {
  RenderSmokeResult result;
  constexpr double sr = 48000.0;
  constexpr int totalSamples = static_cast<int>(sr * 2.0);
  constexpr int block = 512;

  processor.prepareToPlay(sr, block);

  juce::AudioBuffer<float> buffer(2, block);
  float peak = 0.0f;
  juce::AudioBuffer<float> capture(1, totalSamples);
  capture.clear();

  int written = 0;
  for (int offset = 0; offset < totalSamples; offset += block) {
    buffer.clear();
    juce::MidiBuffer midi;
    if (offset == 0) {
      const int notes[] = {60, 63, 67, 70, 74};
      for (int n : notes)
        midi.addEvent(juce::MidiMessage::noteOn(1, n, (juce::uint8)110), 0);
    }
  if (offset == static_cast<int>(sr * 1.5))
      for (int n : {60, 63, 67, 70, 74})
        midi.addEvent(juce::MidiMessage::noteOff(1, n), 0);

    processor.processBlock(buffer, midi);
    const int copyN = std::min(block, totalSamples - offset);
    capture.copyFrom(0, offset, buffer, 0, 0, copyN);
    if (buffer.getNumChannels() > 1)
      capture.addFrom(0, offset, buffer, 1, 0, copyN);
    peak = std::max(peak, buffer.getMagnitude(0, 0, copyN));
    written += copyN;
  }

  result.peakDbfs = peak > 1.0e-8f ? 20.0f * std::log10(peak) : -120.0f;
  if (std::isnan(result.peakDbfs) || std::isinf(result.peakDbfs))
    result.peakDbfs = 120.0f;

  if (result.peakDbfs < -24.0f) {
    result.success = false;
    result.error = "Render too quiet (" + juce::String(result.peakDbfs, 2) + " dBFS)";
    return result;
  }
  if (result.peakDbfs > -0.05f) {
    result.success = false;
    result.error = "Render clipped/hot (" + juce::String(result.peakDbfs, 2) + " dBFS)";
    return result;
  }

  std::unique_ptr<juce::FileOutputStream> stream(wavOut.createOutputStream());
  if (stream == nullptr) {
    result.success = false;
    result.error = "Could not open " + wavOut.getFullPathName();
    return result;
  }

  juce::WavAudioFormat format;
  std::unique_ptr<juce::AudioFormatWriter> writer(
      format.createWriterFor(stream.release(), sr, 1, 16, {}, 0));
  if (writer == nullptr) {
    result.success = false;
    result.error = "WAV writer failed";
    return result;
  }
  writer->writeFromAudioSampleBuffer(capture, 0, written);
  writer->flush();
  result.success = true;
  return result;
}

} // namespace play_series