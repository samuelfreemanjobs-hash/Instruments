#include "MultisampleProgram.h"

namespace autosampler
{

void MultisampleProgram::addZone (Zone zone)
{
    zones_.push_back (std::move (zone));
    rebuildKeyRanges();
}

void MultisampleProgram::rebuildKeyRanges() noexcept
{
    lowKeys_.assign (zones_.size(), 0);
    highKeys_.assign (zones_.size(), 127);

    if (zones_.empty())
        return;

    std::vector<std::size_t> order (zones_.size());
    for (std::size_t i = 0; i < order.size(); ++i)
        order[i] = i;

    std::sort (order.begin(), order.end(), [this] (std::size_t a, std::size_t b)
               { return zones_[a].rootNote < zones_[b].rootNote; });

    for (std::size_t i = 0; i < order.size(); ++i)
    {
        const auto idx = order[i];
        const int root = zones_[idx].rootNote;

        if (i == 0)
            lowKeys_[idx] = 0;
        else
        {
            const int prevRoot = zones_[order[i - 1]].rootNote;
            lowKeys_[idx] = (root + prevRoot + 1) / 2;
        }

        if (i + 1 == order.size())
            highKeys_[idx] = 127;
        else
        {
            const int nextRoot = zones_[order[i + 1]].rootNote;
            highKeys_[idx] = (root + nextRoot - 1) / 2;
        }
    }
}

int MultisampleProgram::zoneIndexForNote (int midiNote) const noexcept
{
    for (std::size_t i = 0; i < zones_.size(); ++i)
    {
        if (midiNote >= lowKeys_[i] && midiNote <= highKeys_[i])
            return static_cast<int> (i);
    }
    return -1;
}

int MultisampleProgram::lowKeyForZone (std::size_t index) const noexcept
{
    return index < lowKeys_.size() ? lowKeys_[index] : 0;
}

int MultisampleProgram::highKeyForZone (std::size_t index) const noexcept
{
    return index < highKeys_.size() ? highKeys_[index] : 127;
}

static juce::String noteTag (int midiNote)
{
    return juce::String (midiNote).paddedLeft ('0', 3);
}

juce::ValueTree MultisampleProgram::toValueTree() const
{
    juce::ValueTree root { "MultisampleProgram" };
    root.setProperty ("name", name, nullptr);

    for (std::size_t zi = 0; zi < zones_.size(); ++zi)
    {
        const auto& z = zones_[zi];
        juce::ValueTree zoneNode { "Zone" };
        zoneNode.setProperty ("root", z.rootNote, nullptr);
        zoneNode.setProperty ("sampleRate", z.sampleRate, nullptr);
        zoneNode.setProperty ("channels", z.audio.getNumChannels(), nullptr);
        zoneNode.setProperty ("length", z.audio.getNumSamples(), nullptr);

        juce::MemoryBlock encoded;
        juce::MemoryOutputStream mos (encoded, false);
        for (int ch = 0; ch < z.audio.getNumChannels(); ++ch)
            mos.write (z.audio.getReadPointer (ch),
                       static_cast<std::size_t> (z.audio.getNumSamples()) * sizeof (float));

        zoneNode.setProperty ("pcm", encoded.toBase64Encoding(), nullptr);
        zoneNode.setProperty ("id", noteTag (z.rootNote), nullptr);
        zoneNode.setProperty ("loopStart", z.loopStart, nullptr);
        zoneNode.setProperty ("loopEnd", z.loopEnd, nullptr);
        root.appendChild (zoneNode, nullptr);
    }

    return root;
}

bool MultisampleProgram::loadFromValueTree (const juce::ValueTree& tree)
{
    if (! tree.hasType ("MultisampleProgram"))
        return false;

    clear();
    name = tree.getProperty ("name", name).toString();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        const auto zoneNode = tree.getChild (i);
        if (! zoneNode.hasType ("Zone"))
            continue;

        Zone z;
        z.rootNote = static_cast<int> (zoneNode.getProperty ("root", 60));
        z.sampleRate = static_cast<double> (zoneNode.getProperty ("sampleRate", 44100.0));
        z.loopStart = static_cast<int> (zoneNode.getProperty ("loopStart", -1));
        z.loopEnd = static_cast<int> (zoneNode.getProperty ("loopEnd", -1));

        const int channels = static_cast<int> (zoneNode.getProperty ("channels", 2));
        const int length = static_cast<int> (zoneNode.getProperty ("length", 0));
        juce::MemoryBlock pcm;
        pcm.fromBase64Encoding (zoneNode.getProperty ("pcm").toString());

        if (length <= 0 || channels <= 0)
            continue;

        z.audio.setSize (channels, length, false, false, true);
        juce::MemoryInputStream mis (pcm, false);
        for (int ch = 0; ch < channels; ++ch)
            mis.read (z.audio.getWritePointer (ch),
                      static_cast<std::size_t> (length) * sizeof (float));

        addZone (std::move (z));
    }

    rebuildKeyRanges();
    return true;
}

} // namespace autosampler
