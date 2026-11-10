# Manual QA / acceptance test plan

- Compile both formats with Visual Studio 2022, C++20 and JUCE 8.
- Start standalone; configure audio and MIDI devices; play 16 simultaneous notes.
- Verify each of 8 engines, pitch bend, note-on/note-off and sustained release.
- Automate each parameter from a host; save and reopen DAW session.
- Change all chord types; check that releasing input notes releases every generated note.
- Enable arpeggiator and change rate/gate with 1, 2, and 4 held notes; check no hanging notes.
- Import sample map with 2 WAV files; test zones and velocity thresholds.
- Export a keygroup; verify 21 stereo 44.1 kHz / 16-bit WAVs and parseable JSON.
- Verify output never produces NaN/Inf or dangerous peaks, including sample-rate changes.
- Test host unload/reload, window resizing, 44.1/48/96 kHz, blocks 32/128/512/2048.
- Run pluginval and fix all reported failures before commercial distribution.
