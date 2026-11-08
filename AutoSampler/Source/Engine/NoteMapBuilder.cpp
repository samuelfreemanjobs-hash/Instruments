#include "NoteMapBuilder.h"

#include <JuceHeader.h>

namespace autosampler
{

int NoteMapBuilder::semitoneStepForPreset (NoteMapPreset preset) noexcept
{
    switch (preset)
    {
        case NoteMapPreset::chromatic: return 1;
        case NoteMapPreset::minorThird: return 3;
        case NoteMapPreset::majorThird: return 4;
        case NoteMapPreset::perfectFifth: return 7;
        case NoteMapPreset::octave: return 12;
    }
    return 1;
}

bool NoteMapBuilder::isBlackKey (int midiNote) noexcept
{
    const int pc = midiNote % 12;
    return pc == 1 || pc == 3 || pc == 6 || pc == 8 || pc == 10;
}

std::vector<int> NoteMapBuilder::buildNoteList (int lowNote,
                                                  int highNote,
                                                  int noteStep,
                                                  NoteMapPreset preset,
                                                  BlackKeyPolicy blackKeys)
{
    const int lo = juce::jlimit (0, 127, lowNote);
    const int hi = juce::jlimit (lo, 127, highNote);
    int step = juce::jmax (1, noteStep);

    if (preset != NoteMapPreset::chromatic)
        step = juce::jmax (step, semitoneStepForPreset (preset));

    std::vector<int> notes;
    for (int n = lo; n <= hi; n += step)
    {
        if (blackKeys == BlackKeyPolicy::skip && isBlackKey (n))
            continue;
        if (blackKeys == BlackKeyPolicy::whiteKeysOnly && isBlackKey (n))
            continue;

        notes.push_back (n);
    }

    if (notes.empty() && lo <= hi)
        notes.push_back (lo);

    return notes;
}

} // namespace autosampler
