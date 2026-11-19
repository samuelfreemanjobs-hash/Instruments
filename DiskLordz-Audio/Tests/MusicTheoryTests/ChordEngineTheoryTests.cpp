#include "../../DPF-MIDI/MusicTheory/MusicTheory.h"

#include <cassert>
#include <iostream>

int main()
{
    using namespace disklordz::theory;

    const int root = 5; // F
    const auto scale = ScaleType::naturalMinor;

    const auto notes = buildVoicedChord(root, scale, 1, ChordQuality::minor, 0, 0.2f, 48);
    assert(notes.size() >= 3);

    for (int n : notes)
        assert(isPitchInScale(n, root, scale));

    const auto trap = buildVoicedChord(root, scale, 6, ChordQuality::major, 0, 0.0f, 48);
    assert(!trap.empty());

    const int rootPc = chordRootPitchClass(root, scale, 3);
    assert(rootPc >= 0 && rootPc < 12);

    std::cout << "ChordEngineTheoryTests: OK (" << notes.size() << " notes in i chord)\n";
    return 0;
}