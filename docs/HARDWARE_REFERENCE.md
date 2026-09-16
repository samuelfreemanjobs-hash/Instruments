# Hardware / external reference WAVs

Optional comparison against clips you own (hardware JD, another plugin, etc.). **Not** run in CI by default — only committed plugin goldens are gated in CI.

## Compare a reference file

1. Render the same program/MIDI with `OfflineRender`:

```bash
./build/OfflineRender /tmp/render.wav 48 40 100 0.5 44100
```

2. Compare to your reference (peak-normalized spectral diff):

```bash
./build/SpectralDiff /path/to/your_reference.wav /tmp/render.wav --max-rms-db -20 --max-spectral-db 3
```

Loosen thresholds for hardware vs model differences.

## Optional local hook

Place a reference at `tests/reference/program48-e2.wav` and run:

```bash
./tests/golden/compare_external.sh
```

The script skips missing files and does not fail CI.
