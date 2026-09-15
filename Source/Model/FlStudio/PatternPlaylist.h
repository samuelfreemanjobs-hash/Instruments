#pragma once

#include <JuceHeader.h>
#include <vector>

namespace vmpc::model
{
/** FL Studio: channel patterns + playlist arrangement. */
struct StepPattern
{
    juce::String name { "Pattern" };
    int colorIndex = 0;
    int lengthSteps = 16;
    // Shares Electribe-style steps per channel in full implementation
};

struct PlaylistBlock
{
    int patternIndex = 0;
    double startBeat = 0.0;
    double lengthBeats = 16.0;
};

class PatternPlaylist
{
public:
    StepPattern& getPattern(int index);
    const StepPattern& getPattern(int index) const;

    int getPatternCount() const noexcept { return static_cast<int>(patterns.size()); }
    int addPattern();

    const std::vector<PlaylistBlock>& getPlaylist() const noexcept { return playlist; }
    void addPlaylistBlock(int patternIndex, double startBeat, double lengthBeats);

    int getSelectedPattern() const noexcept { return selectedPattern; }
    void setSelectedPattern(int p) noexcept { selectedPattern = juce::jmax(0, p); }

    bool isSongMode() const noexcept { return songMode; }
    void setSongMode(bool on) noexcept { songMode = on; }

private:
    std::vector<StepPattern> patterns;
    std::vector<PlaylistBlock> playlist;
    int selectedPattern = 0;
    bool songMode = false;
};
} // namespace vmpc::model
