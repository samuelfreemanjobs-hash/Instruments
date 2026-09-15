#include "SequencerSong.h"

namespace vmpc::model
{
SequencerTrack& SequencerSong::getTrack(int index)
{
    while (index >= static_cast<int>(tracks.size()))
        addTrack("Track " + juce::String(tracks.size() + 1));
    return tracks[static_cast<size_t>(index)];
}

const SequencerTrack& SequencerSong::getTrack(int index) const
{
    static SequencerTrack empty;
    if (index < 0 || index >= static_cast<int>(tracks.size()))
        return empty;
    return tracks[static_cast<size_t>(index)];
}

int SequencerSong::addTrack(const juce::String& name)
{
    SequencerTrack t;
    t.name = name;
    tracks.push_back(std::move(t));
    return static_cast<int>(tracks.size()) - 1;
}
} // namespace vmpc::model
