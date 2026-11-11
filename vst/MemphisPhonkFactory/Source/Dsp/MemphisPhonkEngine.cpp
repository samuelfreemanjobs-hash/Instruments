#include "MemphisPhonkEngine.h"
#include "biquad.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <random>

namespace {
constexpr double kPi = 3.14159265358979323846;

struct Rng {
  mutable std::mt19937 gen;
  explicit Rng(int64_t seed) : gen(static_cast<std::uint32_t>(seed)) {}
  float normal(float stddev) {
    std::normal_distribution<float> d(0.0f, stddev);
    return d(gen);
  }
};

float signf(float x) { return x >= 0.0f ? 1.0f : -1.0f; }
} // namespace

MemphisPhonkEngine::MemphisPhonkEngine(double sampleRate) : sampleRate_(sampleRate) {}

void MemphisPhonkEngine::setSettings(const Settings &s) { settings_ = s; }

void MemphisPhonkEngine::setSeed(int64_t seed) const {
  (void)seed;
}

float MemphisPhonkEngine::tanhNorm(float x, float drive) {
  return std::tanh(x * drive) / std::tanh(drive);
}

void MemphisPhonkEngine::normalizePeak(std::vector<float> &data, float targetPeak) {
  float peak = 0.0f;
  for (float s : data) {
    peak = std::max(peak, std::abs(s));
  }
  if (peak > 0.0f) {
    const float g = targetPeak / peak;
    for (float &s : data) {
      s *= g;
    }
  }
}

void MemphisPhonkEngine::addInPlace(std::vector<float> &dest, int offset,
                                    const std::vector<float> &src, float gain) {
  if (offset < 0) {
    return;
  }
  for (size_t i = 0; i < src.size(); ++i) {
    const int idx = offset + static_cast<int>(i);
    if (idx >= static_cast<int>(dest.size())) {
      break;
    }
    dest[static_cast<size_t>(idx)] += src[i] * gain;
  }
}

float MemphisPhonkEngine::lerp(float a, float b, float t) { return a + (b - a) * t; }

void MemphisPhonkEngine::filterLowpass(std::vector<float> &data, float cutoffHz, int order) const {
  const int stages = std::max(1, order / 2);
  for (int s = 0; s < stages; ++s) {
    DspBiquad bq;
    bq.setLowpass(static_cast<float>(sampleRate_), cutoffHz, 0.707f);
    for (float &x : data) {
      x = bq.process(x);
    }
  }
}

void MemphisPhonkEngine::filterHighpass(std::vector<float> &data, float cutoffHz, int order) const {
  const int stages = std::max(1, order / 2);
  for (int s = 0; s < stages; ++s) {
    DspBiquad bq;
    const float w0 = 6.28318530718f * cutoffHz / static_cast<float>(sampleRate_);
    const float cosw0 = std::cos(w0);
    const float sinw0 = std::sin(w0);
    const float q = 0.707f;
    const float alpha = sinw0 / (2.0f * q);
    const float a0 = 1.0f + alpha;
    bq.b0 = ((1.0f + cosw0) * 0.5f) / a0;
    bq.b1 = (-(1.0f + cosw0)) / a0;
    bq.b2 = bq.b0;
    bq.a1 = (-2.0f * cosw0) / a0;
    bq.a2 = (1.0f - alpha) / a0;
    for (float &x : data) {
      x = bq.process(x);
    }
  }
}

std::vector<float> MemphisPhonkEngine::cassetteFx(std::vector<float> audio, float drive,
                                                  int bitDepth, float hissLevel,
                                                  float lpfCutoff) const {
  Rng rng(settings_.seed != 0 ? settings_.seed : 1337);
  const float steps = static_cast<float>(1 << (bitDepth - 1));
  for (float &x : audio) {
    x = tanhNorm(x, drive);
    x = std::round(x * steps) / steps;
    if (std::abs(x) > 0.001f) {
      x += rng.normal(hissLevel);
    }
  }
  filterLowpass(audio, lpfCutoff, 2);
  normalizePeak(audio, 0.89f);
  return audio;
}

std::vector<float> MemphisPhonkEngine::generateKick(float duration, float startFreq, float endFreq,
                                                    float punch) const {
  Rng rng(settings_.seed != 0 ? settings_.seed + 1 : 4242);
  const int n = static_cast<int>(sampleRate_ * duration);
  std::vector<float> kick(static_cast<size_t>(n), 0.0f);
  double phase = 0.0;
  for (int i = 0; i < n; ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
    const float freq = endFreq + (startFreq - endFreq) * std::exp(-t * punch);
    phase += 2.0 * kPi * freq / sampleRate_;
    const float body = static_cast<float>(std::sin(phase));
    const float click = rng.normal(0.3f) * std::exp(-t * 140.0f);
    const float amp = std::exp(-t * 5.5f);
    kick[static_cast<size_t>(i)] = (body + click) * amp;
  }
  return cassetteFx(std::move(kick), 3.2f, 12, settings_.hissLevel, 7500.0f);
}

std::vector<float> MemphisPhonkEngine::generateSnare(float duration, float toneFreq) const {
  Rng rng(settings_.seed != 0 ? settings_.seed + 2 : 5151);
  const int n = static_cast<int>(sampleRate_ * duration);
  std::vector<float> snare(static_cast<size_t>(n), 0.0f);
  std::vector<float> noise(static_cast<size_t>(n), 0.0f);
  for (int i = 0; i < n; ++i) {
    noise[static_cast<size_t>(i)] = rng.normal(1.0f);
  }
  filterHighpass(noise, 1100.0f, 2);
  filterLowpass(noise, 6800.0f, 2);

  double phase = 0.0;
  for (int i = 0; i < n; ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
    const float pitch = toneFreq + 90.0f * std::exp(-t * 45.0f);
    phase += 2.0 * kPi * pitch / sampleRate_;
    const float body = static_cast<float>(std::sin(phase)) * std::exp(-t * 18.0f);
    const float wires = noise[static_cast<size_t>(i)] * std::exp(-t * 12.0f);
    snare[static_cast<size_t>(i)] = body * 0.65f + wires * 0.85f;
  }
  return cassetteFx(std::move(snare), 2.8f, 12, settings_.hissLevel, 9000.0f);
}

std::vector<float> MemphisPhonkEngine::generateHihat(float duration, bool openHat) const {
  Rng rng(settings_.seed != 0 ? settings_.seed + 3 : 6161);
  if (openHat) {
    duration = 0.32f;
  }
  const int n = static_cast<int>(sampleRate_ * duration);
  std::vector<float> hat(static_cast<size_t>(n), 0.0f);
  const float freqs[] = {205.3f, 304.4f, 369.6f, 522.7f, 540.0f, 800.0f};
  const int numFreqs = 6;
  for (int i = 0; i < n; ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
    float metal = 0.0f;
    for (int f = 0; f < numFreqs; ++f) {
      metal += signf(std::sin(2.0f * static_cast<float>(kPi) * freqs[f] * t));
    }
    metal /= static_cast<float>(numFreqs);
    const float noise = rng.normal(0.5f);
    hat[static_cast<size_t>(i)] = metal + noise;
  }
  filterHighpass(hat, 6500.0f, 3);
  for (size_t i = 0; i < hat.size(); ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
    const float decay = openHat ? 7.5f : 48.0f;
    hat[i] *= std::exp(-t * decay);
  }
  return cassetteFx(std::move(hat), 2.0f, 12, settings_.hissLevel, 11000.0f);
}

std::vector<float> MemphisPhonkEngine::generateCowbell(float duration, float rootFreq) const {
  const int n = static_cast<int>(sampleRate_ * duration);
  std::vector<float> bell(static_cast<size_t>(n), 0.0f);
  for (int i = 0; i < n; ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
    const float osc1 = signf(std::sin(2.0f * static_cast<float>(kPi) * rootFreq * t));
    const float osc2 = signf(std::sin(2.0f * static_cast<float>(kPi) * (rootFreq * 1.481f) * t));
    bell[static_cast<size_t>(i)] = osc1 + osc2;
  }
  filterHighpass(bell, 900.0f, 2);
  filterLowpass(bell, 4200.0f, 2);
  for (size_t i = 0; i < bell.size(); ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
    bell[i] *= std::exp(-t * 9.5f);
  }
  return cassetteFx(std::move(bell), 2.6f, 12, settings_.hissLevel, 8000.0f);
}

std::vector<float> MemphisPhonkEngine::generateDrumLoop(float bpm, int bars, float swing,
                                                        const DrumPattern &pattern) const {
  const float beatDur = 60.0f / bpm;
  const float stepDur = beatDur / 4.0f;
  const int totalSteps = bars * 16;
  const int totalSamples = static_cast<int>((totalSteps * stepDur + 0.5) * sampleRate_);
  std::vector<float> loop(static_cast<size_t>(totalSamples), 0.0f);

  const auto kick = generateKick();
  const auto snare = generateSnare();
  const auto ch = generateHihat(0.09f, false);
  const auto oh = generateHihat(0.09f, true);

  for (int step = 0; step < totalSteps; ++step) {
    const int barStep = step % 16;
    const float swingOffset = (step % 2 == 1) ? (stepDur * swing) : 0.0f;
    const int pos = static_cast<int>((step * stepDur + swingOffset) * sampleRate_);

    if (pattern.stepOpenHat(barStep)) {
      addInPlace(loop, pos, oh, 0.55f);
    } else {
      const float vel = (step % 2 == 0) ? 0.55f : 0.35f;
      addInPlace(loop, pos, ch, vel);
    }
    if (pattern.stepKick(barStep)) {
      addInPlace(loop, pos, kick, 0.95f);
    }
    if (pattern.stepSnare(barStep)) {
      addInPlace(loop, pos, snare, 0.90f);
    }
  }

  const int exactLen = static_cast<int>(totalSteps * stepDur * sampleRate_);
  if (exactLen < static_cast<int>(loop.size())) {
    loop.resize(static_cast<size_t>(exactLen));
  }

  std::vector<float> wobbled(loop.size());
  for (size_t i = 0; i < loop.size(); ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(sampleRate_);
    const float wobble = std::sin(2.0f * static_cast<float>(kPi) * 1.8f * t) * 18.0f;
    const float srcIdx = static_cast<float>(i) + wobble;
    const int i0 = static_cast<int>(std::floor(srcIdx));
    const int i1 = std::min(i0 + 1, static_cast<int>(loop.size()) - 1);
    const float frac = srcIdx - static_cast<float>(i0);
    const int clamp0 = std::max(0, i0);
    wobbled[i] = lerp(loop[static_cast<size_t>(clamp0)], loop[static_cast<size_t>(i1)], frac);
  }

  return cassetteFx(std::move(wobbled), 2.4f, 12, 0.005f, 8800.0f);
}

bool MemphisPhonkEngine::writeMonoWav(const juce::File &file, const std::vector<float> &samples,
                                      double sampleRate) {
  juce::AudioBuffer<float> buffer(1, static_cast<int>(samples.size()));
  buffer.copyFrom(0, 0, samples.data(), static_cast<int>(samples.size()));
  std::unique_ptr<juce::FileOutputStream> out(file.createOutputStream());
  if (!out) {
    return false;
  }
  juce::WavAudioFormat wav;
  std::unique_ptr<juce::AudioFormatWriter> writer(
      wav.createWriterFor(out.get(), sampleRate, 1, 16, {}, 0));
  if (!writer) {
    return false;
  }
  out.release();
  return writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
}

namespace {
bool exportLoopWav(const MemphisPhonkEngine &engine, const juce::File &kitDir, float bpm,
                   float swing, const DrumPattern &pattern, double sampleRate) {
  const juce::String name =
      "Memphis_Loop_" + juce::String(juce::roundToInt(bpm)) + "BPM.wav";
  const juce::File dest = kitDir.getChildFile(name);
  if (dest.existsAsFile()) {
    return true;
  }
  return MemphisPhonkEngine::writeMonoWav(
      dest, engine.generateDrumLoop(bpm, 2, swing, pattern), sampleRate);
}
} // namespace

juce::String MemphisPhonkEngine::exportMpcKit(const juce::File &outputDirectory, float primaryBpm,
                                                float swing, const DrumPattern &pattern,
                                                bool includeClassicTempos,
                                                bool wrapInMpcKitFolder) const {
  const juce::File kitDir =
      wrapInMpcKitFolder ? outputDirectory.getChildFile("Memphis_Phonk_MPC_Kit") : outputDirectory;
  if (!kitDir.createDirectory()) {
    return "Could not create directory: " + kitDir.getFullPathName();
  }

  for (int i = 1; i <= 4; ++i) {
    const juce::String idx = juce::String(i).paddedLeft('0', 2);
    const float kickStart = 125.0f + i * 8.0f;
    const float kickEnd = 42.0f + i * 2.0f;
  const float snareTone = 175.0f + i * 12.0f;
    const float cowFreq = 520.0f + i * 15.0f;

    if (!writeMonoWav(kitDir.getChildFile("Phonk_Kick_" + idx + ".wav"),
                      generateKick(0.55f, kickStart, kickEnd, 18.0f), sampleRate_)) {
      return "Failed writing kick " + idx;
    }
    if (!writeMonoWav(kitDir.getChildFile("Phonk_Snare_" + idx + ".wav"),
                      generateSnare(0.35f, snareTone), sampleRate_)) {
      return "Failed writing snare " + idx;
    }
    if (!writeMonoWav(kitDir.getChildFile("Phonk_ClosedHat_" + idx + ".wav"),
                      generateHihat(0.09f, false), sampleRate_)) {
      return "Failed writing closed hat " + idx;
    }
    if (!writeMonoWav(kitDir.getChildFile("Phonk_OpenHat_" + idx + ".wav"),
                      generateHihat(0.09f, true), sampleRate_)) {
      return "Failed writing open hat " + idx;
    }
    if (!writeMonoWav(kitDir.getChildFile("Phonk_Cowbell_" + idx + ".wav"),
                      generateCowbell(0.38f, cowFreq), sampleRate_)) {
      return "Failed writing cowbell " + idx;
    }
  }

  int loopFiles = 0;
  if (!exportLoopWav(*this, kitDir, primaryBpm, swing, pattern, sampleRate_)) {
    return "Failed writing loop at " + juce::String(juce::roundToInt(primaryBpm)) + " BPM";
  }
  ++loopFiles;

  if (includeClassicTempos) {
    for (float bpm : {112.0f, 118.0f, 124.0f}) {
      if (std::abs(bpm - primaryBpm) < 0.05f) {
        continue;
      }
      if (!exportLoopWav(*this, kitDir, bpm, swing, pattern, sampleRate_)) {
        return "Failed writing loop at " + juce::String(juce::roundToInt(bpm)) + " BPM";
      }
      ++loopFiles;
    }
  }

  const int total = 20 + loopFiles;
  return "Exported " + juce::String(total) + " WAVs (loop " +
         juce::String(juce::roundToInt(primaryBpm)) + " BPM) to " + kitDir.getFullPathName();
}

juce::String MemphisPhonkEngine::exportBatchKits(const juce::File &parentDirectory, int count,
                                                 float primaryBpm, float swing,
                                                 const DrumPattern &pattern,
                                                 bool includeClassicTempos) const {
  if (count < 1) {
    return "Batch count must be at least 1";
  }
  if (count > 64) {
    count = 64;
  }

  const auto stamp =
      juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S");
  const juce::File batchRoot = parentDirectory.getChildFile("Memphis_Phonk_Batch_" + stamp);
  if (!batchRoot.createDirectory()) {
    return "Could not create batch folder: " + batchRoot.getFullPathName();
  }

  const auto saved = settings_;
  int exported = 0;
  for (int kit = 1; kit <= count; ++kit) {
    MemphisPhonkEngine variant(sampleRate_);
    auto s = saved;
    s.seed = saved.seed + kit * 104729;
    variant.setSettings(s);

    const juce::String kitName = "Kit_" + juce::String(kit).paddedLeft('0', 3);
    const juce::String msg = variant.exportMpcKit(batchRoot.getChildFile(kitName), primaryBpm,
                                                  swing, pattern, includeClassicTempos, false);
    if (!msg.contains("Exported")) {
      return msg;
    }
    ++exported;
  }

  return "Batch exported " + juce::String(exported) + " kits (" +
         juce::String(exported * 23) + " WAVs) to " + batchRoot.getFullPathName();
}
