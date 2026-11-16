# New drum voice

Follow `.cursor/skills/drum-synth-dsp/SKILL.md`.

1. Add `VoiceName.h` next to `Kick808Voice.h` with `prepare`, `trigger`, and `process`.
2. Reset phase on trigger. Pre-size buffers in `prepare`.
3. Add the parameter ids to `disklordz/drum-engine/params.json`.
4. Register the GM note in `MidiMaps.h`.
5. Extend `DrumEngineTests` with a finite-output and deterministic-render check.
6. Add a `--voice` branch in `DrumBatchExporter.cpp` only if the voice should ship as one-shots.
7. Build `DrumEngineTests` and run ctest. Do not create a new `vst/` tree.
