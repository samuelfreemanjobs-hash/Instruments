#pragma once

#include <juce_core/juce_core.h>
#include <vector>
#include "DrumPattern.h"
#include "PackExportOptions.h"

/** Procedural Memphis phonk drum kit + loop generator. */
class MemphisPhonkEngine {
public:
  struct Settings {
    float drive = 2.5f;
    float hissLevel = 0.004f;
    int bitDepth = 12;
    int64_t seed = 0;
    float dustMacro = 0.35f;
    float wowDepth = 18.0f;
    bool tightenToLoop = false;
    float tightenBpm = 118.0f;
  };

  struct DrumLoopStems {
    std::vector<float> kick;
    std::vector<float> snare;
    std::vector<float> hats;
    std::vector<float> mix;
  };

  explicit MemphisPhonkEngine(double sampleRate = 44100.0);

  static Settings settingsFromDust(float dustMacro, int variationSeed);
  static void applyDustMacro(Settings &s, float dust);

  void setSampleRate(double sampleRate) { sampleRate_ = sampleRate; }
  double getSampleRate() const { return sampleRate_; }

  void setSettings(const Settings &s);
  Settings getSettings() const { return settings_; }

  std::vector<float> generateKick(float duration = 0.55f, float startFreq = 135.0f,
                                  float endFreq = 44.0f, float punch = 18.0f) const;

  std::vector<float> generateSnare(float duration = 0.35f, float toneFreq = 185.0f) const;

  std::vector<float> generateHihat(float duration = 0.09f, bool openHat = false) const;

  std::vector<float> generateCowbell(float duration = 0.38f, float rootFreq = 540.0f) const;

  std::vector<float> generateDrumLoop(float bpm, int bars, float swing,
                                    const DrumPattern &pattern) const;

  DrumLoopStems generateDrumLoopStems(float bpm, int bars, float swing,
                                      const DrumPattern &pattern, bool processMixFx) const;

  juce::String exportMpcKit(const juce::File &outputDirectory, const PackExportOptions &opts,
                            bool wrapInMpcKitFolder = true) const;

  juce::String exportBatchKits(const juce::File &parentDirectory, int count,
                               const PackExportOptions &opts) const;

  static bool writeMonoWav(const juce::File &file, const std::vector<float> &samples,
                           double sampleRate);

  static juce::String writePackManifest(const juce::File &kitDir, const PackExportOptions &opts,
                                        const Settings &settings);

private:
  double sampleRate_;
  Settings settings_;

  std::vector<float> cassetteFx(std::vector<float> audio, float drive, int bitDepth,
                                float hissLevel, float lpfCutoff) const;
  std::vector<float> applyWowFlutter(const std::vector<float> &loop) const;

  void filterLowpass(std::vector<float> &data, float cutoffHz, int order) const;
  void filterHighpass(std::vector<float> &data, float cutoffHz, int order) const;

  static float tanhNorm(float x, float drive);
  static void normalizePeak(std::vector<float> &data, float targetPeak);
  static void addInPlace(std::vector<float> &dest, int offset, const std::vector<float> &src,
                         float gain);
  static float lerp(float a, float b, float t);
};
