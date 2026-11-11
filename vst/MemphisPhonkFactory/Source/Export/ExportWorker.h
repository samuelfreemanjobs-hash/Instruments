#pragma once

#include <atomic>
#include <functional>
#include "Dsp/MemphisPhonkEngine.h"
#include "Dsp/PackExportOptions.h"

struct ExportJob {
  bool batch = false;
  int batchCount = 1;
  juce::File parentDir;
  PackExportOptions options;
  MemphisPhonkEngine::Settings engineSettings;
  double sampleRate = 44100.0;
};

/** Runs WAV/MIDI export off the message thread. */
class ExportWorker : private juce::Thread {
public:
  ExportWorker();
  ~ExportWorker() override;

  bool isBusy() const { return isThreadRunning(); }

  void startJob(ExportJob job, std::function<void(juce::String result)> onComplete,
                std::function<void(float progress)> onProgress = {});

  void requestStop();

private:
  ExportJob job_;
  std::function<void(juce::String)> onComplete_;
  std::function<void(float)> onProgress_;
  std::atomic<bool> stopRequested_{false};

  void run() override;
};
