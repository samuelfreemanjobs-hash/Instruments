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

Committed captures live under `tests/golden/`. Each row in **`tests/golden/manifest.tsv`** defines:

`filename · program index · MIDI note · velocity · seconds · sample rate`

| File | Program | Category (factory) | Note |
|------|---------|-------------------|------|
| `program0-c4-0.5s.wav` | 0 | EP | C4 |
| `program24-c4-0.5s.wav` | 24 | Pad | C4 |
| `program48-e2-0.5s.wav` | 48 | Bass | E2 |
| `program64-c4-0.5s.wav` | 64 | Vapor | C4 |
| `program80-c4-0.5s.wav` | 80 | 80s R&B | C4 |
| `program4-c4-0.5s.wav` | 4 | EP (cross-mod coupling) | C4 |
| `program112-c4-0.5s.wav` | 112 | Elite (coupling) | C4 |

Verify locally (after build):

```bash
./tests/golden/verify_golden.sh
```

Regenerate all golden files after intentional DSP changes:

```bash
./tests/golden/refresh_golden.sh
git add tests/golden/*.wav
```

CI runs `verify_golden.sh` after the determinism check.

## External reference

To compare against a hardware/plugin reference clip, render with matching program/MIDI via `OfflineRender`, then:

```bash
./build/SpectralDiff your_reference.wav /tmp/jdupg-render.wav
```
