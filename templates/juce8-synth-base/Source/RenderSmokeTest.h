#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class PlaySeriesTemplateAudioProcessor;

namespace play_series {

struct RenderSmokeResult {
  bool success = false;
  float peakDbfs = -120.0f;
  juce::String error;
};

RenderSmokeResult runRenderSmokeTest(PlaySeriesTemplateAudioProcessor &processor,
                                     const juce::File &wavOut);

} // namespace play_series