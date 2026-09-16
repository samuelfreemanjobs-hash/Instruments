# Offline tools — architecture

Command-line binaries built from `tools/` and registered in the root `CMakeLists.txt`. They support ROM generation, headless audio capture, and regression comparison. They are **not** loaded by the DAW; they link JUCE (and `OfflineRender` links the plugin target).

## Targets

| Target | Source | Role |
|--------|--------|------|
| `GenerateCleanroomRom` | `GenerateCleanroomRom.cpp` | Synthesize `jdupg_cleanroom.rom` at build time (256 waves, multisample metadata). |
| `OfflineRender` | `OfflineRender.cpp` | Instantiate `JDUpgradedAudioProcessor`, feed MIDI, write stereo 24-bit WAV. |
| `SpectralDiff` | `SpectralDiff.cpp`, `WavCompare.h` | Peak-normalized mono comparison; RMS and mean spectral bin error. |

## GenerateCleanroomRom

- **Input:** output path (CLI arg).
- **Output:** binary ROM consumed by `JDUpgradedRomData` (embedded in plugin).
- **Logic:** Procedural waveforms only — no external samples. See [docs/ROM.md](../docs/ROM.md).
- **Build hook:** `add_custom_command` runs before `juce_add_binary_data`.

## OfflineRender

```
argv → createPluginFilter() → prepareToPlay → setCurrentProgram
     → loop processBlock(blockSize=512) with note on/off MIDI
     → stereo buffer → WAV writer (24-bit)
```

- **Defaults:** program `0`, note `60`, velocity `100`, `2.0` s, `44100` Hz.
- **Dependencies:** Full `JDUpgraded` target + ROM binary data (same as plugin).
- **Use:** CI smoke, golden reference generation, manual A/B ([AB_HARNESS.md](../docs/AB_HARNESS.md)).

## SpectralDiff

- Loads two WAVs via JUCE `AudioFormatReader`.
- Resamples to common length / mono mix in `WavCompare.h`.
- Computes RMS difference (dB) and mean per-bin spectral magnitude error (dB).
- Exits non-zero if thresholds exceeded (`--max-rms-db`, `--max-spectral-db`).

## Golden references

Committed under `tests/golden/`. Regenerate after intentional DSP changes:

```bash
./build/OfflineRender tests/golden/program0-c4-0.5s.wav 0 60 100 0.5 44100
```

CI renders the same arguments and compares with `SpectralDiff` against the committed file.

## Extension points

- Add a new tool: `add_executable` in `CMakeLists.txt`, document it in this file and in root [ARCHITECTURE.md](../ARCHITECTURE.md).
- Shared WAV logic belongs in `WavCompare.h` (header-only) to keep `SpectralDiff` thin.
