# Disklordz drum engine

## Purpose

Shared drum voices for a VST and for a headless one-shot exporter. The same headers render inside a future plugin and in `DrumBatchExporter`. Producers get 24-bit / 48 kHz kicks, snares, claps, and hats without opening a DAW.

## Build & run

```bash
cmake -S disklordz/drum-engine -B disklordz/drum-engine/build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build disklordz/drum-engine/build --target DrumEngineTests DrumBatchExporter
ctest --test-dir disklordz/drum-engine/build --output-on-failure
./disklordz/drum-engine/build/DrumBatchExporter --out /tmp/dlz-kit --voice kick --count 2
python3 disklordz/drum-engine/python/test_workflows.py
```

JUCE comes from `build/_deps/juce-src` when the root build has already fetched 8.0.15. Otherwise this CMake fetches that tag. The project enables C and CXX because JUCE requires a C compiler.

## Data flow

```text
trigger(params) → voice.process(buffer) → 4x oversampled tanh
              → DrumBatchExporter trims, fades, peak-normalizes, writes 24-bit WAV
              → process_kit.py / visualize_wav.py / stitch_preview.py
```

Python kit tools read and write WAV with the stdlib. ffmpeg is used only for the mp3 preview.

## Threading / realtime

`prepare()` sizes buffers and calls `Oversampling::initProcessing`. `process()` does not allocate or lock. `LicenseManager::allowsProcessing()` is an atomic load. Activation and `PropertiesFile` access run on a background thread.

## Key modules

| Path | Role |
|------|------|
| `include/Kick808Voice.h` | Phase-reset 808 with pitch envelope and click |
| `include/SnareVoice.h` | Body, overtone, bandpass noise |
| `include/ClapVoice.h` | Four bursts plus a noise tail |
| `include/HiHatVoice.h` | Six-square 808 hat at the oversampled rate |
| `include/OversampledWaveShaper.h` | 4x oversampling (JUCE factor 2) |
| `include/LicenseManager.h` | Offline grace, off the audio thread |
| `include/MidiMaps.h` | GM drum notes |
| `src/DrumBatchExporter.cpp` | CLI WAV writer |
| `python/process_kit.py` | Folder mastering and zip |
| `packaging/` | Inno Setup script and macOS notarize script |

## Extension points

Add a voice header, a `MidiMaps.h` row, a `params.json` id, and a `DrumEngineTests` case. New commercial sample rates and filenames follow `.cursor/rules/sample-specs.mdc`.

## Related docs

- [docs/CURSOR_AUDIO_WORKSTATION.md](../../docs/CURSOR_AUDIO_WORKSTATION.md)
- [disklordz/sound-factory/ARCHITECTURE.md](../sound-factory/ARCHITECTURE.md)
- [disklordz/website/ARCHITECTURE.md](../website/ARCHITECTURE.md)
