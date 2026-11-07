#include "MegaBankRunner.h"
#include "../Export/MpcKeygroupExporter.h"

namespace autosampler
{

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
    return cfg;
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
    job.sources.clear();

    if (auto* arr = root.getProperty ("sources", {}).getArray())
    {
        for (const auto& entry : *arr)
        {
            if (static_cast<int> (job.sources.size()) >= kMaxSourcePlugins)
                break;

            Vst3CaptureSpec spec;
            spec.vst3Path = juce::File (entry.getProperty ("vst3Path").toString());
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
                error = "Missing vst3Path for source";
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

    for (const auto& spec : job.sources)
    {
        MultisampleProgram program;
        if (! Vst3OfflineRenderer::captureProgram (spec, program, error))
            return false;

        const auto exported = MultisampleExporter::exportPreset (program, rootDir.getChildFile ("presets"));
        if (! exported.ok)
        {
            error = exported.error;
            return false;
        }

        juce::String mpcErr;
        if (! MpcKeygroupExporter::exportKeygroup (program, rootDir.getChildFile ("mpc"), mpcErr))
        {
            error = mpcErr;
            return false;
        }

        bank.presets.push_back (std::move (program));
    }

    const auto bankResult = MultisampleExporter::exportBank (bank, rootDir);
    if (! bankResult.ok)
    {
        error = bankResult.error;
        return false;
    }

    return true;
}

} // namespace autosampler
