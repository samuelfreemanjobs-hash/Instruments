# Offline tools — architecture

Command-line binaries built from `tools/` and registered in the root `CMakeLists.txt`. They support ROM generation, headless audio capture, and regression comparison. They are **not** loaded by the DAW; they link JUCE (and `OfflineRender` links the plugin target).

**Python (not CMake):** [rompler-factory/](rompler-factory/) — ASR-class wave batches and third-party library fetch helpers for [content/asr-class-v0/](../content/asr-class-v0/).

## Targets

| Target | Source | Role |
|--------|--------|------|
| `GenerateCleanroomRom` | `GenerateCleanroomRom.cpp` | Synthesize `jdupg_cleanroom.rom` at build time (256 waves, multisample metadata). |
| `OfflineRender` | `OfflineRender.cpp` | Instantiate `JDUpgradedAudioProcessor`, feed MIDI, write stereo 24-bit WAV. |
| `SpectralDiff` | `SpectralDiff.cpp`, `WavCompare.h` | Peak-normalized mono comparison; RMS and mean spectral bin error. |
| `ExportPreset` | `ExportPreset.cpp` | Write `.jdpreset` APVTS XML for a factory program index. |

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

Committed under `tests/golden/` with **`manifest.tsv`** (program, note, duration). Scripts:

- `tests/golden/refresh_golden.sh` — rewrite all golden WAVs via `OfflineRender`
- `tests/golden/verify_golden.sh` — CI/local regression (`SpectralDiff` per manifest row)

## Headless VST3 validation (pluginval)

[`scripts/vst/run_pluginval.py`](../scripts/vst/run_pluginval.py) downloads Tracktion **pluginval** v1.0.4 (cached under `build/tools/pluginval/`), runs strict in-process fuzz tests on built `.vst3` bundles, and prints a failure tail for agents/CI.

```bash
cmake --build build -j --target JDUpgraded_VST3
python3 scripts/vst/run_pluginval.py --plugin "build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3"
# or validate all default monorepo artefacts (JD Upgraded, Wave909 when built):
python3 scripts/vst/run_pluginval.py --default-artefacts
# watch build output after incremental compiles:
python3 scripts/vst/run_pluginval.py --watch build/JDUpgraded_artefacts/Release/VST3
```

Environment overrides: `PLUGINVAL_BIN`, `PLUGINVAL_DOWNLOAD_URL`, `PLUGINVAL_CACHE_DIR`.

CI: [`.github/workflows/build.yml`](../.github/workflows/build.yml) runs the script after golden WAV verification.

Local **command center** (drop-in folder + AI-friendly `error_log.txt`): [vst-testing-ops/](../vst-testing-ops/) — `python3 vst-testing-ops/test_runner.py`.

## Extension points

- Add a new tool: `add_executable` in `CMakeLists.txt`, document it in this file and in root [ARCHITECTURE.md](../ARCHITECTURE.md).
- Shared WAV logic belongs in `WavCompare.h` (header-only) to keep `SpectralDiff` thin.
