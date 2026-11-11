#pragma once

#include <juce_core/juce_core.h>
#include <vector>
#include "DrumPattern.h"

/** Procedural Memphis phonk drum kit + loop generator (Python reference parity). */
class MemphisPhonkEngine {
public:
  struct Settings {
    float drive = 2.5f;
    float hissLevel = 0.004f;
    int bitDepth = 12;
    int64_t seed = 0;
  };

  explicit MemphisPhonkEngine(double sampleRate = 44100.0);

  void setSampleRate(double sampleRate) { sampleRate_ = sampleRate; }
  double getSampleRate() const { return sampleRate_; }

  void setSettings(const Settings &s);
  Settings getSettings() const { return settings_; }

  std::vector<float> generateKick(float duration = 0.55f, float startFreq = 135.0f,
                                  float endFreq = 44.0f, float punch = 18.0f) const;

  std::vector<float> generateSnare(float duration = 0.35f, float toneFreq = 185.0f) const;

  std::vector<float> generateHihat(float duration = 0.09f, bool openHat = false) const;

  std::vector<float> generateCowbell(float duration = 0.38f, float rootFreq = 540.0f) const;

  std::vector<float> generateDrumLoop(float bpm, int bars = 2, float swing = 0.03f,
                                    const DrumPattern &pattern = DrumPattern::memphisClassic())
      const;

  /**
   * Writes 20 one-shots + 3 loops (23 WAVs).
   * If `wrapInMpcKitFolder` is true, creates `outputDirectory/Memphis_Phonk_MPC_Kit/`.
   */
  juce::String exportMpcKit(const juce::File &outputDirectory, float swing = 0.03f,
                            const DrumPattern &pattern = DrumPattern::memphisClassic(),
                            bool wrapInMpcKitFolder = true) const;

  /** Writes `count` kits to `parent/Batch_NNN/Kit_MMM/` with unique seeds. */
  juce::String exportBatchKits(const juce::File &parentDirectory, int count, float swing,
                               const DrumPattern &pattern) const;

  static bool writeMonoWav(const juce::File &file, const std::vector<float> &samples,
                           double sampleRate);

private:
  double sampleRate_;
  Settings settings_;

  void setSeed(int64_t seed) const;
  std::vector<float> cassetteFx(std::vector<float> audio, float drive, int bitDepth,
                                float hissLevel, float lpfCutoff) const;

  void filterLowpass(std::vector<float> &data, float cutoffHz, int order) const;
  void filterHighpass(std::vector<float> &data, float cutoffHz, int order) const;

  static float tanhNorm(float x, float drive);
  static void normalizePeak(std::vector<float> &data, float targetPeak);
  static void addInPlace(std::vector<float> &dest, int offset, const std::vector<float> &src,
                         float gain);
  static float lerp(float a, float b, float t);
};
