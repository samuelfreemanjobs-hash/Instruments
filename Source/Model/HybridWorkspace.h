#pragma once

#include "AppMode.h"
#include "Electribe/ElectribeSong.h"
#include "Maschine/MaschineKit.h"
#include "Ableton/SessionMatrix.h"
#include "FlStudio/PatternPlaylist.h"
#include <JuceHeader.h>

namespace vmpc::model
{
/**
 * Unified model for hybrid DAW features (Maschine + Ableton + FL + Electribe).
 * Views/controllers read/write here; audio reads ElectribeSong via ElectribeSequencer.
 */
class HybridWorkspace
{
public:
    HybridWorkspace();

    AppMode getMode() const noexcept { return currentMode; }
    void setMode(AppMode mode) noexcept { currentMode = mode; }

    ElectribeSong& getElectribeSong() noexcept { return electribeSong; }
    MaschineKit& getMaschineKit() noexcept { return maschineKit; }
    SessionMatrix& getSessionMatrix() noexcept { return sessionMatrix; }
    PatternPlaylist& getPatternPlaylist() noexcept { return patternPlaylist; }

    struct FeatureFlags
    {
        bool maschineScenes = true;
        bool maschineGroups = true;
        bool abletonSessionLaunch = true;
        bool abletonFollowActions = true;
        bool flStepSequencer = true;
        bool flPlaylist = true;
        bool electribeMotionSeq = true;
        bool electribeParts = true;
    };

    const FeatureFlags& getFeatures() const noexcept { return features; }

    void applyModeDefaults(AppMode mode);

private:
    AppMode currentMode = AppMode::Electribe;
    ElectribeSong electribeSong;
    MaschineKit maschineKit;
    SessionMatrix sessionMatrix;
    PatternPlaylist patternPlaylist;
    FeatureFlags features;
};
} // namespace vmpc::model
