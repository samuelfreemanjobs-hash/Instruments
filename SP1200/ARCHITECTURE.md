# SP-1200 Drumulator — architecture

Standalone **12-bit / 26.040 kHz** sampler inspired by the E-mu SP-1200 workflow. Product spec: [docs/SP1200_STANDALONE_SPEC.md](../docs/SP1200_STANDALONE_SPEC.md).

## Purpose

Performance sampler with **16 pads**, **16 voices**, **7:00** embedded sample RAM, WAV import, audio input record, **SQ-1 default MIDI map** (ch 10, notes 36–51, CC 20–35), keyboard transport (Space/Esc/R/Tab, Q–I / A–K pads).

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target SP1200_Standalone SP1200_VST3 SP1200MemoryTests SP1200SequencerTests SP1200ProjectFileTests
ctest --test-dir build -R SP1200
```

Artifacts:
- Standalone: `build/SP1200/SP1200_artefacts/Release/Standalone/SP-1200 Drumulator`
- VST3: `build/SP1200/SP1200_artefacts/Release/VST3/SP-1200 Drumulator.vst3`

## Data flow

```text
WAV / input → SampleImporter (mono sum, resample) → TwelveBitBuffer → SampleMemoryPool
MIDI / UI pad → SamplerEngine → 16× SampleVoice (drop-sample pitch) → Ssm2044BusFilter → stereo out
```

## Threading

| Thread | Work |
|--------|------|
| Audio | `processBlock`, voice render, input record append — **no alloc** |
| Message | Editor, file import, chooser |
| Background | Future: transient chop, project encode |

## Key modules

| Path | Responsibility |
|------|----------------|
| `Source/SP1200Constants.h` | 26.040 kHz, 7:00 cap, pad count |
| `Source/Memory/*` | 12-bit buffers, RAM pool |
| `Source/Import/SampleImporter.*` | File/buffer → SP format |
| `Source/Engine/SamplerEngine.*` | MIDI, voices, record |
| `Source/PluginProcessor.*` | JUCE standalone shell |
| `Source/PluginEditor.*` | Console, MOD 11 chop overlay, save/load |
| `Source/UI/ChopModalComponent.*` | MOD 11 truncate / auto-chop / normalize |
| `Source/Project/ProjectFile.*` | `.sp12p` embedded samples + patterns |
| `Source/DSP/Ssm2044BusFilter.*` | Bus low-pass + per-step cutoff mod |

## MIDI & keyboard (defaults)

| Source | Mapping |
|--------|---------|
| SQ-1 row A/B | Ch **10**, notes **36–51** → pads 1–16 |
| SQ-1 knobs | Ch **10**, CC **20–35** → faders (VOL / PITCH / DECAY mode) |
| Keyboard | **Space** play, **Esc** stop, **R** step record (seq view), **Tab** fader mode, **Q–I / A–K** pads |

## Song (Module 24)

- Eight visible chain slots (32 stored); **END** (`kSongSlotEnd`) stops playback; optional **loop** wraps slot 0–7.

## Choke & filter roles

- Pads **5–6** and **13–14** default to hat **choke group** 1 (retrigger cuts siblings).
- Pads **3–4** and **11–12** use per-voice **hi-trim** before bus SSM2044 (`PadFilterRoles.h`).

## Module 10 SETUP tab

- MIDI channel (1–16), **omni**, **clock** internal / slave / master
- **Learn NOTE / Learn CC** per pad; reset to SQ-1 defaults
- **Choke group** per pad (None + groups 1–4)
- Persisted in project **v4** (maps + vinyl toggle)

## Module 30 (special)

- Console **VINYL 33→45 import**: import at 45/33 rate, auto **~−5 st** pad tune on assign
- **COMBINE**: select pad A → **MOD 30 COMBINE** → pick pad B; appends B onto A segment and removes B from pool (`SegmentEditor::combineSegments`)
- **SEG → BANK**: sets selected pad segment `SampleSegment::bank` to active bank A–D
- **KeypadComponent**: 0–9 + ENTER/CANCEL; pattern entry on **MOD 20**, bank entry on console (keyboard digits also wired)

## Timing (96 PPQN + SP swing)

- Sequencer grid **96 PPQN**; each 1/16 step = **24 ticks** at swing 0.
- Swing applies **±12 tick** shift on even/odd sixteenths (pair sum unchanged); internal `advance()` and MIDI **slave** clock use per-step pulse counts (3/9 clocks at max swing vs 6/6 flat).

## MOD 11 WAVE CHOP tab

- **11 CHOP** tab embeds the chop/truncate editor (same as console overlay **MOD 11 CHOP**).
- Console button still opens full-screen overlay with **Close**.

## Module 12–14 PROGRAM tab

- **12–14 PROG** tab: soft keys **12 PITCH**, **13 DECAY**, **14 MIX**; 16 faders + pads (same grid as console).
- Faders edit all pads for the active module; LCD scrub targets selected pad (±12 st tune @ 0.1 st steps).
- **Shift+Tab** cycles pitch → decay → mix while on program tab.

## Module 15 FILTER tab

- **15 SSM2044** tab: bus cutoff/resonance sliders, pad hi-trim vs flat role readout, LCD scrub (**±1**, **YES·EXEC** / **NO·BACK**).
- Console bus SSM controls moved here (console keeps fader/multi-pitch row only).

## Banks & LCD

- **BANK A–D** selects import/record target (`SampleSegment::bank`); display quota **1:45/bank** on top of **7:00** global pool.
- **LcdPanelComponent** + **KeypadComponent** + scrub row; `LcdEditField` drives staged edits (Shift+Tab cycles field on filter/seq).
- **Clear pattern** requires LCD confirm (YES·EXEC).

## QA

- [docs/SP1200_QA_CHECKLIST.md](../docs/SP1200_QA_CHECKLIST.md)

## Extension points

- Full MIDI Learn UI for transport; device picker; 96 PPQN micro-timing
- Multi-pitch / chromatic cap via `quantizeToMultiPitch`

## Related

- [ARCHITECTURE.md](../ARCHITECTURE.md) (repo index)
