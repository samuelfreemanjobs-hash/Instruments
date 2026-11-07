#include "AutoSampleSessionPlan.h"

namespace autosampler
{

std::vector<int> AutoSampleSessionPlanBuilder::activeVelocities (const AutoRecordEngine::Config& config)
{
    std::vector<int> out;
    for (int v : config.velocitySlots)
    {
        if (v > 0)
            out.push_back (juce::jlimit (1, 127, v));
    }

    if (out.empty() && config.velocity > 0)
        out.push_back (config.velocity);

    if (out.empty())
        out.push_back (100);

    return out;
}

double AutoSampleSessionPlanBuilder::captureLengthSeconds (const AutoRecordEngine::Config& config)
{
    if (config.recordUntilSilence)
        return config.maxRecordSeconds;

    return (config.prerollMs + config.noteHoldMs + config.tailMs) / 1000.0;
}

AutoSampleSessionPlan AutoSampleSessionPlanBuilder::plan (const AutoRecordEngine::Config& config)
{
    AutoSampleSessionPlan plan;
    const auto notes = NoteMapBuilder::buildNoteList (config.lowNote,
                                                        config.highNote,
                                                        config.noteStep,
                                                        config.noteMapPreset,
                                                        config.blackKeyPolicy);
    const auto velocities = activeVelocities (config);

    plan.noteCount = static_cast<int> (notes.size());
    plan.velocityCount = static_cast<int> (velocities.size());
    plan.totalCaptures = plan.noteCount * plan.velocityCount;
    plan.secondsPerCapture = captureLengthSeconds (config);
    plan.pauseSeconds = config.pauseMs / 1000.0;

    if (plan.totalCaptures <= 0)
        return plan;

    const double captureTime = static_cast<double> (plan.totalCaptures) * plan.secondsPerCapture;
    const double pauseTime = static_cast<double> (juce::jmax (0, plan.totalCaptures - 1)) * plan.pauseSeconds;
    plan.estimatedTotalSeconds = captureTime + pauseTime;
    return plan;
}

} // namespace autosampler
