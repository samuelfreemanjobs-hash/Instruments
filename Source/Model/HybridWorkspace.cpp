#include "HybridWorkspace.h"

namespace vmpc::model
{
namespace
{
const char* kEsxPartLabels[] = { "Kick", "Snare", "Clap", "HH Cl", "HH Op", "Tom", "Perc 1", "Perc 2",
                                 "Audio In", "Synth 1", "Synth 2", "Synth 3", "Synth 4", "Synth 5" };
const char* kEsxSamples[] = { "MPC60_KICK_01", "12BIT_SNARE", "CLAP_909", "CH_HAT", "OP_HAT", "LOW_TOM",
                              "CONGA", "SHAKER", "LINE_IN", "CHOP_RHODES_Fm", "MINOR_STAB", "VOCAL_CHOP",
                              "PAD_WASH", "SUB_808" };
} // namespace

HybridWorkspace::HybridWorkspace()
{
    for (int i = 0; i < ElectribeSong::kNumParts; ++i)
    {
        auto& p = electribeSong.getPart(i);
        p.name = kEsxPartLabels[i];
        p.sampleTag = kEsxSamples[i];
        p.midiNote = 36 + i;
    }

    auto& song = electribeSong;
    song.setPatternName("A.01: BOOMBAP_GRV");

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
