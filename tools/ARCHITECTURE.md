# Offline tools — architecture

Command-line binaries built from `tools/` and registered in the root `CMakeLists.txt`. They support ROM generation, headless audio capture, and regression comparison. They are **not** loaded by the DAW; they link JUCE (and `OfflineRender` links the plugin target).

## Targets

| Target | Source | Role |
|--------|--------|------|
| `GenerateCleanroomRom` | `GenerateCleanroomRom.cpp` | Synthesize `jdupg_cleanroom.rom` at build time (256 waves, multisample metadata). |
| `OfflineRender` | `OfflineRender.cpp` | Instantiate `JDUpgradedAudioProcessor`, feed MIDI, write stereo 24-bit WAV. |
| `Vst3OfflineRender` | `Vst3OfflineRender.cpp` | Load a `.vst3` bundle via JUCE VST3 host; same MIDI→WAV path as `OfflineRender` ([`HeadlessMidiRender.h`](HeadlessMidiRender.h)). |
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

## Vst3OfflineRender

```
argv → load .vst3 (JUCE VST3PluginFormat) → prepareToPlay → setCurrentProgram
     → shared HeadlessMidiRender loop → stereo 24-bit WAV
```

```bash
cmake --build build -j --target JDUpgraded_VST3 Vst3OfflineRender
./build/Vst3OfflineRender "build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3" /tmp/out.wav 0 60 100 2.0 44100
./tests/golden/compare_vst3_offline.sh   # optional parity vs in-process OfflineRender
```

- **Why:** Validates the **shipped VST3 wrapper** (parameter layout, bus config, factory) separately from linking `JDUpgraded` directly.
- **Build:** `add_dependencies(Vst3OfflineRender JDUpgraded_VST3)` so the bundle exists before the tool is used.

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
# or validate all default monorepo artefacts (JD Upgraded, Wave9090 when built):
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
