#pragma once

#include "AutoRecordEngine.h"
#include "NoteMapBuilder.h"

namespace autosampler
{

/** MPC Auto Sampler-style session summary (keys × velocities × timing). */
struct AutoSampleSessionPlan
{
    int noteCount = 0;
    int velocityCount = 0;
    int totalCaptures = 0;
    double secondsPerCapture = 0.0;
    /** Gate + release (+ preroll) + pause — MPC “per split” wall time. */
    double secondsPerSplit = 0.0;
    double pauseSeconds = 0.0;
    double estimatedTotalSeconds = 0.0;
};

class AutoSampleSessionPlanBuilder
{
public:
    static AutoSampleSessionPlan plan (const AutoRecordEngine::Config& config);
    static std::vector<int> activeVelocities (const AutoRecordEngine::Config& config);
    static double captureLengthSeconds (const AutoRecordEngine::Config& config);
};

} // namespace autosampler
