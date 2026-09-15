#include "HybridWorkspace.h"

namespace vmpc::model
{
HybridWorkspace::HybridWorkspace()
{
    for (int i = 0; i < ElectribeSong::kNumParts; ++i)
    {
        auto& p = electribeSong.getPart(i);
        p.name = "PART " + juce::String(i + 1);
        p.midiNote = 36 + i;
    }

    for (int g = 0; g < MaschineKit::kGroups; ++g)
        maschineKit.getGroup(g).name = "GROUP " + juce::String(char('A' + g));

    patternPlaylist.addPattern();
    patternPlaylist.addPattern();
    patternPlaylist.addPattern();
    patternPlaylist.addPattern();
    patternPlaylist.addPlaylistBlock(0, 0.0, 16.0);
    patternPlaylist.addPlaylistBlock(1, 16.0, 16.0);
    patternPlaylist.addPlaylistBlock(2, 32.0, 8.0);

    applyModeDefaults(AppMode::Electribe);
}

void HybridWorkspace::applyModeDefaults(AppMode mode)
{
    currentMode = mode;

    switch (mode)
    {
        case AppMode::Electribe:
            electribeSong.setSelectedPart(0);
            break;
        case AppMode::Maschine:
            maschineKit.setActiveGroup(0);
            maschineKit.setActiveScene(0);
            break;
        case AppMode::SessionClip:
            sessionMatrix.setQuantizeBeats(1.0);
            break;
        case AppMode::PatternSong:
            patternPlaylist.setSelectedPattern(0);
            break;
        case AppMode::HybridMpc:
        default:
            break;
    }
}
} // namespace vmpc::model
