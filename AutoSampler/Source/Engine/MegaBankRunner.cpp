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
    opt.maxRetriesPerSource = static_cast<int> (v.getProperty ("maxRetriesPerSource", opt.maxRetriesPerSource));
    opt.sourceTimeoutSeconds = static_cast<double> (v.getProperty ("sourceTimeoutSeconds", opt.sourceTimeoutSeconds));
    opt.resume = static_cast<bool> (v.getProperty ("resume", opt.resume));
    opt.writeProgress = static_cast<bool> (v.getProperty ("writeProgress", opt.writeProgress));

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

bool MegaBankRunner::runJob (const MegaBankJob& job, const juce::File& outputDirectory, juce::String& error)
{
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
            MegaBankProgress::markSourceFailed (progressState, spec.presetName, lastErr, job.runOptions.maxRetriesPerSource + 1);
            if (job.runOptions.writeProgress)
            {
                juce::String progressErr;
                MegaBankProgress::save (progressPath, progressState, progressErr);
            }

            error = "Failed source " + spec.presetName + ": " + lastErr;
            MegaBankProgress::setFinished (progressState, false);
            if (job.runOptions.writeProgress)
            {
                juce::String progressErr;
                MegaBankProgress::save (progressPath, progressState, progressErr);
            }
            return false;
        }

        const auto validation = MpcKeyzoneValidator::validateProgram (program);
        validationLines.add (spec.presetName + ": " + (validation.ok ? "OK" : "FAILED"));
        if (! validation.ok)
            validationLines.add (validation.reportText);

        const auto exported = MultisampleExporter::exportMpcKit (program, rootDir.getChildFile ("presets"), true);
        if (! exported.ok)
        {
            error = exported.error;
            return false;
        }

        bank.presets.push_back (std::move (program));
        MegaBankProgress::markSourceCompleted (progressState,
                                                 spec.presetName,
                                                 exported.presetJson.getParentDirectory());

        if (job.runOptions.writeProgress)
        {
            juce::String progressErr;
            MegaBankProgress::save (progressPath, progressState, progressErr);
        }
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

    MegaBankProgress::setFinished (progressState, true);
    if (job.runOptions.writeProgress)
        MegaBankProgress::save (progressPath, progressState, error);

    return true;
}

} // namespace autosampler
