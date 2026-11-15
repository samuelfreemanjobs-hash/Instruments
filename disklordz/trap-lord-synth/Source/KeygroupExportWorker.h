#pragma once
#include "KeygroupExportOptions.h"
#include <JuceHeader.h>
#include <atomic>
#include <functional>

class TrapLordSynthAudioProcessor;

class KeygroupExportWorker final : public juce::Thread
{
public:
    using ProgressFn = std::function<void (float, const juce::String&)>;

    KeygroupExportWorker (TrapLordSynthAudioProcessor& processor);
    void startExport (juce::File destinationParent, KeygroupExportOptions options, ProgressFn onProgress);
    void requestStop() { shouldAbort = true; }
    bool succeeded() const { return success.load(); }
    juce::String resultMessage() const { return resultMsg; }

private:
    void run() override;

    TrapLordSynthAudioProcessor& proc;
    juce::File parentFolder;
    KeygroupExportOptions opts;
    ProgressFn progress;
    std::atomic<bool> shouldAbort { false };
    std::atomic<bool> success { false };
    juce::String resultMsg;
};
