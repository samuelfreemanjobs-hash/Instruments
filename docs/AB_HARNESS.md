# A/B offline render (manual harness)

Automated spectral diff is not wired yet. Use this workflow to compare JD Upgraded against reference audio:

## Render from the standalone

1. Build: `cmake --build build -j`
2. Run `build/JDUpgraded_artefacts/Release/Standalone/JD Upgraded`
3. Load factory program, set envelopes, record MIDI performance in a DAW or use the plugin in a host and export WAV.

## Compare

```bash
# Level-normalized difference (requires sox)
sox ref.wav plugin.wav -n trim 0 10 stat 2>&1 | head
```

For repeatable notes, send a short MIDI clip (e.g. C4, 2 s hold) through both the reference JD plugin and JD Upgraded with the same program index.

## Headless render (`OfflineRender`)

```bash
cmake --build build -j
./build/OfflineRender /tmp/jdupg-smoke.wav 0 60 100 2.0 44100
```

Arguments: `output.wav [program] [midiNote] [velocity] [seconds] [sampleRate]`.

## Spectral diff (`SpectralDiff`)

Compare two WAV files (peak-normalized mono):

```bash
./build/SpectralDiff reference.wav test.wav --max-rms-db -40 --max-spectral-db -20
```

CI renders the same program twice and asserts near-identical output (determinism regression).

## Golden reference (CI)

Committed capture: `tests/golden/program0-c4-0.5s.wav` (program 0, C4, 0.5 s, 44100 Hz).

Regenerate after intentional DSP changes:

```bash
./build/OfflineRender tests/golden/program0-c4-0.5s.wav 0 60 100 0.5 44100
```

CI also diffs a fresh render against this file via `SpectralDiff`.

## External reference

To compare against a hardware/plugin reference clip, render with matching program/MIDI via `OfflineRender`, then:

```bash
./build/SpectralDiff your_reference.wav /tmp/jdupg-render.wav
```
