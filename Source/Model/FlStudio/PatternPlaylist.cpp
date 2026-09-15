#include "PatternPlaylist.h"

namespace vmpc::model
{
StepPattern& PatternPlaylist::getPattern(int index)
{
    while (index >= static_cast<int>(patterns.size()))
        addPattern();
    return patterns[static_cast<size_t>(index)];
}

const StepPattern& PatternPlaylist::getPattern(int index) const
{
    static StepPattern empty;
    if (index < 0 || index >= static_cast<int>(patterns.size()))
        return empty;
    return patterns[static_cast<size_t>(index)];
}

int PatternPlaylist::addPattern()
{
    StepPattern p;
    p.name = "Pattern " + juce::String(static_cast<int>(patterns.size()) + 1);
    patterns.push_back(p);
    return static_cast<int>(patterns.size()) - 1;
}

void PatternPlaylist::addPlaylistBlock(int patternIndex, double startBeat, double lengthBeats)
{
    playlist.push_back({ patternIndex, startBeat, lengthBeats });
}
} // namespace vmpc::model
