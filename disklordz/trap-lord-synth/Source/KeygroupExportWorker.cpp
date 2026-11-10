#include "KeygroupExportWorker.h"
#include "PluginProcessor.h"

KeygroupExportWorker::KeygroupExportWorker (TrapLordSynthAudioProcessor& p)
    : juce::Thread ("MpcKeygroupExport"), proc (p)
{
}

void KeygroupExportWorker::startExport (juce::File destinationParent,
                                        KeygroupExportOptions options,
                                        ProgressFn onProgress)
{
    parentFolder = std::move (destinationParent);
    opts = std::move (options);
    progress = std::move (onProgress);
    shouldAbort = false;
    success = false;
    startThread (juce::Thread::Priority::normal);
}

void KeygroupExportWorker::run()
{
    const bool ok = proc.exportMpcKeygroupOffline (parentFolder, opts, shouldAbort, progress);
    success = ok;
    resultMsg = ok ? "MPC keygroup export complete" : "Export failed or cancelled";
}
