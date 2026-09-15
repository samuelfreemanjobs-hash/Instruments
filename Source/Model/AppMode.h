#pragma once

#include <JuceHeader.h>

namespace resonance::model
{
/** Primary UI / workflow mode (inspired by Electribe, MPC, Maschine, Ableton, FL). */
enum class AppMode
{
    Electribe = 0,   // Step sequencer is the main surface (default)
    HybridMpc,       // Resonance hardware-style layout
    Maschine,        // NI Maschine: groups, scenes, 16 pads
    SessionClip,     // Ableton Live: session view clip matrix
    PatternSong      // FL Studio: step patterns + playlist
};

inline juce::String appModeToString(AppMode mode)
{
    switch (mode)
    {
        case AppMode::Electribe: return "Electribe";
        case AppMode::HybridMpc: return "MPC";
        case AppMode::Maschine: return "Maschine";
        case AppMode::SessionClip: return "Session";
        case AppMode::PatternSong: return "Pattern";
        default: return "Electribe";
    }
}

inline AppMode appModeFromString(const juce::String& s)
{
    if (s.equalsIgnoreCase("MPC") || s.equalsIgnoreCase("HybridMpc"))
        return AppMode::HybridMpc;
    if (s.equalsIgnoreCase("Maschine"))
        return AppMode::Maschine;
    if (s.equalsIgnoreCase("Session") || s.equalsIgnoreCase("SessionClip"))
        return AppMode::SessionClip;
    if (s.equalsIgnoreCase("Pattern") || s.equalsIgnoreCase("PatternSong"))
        return AppMode::PatternSong;
    return AppMode::Electribe;
}
} // namespace resonance::model
