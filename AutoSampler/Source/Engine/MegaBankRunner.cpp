#include "MegaBankRunner.h"
#include "../Export/MpcKeygroupExporter.h"
#include "../Export/MpcKeyzoneValidator.h"
#include "MegaBankTemplate.h"
#include "Vst3PluginCache.h"

namespace autosampler
{

static NoteMapPreset parseNoteMap (const juce::String& name)
{
    if (name.equalsIgnoreCase ("minorThird"))
        return NoteMapPreset::minorThird;
    if (name.equalsIgnoreCase ("majorThird"))
        return NoteMapPreset::majorThird;
    if (name.equalsIgnoreCase ("fifth") || name.equalsIgnoreCase ("perfectFifth"))
        return NoteMapPreset::perfectFifth;
    if (name.equalsIgnoreCase ("octave"))
        return NoteMapPreset::octave;
    return NoteMapPreset::chromatic;
}

static BlackKeyPolicy parseBlackKeyPolicy (const juce::String& name)
{
    if (name.equalsIgnoreCase ("skip"))
        return BlackKeyPolicy::skip;
    if (name.equalsIgnoreCase ("whiteKeysOnly"))
        return BlackKeyPolicy::whiteKeysOnly;
    return BlackKeyPolicy::include;
}

static PostProcessConfig postFromVar (const juce::var& captureObj, const PostProcessConfig& defaults)
{
    PostProcessConfig cfg = defaults;
    if (captureObj.isVoid())
        return cfg;

    if (captureObj.hasProperty ("trimSilence"))
        cfg.trimSilence = static_cast<bool> (captureObj.getProperty ("trimSilence", cfg.trimSilence));
    if (captureObj.hasProperty ("normalize"))
        cfg.normalize = static_cast<bool> (captureObj.getProperty ("normalize", cfg.normalize));
    if (captureObj.hasProperty ("sustainLoop"))
        cfg.findSustainLoop = static_cast<bool> (captureObj.getProperty ("sustainLoop", cfg.findSustainLoop));
    if (captureObj.hasProperty ("trimThresholdDb"))
        cfg.trimThresholdDb = static_cast<float> (captureObj.getProperty ("trimThresholdDb", cfg.trimThresholdDb));
    if (captureObj.hasProperty ("normalizePeakDb"))
        cfg.normalizePeakDb = static_cast<float> (captureObj.getProperty ("normalizePeakDb", cfg.normalizePeakDb));
    if (captureObj.hasProperty ("minLoopConfidence"))
        cfg.minLoopConfidence = static_cast<float> (captureObj.getProperty ("minLoopConfidence", cfg.minLoopConfidence));
    return cfg;
}

static AutoRecordEngine::Config captureFromVar (const juce::var& v)
{
    AutoRecordEngine::Config cfg;
    cfg.lowNote = static_cast<int> (v.getProperty ("lowNote", cfg.lowNote));
    cfg.highNote = static_cast<int> (v.getProperty ("highNote", cfg.highNote));
    cfg.noteStep = static_cast<int> (v.getProperty ("noteStep", cfg.noteStep));
    cfg.recordSeconds = static_cast<double> (v.getProperty ("recordSeconds", cfg.recordSeconds));
    cfg.gapSeconds = static_cast<double> (v.getProperty ("gapMs", cfg.gapSeconds * 1000.0)) / 1000.0;
    cfg.velocity = static_cast<int> (v.getProperty ("recordVelocity", cfg.velocity));
    cfg.postProcess = postFromVar (v, {});
    cfg.noteMapPreset = parseNoteMap (v.getProperty ("noteMapPreset", "chromatic").toString());
    cfg.blackKeyPolicy = parseBlackKeyPolicy (v.getProperty ("blackKeyPolicy", "include").toString());
    cfg.multiVelocityLayers = static_cast<bool> (v.getProperty ("multiVelocityLayers", false));
    cfg.recordUntilSilence = static_cast<bool> (v.getProperty ("recordUntilSilence", true));
    cfg.maxRecordSeconds = static_cast<double> (v.getProperty ("maxRecordSeconds", cfg.maxRecordSeconds));
    cfg.silenceThresholdDb = static_cast<float> (v.getProperty ("silenceThresholdDb", cfg.silenceThresholdDb));
    cfg.silenceHoldSeconds = static_cast<double> (v.getProperty ("silenceHoldSeconds", cfg.silenceHoldSeconds));
    cfg.noteOffAtRatio = static_cast<double> (v.getProperty ("noteOffAtRatio", cfg.noteOffAtRatio));

    if (auto* velArr = v.getProperty ("velocityLayers", {}).getArray())
    {
        cfg.velocityLayers.clear();
        for (const auto& vel : *velArr)
            cfg.velocityLayers.push_back (static_cast<int> (vel));
    }

    return cfg;
}

static MegaBankRunOptions runOptionsFromVar (const juce::var& v)
{
    MegaBankRunOptions opt;
    opt.failFast = static_cast<bool> (v.getProperty ("failFast", opt.failFast));
    opt.skipFailedSources = static_cast<bool> (v.getProperty ("skipFailedSources", opt.skipFailedSources));
    opt.maxRetriesPerSource = static_cast<int> (v.getProperty ("maxRetriesPerSource", opt.maxRetriesPerSource));
    opt.sourceTimeoutSeconds = static_cast<double> (v.getProperty ("sourceTimeoutSeconds", opt.sourceTimeoutSeconds));
    opt.resume = static_cast<bool> (v.getProperty ("resume", opt.resume));
    opt.writeProgress = static_cast<bool> (v.getProperty ("writeProgress", opt.writeProgress));

    if (opt.failFast && ! v.hasProperty ("maxRetriesPerSource"))
        opt.maxRetriesPerSource = 0;

    const auto cachePath = v.getProperty ("pluginCacheFile", {}).toString();
    if (cachePath.isNotEmpty())
        opt.pluginCacheFile = juce::File (cachePath);

    return opt;
}

bool MegaBankRunner::loadJob (const juce::File& jsonFile, MegaBankJob& job, juce::String& error)
{
    const juce::var root = juce::JSON::parse (jsonFile);
    if (root.isVoid())
    {
        error = "Invalid JSON";
        return false;
    }

    job.name = root.getProperty ("name", job.name).toString();
    job.captureDefaults = captureFromVar (root.getProperty ("capture", {}));
    job.runOptions = runOptionsFromVar (root.getProperty ("run", {}));
    job.sources.clear();

    if (auto* arr = root.getProperty ("sources", {}).getArray())
    {
        for (const auto& entry : *arr)
        {
            if (static_cast<int> (job.sources.size()) >= kMaxSourcePlugins)
                break;

            Vst3CaptureSpec spec;
            spec.vst3Path = juce::File (entry.getProperty ("vst3Path", juce::var()).toString());
            spec.presetName = entry.getProperty ("presetName", entry.getProperty ("name", {})).toString();
            spec.pluginProgramIndex = static_cast<int> (entry.getProperty ("programIndex", 0));
            spec.captureConfig = job.captureDefaults;

            if (entry.hasProperty ("lowNote"))
                spec.captureConfig.lowNote = static_cast<int> (entry.getProperty ("lowNote", spec.captureConfig.lowNote));
            if (entry.hasProperty ("highNote"))
                spec.captureConfig.highNote = static_cast<int> (entry.getProperty ("highNote", spec.captureConfig.highNote));
            if (entry.hasProperty ("noteStep"))
                spec.captureConfig.noteStep = static_cast<int> (entry.getProperty ("noteStep", spec.captureConfig.noteStep));

            if (! spec.vst3Path.exists())
            {
                error = "Missing vst3Path for source: " + spec.presetName;
                return false;
            }

            job.sources.push_back (spec);
        }
    }

    if (job.sources.empty())
    {
        error = "No sources listed (max " + juce::String (kMaxSourcePlugins) + ")";
        return false;
    }

    return true;
}

bool MegaBankRunner::writeMegaBankTemplate (const juce::File& jsonFile, juce::String& error)
{
    return writeMegaBankJsonTemplate (jsonFile, error);
}

static void recordSkip (MegaBankRunner::RunSummary* summary,
                        const juce::String& presetName,
                        const juce::String& reason,
                        juce::StringArray& skippedLines)
{
    if (summary != nullptr)
    {
        ++summary->skipped;
        summary->skippedPresets.add (presetName);
        summary->skippedReasons.add (reason);
    }
    skippedLines.add (presetName + " | " + reason);
}

bool MegaBankRunner::runJob (const MegaBankJob& job, const juce::File& outputDirectory, juce::String& error, RunSummary* summary)
{
    if (summary != nullptr)
    {
        summary->sourcesTotal = static_cast<int> (job.sources.size());
        summary->completed = 0;
        summary->skipped = 0;
        summary->skippedPresets.clearQuick();
        summary->skippedReasons.clearQuick();
    }
    if (! Vst3OfflineRenderer::isSupportedOnThisPlatform())
    {
        error = "VST3 mega-bank capture requires a desktop build with JUCE_PLUGINHOST_VST3";
        return false;
    }

    BankExportSpec bank;
    bank.bankName = job.name;

    const juce::File rootDir = outputDirectory.getChildFile (job.name.replaceCharacter ('/', '_'));
    rootDir.createDirectory();

    const juce::File progressPath = MegaBankProgress::progressFileFor (outputDirectory, job.name);
    juce::var progressState = MegaBankProgress::createEmpty (job.name, static_cast<int> (job.sources.size()));

    if (job.runOptions.resume)
    {
        juce::String progressErr;
        juce::var loaded;
        if (MegaBankProgress::load (progressPath, loaded, progressErr) && ! loaded.isVoid())
            progressState = loaded;
    }

    MegaBankProgress::setRunning (progressState,
                                  job.name,
                                  static_cast<int> (job.sources.size()),
                                  {});

    if (job.runOptions.writeProgress)
    {
        juce::String progressErr;
        MegaBankProgress::save (progressPath, progressState, progressErr);
    }

    Vst3PluginCache pluginCache (job.runOptions.pluginCacheFile.getFullPathName().isNotEmpty()
                                     ? job.runOptions.pluginCacheFile
                                     : rootDir.getChildFile ("vst3_plugin_cache.json"));

    juce::StringArray validationLines;
    juce::StringArray skippedLines;

    for (const auto& spec : job.sources)
    {
        if (job.runOptions.resume && MegaBankProgress::isSourceCompleted (progressState, spec.presetName))
            continue;

        MegaBankProgress::setRunning (progressState,
                                      job.name,
                                      static_cast<int> (job.sources.size()),
                                      spec.presetName);
        if (job.runOptions.writeProgress)
        {
            juce::String progressErr;
            MegaBankProgress::save (progressPath, progressState, progressErr);
        }

        MultisampleProgram program;
        bool captured = false;
        juce::String lastErr;

        for (int attempt = 0; attempt <= job.runOptions.maxRetriesPerSource; ++attempt)
        {
            lastErr.clear();
            if (Vst3OfflineRenderer::captureProgram (spec,
                                                      program,
                                                      lastErr,
                                                      &pluginCache,
                                                      job.runOptions.sourceTimeoutSeconds))
            {
                captured = true;
                break;
            }

            if (attempt < job.runOptions.maxRetriesPerSource)
                juce::Thread::sleep (2000);
        }

        if (! captured)
        {
            const juce::String reason = "capture failed: " + lastErr;
            MegaBankProgress::markSourceFailed (progressState, spec.presetName, lastErr, job.runOptions.maxRetriesPerSource + 1);
            recordSkip (summary, spec.presetName, reason, skippedLines);

            if (job.runOptions.writeProgress)
            {
                juce::String progressErr;
                MegaBankProgress::save (progressPath, progressState, progressErr);
            }

            if (! job.runOptions.skipFailedSources)
            {
                error = "Failed source " + spec.presetName + ": " + lastErr;
                MegaBankProgress::setFinished (progressState, false);
                if (job.runOptions.writeProgress)
                    MegaBankProgress::save (progressPath, progressState, error);
                return false;
            }
            continue;
        }

        const auto validation = MpcKeyzoneValidator::validateProgram (program);
        validationLines.add (spec.presetName + ": " + (validation.ok ? "OK" : "FAILED"));
        if (! validation.ok)
            validationLines.add (validation.reportText);

        if (! validation.ok)
        {
            const juce::String reason = "MPC validation failed: " + validation.errors.joinIntoString ("; ");
            if (job.runOptions.skipFailedSources)
            {
                MegaBankProgress::markSourceFailed (progressState, spec.presetName, validation.errors.joinIntoString ("; "), 1);
                recordSkip (summary, spec.presetName, reason, skippedLines);
                if (job.runOptions.writeProgress)
                {
                    juce::String progressErr;
                    MegaBankProgress::save (progressPath, progressState, progressErr);
                }
                continue;
            }

            error = spec.presetName + ": " + reason;
            MegaBankProgress::setFinished (progressState, false);
            if (job.runOptions.writeProgress)
                MegaBankProgress::save (progressPath, progressState, error);
            return false;
        }

        const auto exported = MultisampleExporter::exportMpcKit (program, rootDir.getChildFile ("presets"), true);
        if (! exported.ok)
        {
            recordSkip (summary, spec.presetName, "export failed: " + exported.error, skippedLines);
            MegaBankProgress::markSourceFailed (progressState, spec.presetName, exported.error, 1);

            if (! job.runOptions.skipFailedSources)
            {
                error = exported.error;
                MegaBankProgress::setFinished (progressState, false);
                if (job.runOptions.writeProgress)
                    MegaBankProgress::save (progressPath, progressState, error);
                return false;
            }
            continue;
        }

        bank.presets.push_back (std::move (program));
        if (summary != nullptr)
            ++summary->completed;
        MegaBankProgress::markSourceCompleted (progressState,
                                                 spec.presetName,
                                                 exported.presetJson.getParentDirectory());

        if (job.runOptions.writeProgress)
        {
            juce::String progressErr;
            MegaBankProgress::save (progressPath, progressState, progressErr);
        }
    }

    if (bank.presets.empty())
    {
        error = "Mega bank produced no presets";
        MegaBankProgress::setFinished (progressState, false);
        if (job.runOptions.writeProgress)
            MegaBankProgress::save (progressPath, progressState, error);
        return false;
    }

    const auto bankResult = MultisampleExporter::exportBank (bank, rootDir);
    if (! bankResult.ok)
    {
        error = bankResult.error;
        MegaBankProgress::setFinished (progressState, false);
        if (job.runOptions.writeProgress)
            MegaBankProgress::save (progressPath, progressState, error);
        return false;
    }

    const juce::File reportFile = MegaBankProgress::validationReportFor (outputDirectory, job.name);
    juce::String report = "Mega bank validation report\n";
    report << validationLines.joinIntoString ("\n");
    reportFile.replaceWithText (report);

    const juce::File skippedFile = MegaBankProgress::skippedReportFor (outputDirectory, job.name);
    juce::String skippedReport = "Mega bank skipped presets report\n";
    skippedReport << "Bank: " << job.name << "\n";
    skippedReport << "Completed: " << juce::String (summary != nullptr ? summary->completed : static_cast<int> (bank.presets.size())) << "\n";
    skippedReport << "Skipped: " << juce::String (skippedLines.size()) << "\n\n";
    if (skippedLines.isEmpty())
        skippedReport << "(none)\n";
    else
        skippedReport << skippedLines.joinIntoString ("\n") << "\n";
    skippedFile.replaceWithText (skippedReport);

    const bool hadSkips = ! skippedLines.isEmpty();
    MegaBankProgress::setFinished (progressState, true, hadSkips);
    if (job.runOptions.writeProgress)
        MegaBankProgress::save (progressPath, progressState, error);

    if (hadSkips)
    {
        error = "Skipped " + juce::String (skippedLines.size()) + " preset(s). See "
                + skippedFile.getFullPathName() + ":\n" + skippedLines.joinIntoString ("\n");
        return false;
    }

    return true;
}

} // namespace autosampler
