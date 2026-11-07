#pragma once

#include <vector>

namespace autosampler
{

enum class NoteMapPreset
{
    chromatic = 0,
    minorThird,
    majorThird,
    perfectFifth,
    octave
};

enum class BlackKeyPolicy
{
    include = 0,
    skip,
    whiteKeysOnly
};

struct NoteMapBuilder
{
    static int semitoneStepForPreset (NoteMapPreset preset) noexcept;
    static bool isBlackKey (int midiNote) noexcept;
    static std::vector<int> buildNoteList (int lowNote,
                                             int highNote,
                                             int noteStep,
                                             NoteMapPreset preset,
                                             BlackKeyPolicy blackKeys);
};

} // namespace autosampler
