#include "ExportWorker.h"
#include <juce_events/juce_events.h>

ExportWorker::ExportWorker() : juce::Thread("MemphisPhonkExport") {}

ExportWorker::~ExportWorker() {
  stopThread(12000);
}

void ExportWorker::startJob(ExportJob job,
                            std::function<void(juce::String result)> onComplete,
                            std::function<void(float progress)> onProgress) {
  if (isThreadRunning()) {
    return;
  }
  stopRequested_.store(false);
  job_ = std::move(job);
  onComplete_ = std::move(onComplete);
  onProgress_ = std::move(onProgress);
  startThread();
}

void ExportWorker::requestStop() { stopRequested_.store(true); }

void ExportWorker::run() {
  MemphisPhonkEngine engine(job_.sampleRate);
  engine.setSettings(job_.engineSettings);

  if (onProgress_) {
    onProgress_(0.05f);
  }

  juce::String result;
  if (stopRequested_.load()) {
    result = "Export cancelled.";
  } else if (job_.batch) {
    result = engine.exportBatchKits(job_.parentDir, job_.batchCount, job_.options);
    if (onProgress_) {
      onProgress_(1.0f);
    }
  } else {
    result = engine.exportMpcKit(job_.parentDir, job_.options);
    if (onProgress_) {
      onProgress_(1.0f);
    }
  }

  if (onComplete_) {
    juce::MessageManager::callAsync([cb = onComplete_, result]() { cb(result); });
  }
}
