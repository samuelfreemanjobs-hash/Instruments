# TRAP LORD SYNTH — Phase 2 expansion plan (Sam)

Neutral naming only: no artist, producer, or trademark strings in presets, tags, UI, or docs.

## Architecture

```
UI (APVTS) ──► buildPatch() + macros ──► TrapVoice×16 (DSP)
                    │                         │
                    ├─ Chorus / Delay / Reverb / Rotary (bus)
                    │
PresetLibrary ◄── BinaryData(JSON) ◄── tools/generate_preset_bank.py
                    │
KeygroupExportWorker (background thread) ──► Offline render ──► .WAV + .xpm folder
```

- **Realtime path:** `processBlock` reads atomics only; no file I/O, no locks, no allocations.
- **Preset load:** message thread (`applyPreset`, file browser); bank JSON loaded once at startup.
- **MPC export:** `suspendProcessing(true)` + worker `Thread` renders offline; UI progress via `AsyncUpdater`.

## Engines (synthesis)

| ID | Name | Use |
|----|------|-----|
| 0 | Brass | Detuned PolyBLEP saws |
| 1 | Organ | Additive drawbars + vibrato |
| 2 | Strings | Layered saws |
| 3 | FM Bell | Classic FM |
| 4 | Bass | Sine + triangle |
| 5 | Choir | Harmonic stack |
| 6 | Lead | Saw + triangle |
| 7 | Multisample | User WAV map |
| 8 | Analog Pulse | Variable width + blep |
| 9 | Bright EP | FM tine |
| 10 | Haunted FM | Inharmonic metallic |

## Macro → parameter mapping

| Macro | Voice / bus targets |
|-------|---------------------|
| **Darkness** | Cutoff ×0.42–1, resonance +0.12, reverb damping ↑ |
| **Dirt** | Drive +0–2.8 |
| **Movement** | Detune +0–6 ct, vibrato depth, chorus mix/depth, rotary depth (organ + macro) |
| **Space** | Reverb wet +0–0.35, delay mix +0–0.28 |

Macros stack on top of per-preset base values from the library.

## Preset system (500)

- **Storage:** `assets/factory/preset_bank.json` embedded via `juce_add_binary_data` (regenerate with `tools/generate_preset_bank.py`).
- **Categories (9):** Brass, Strings, Keys, Organ, Bells, Leads, Bass, Pads/Choir, Hits/FX.
- **Generation:** Seeded PRNG from six curated base recipes + constrained random offsets; forbidden-name filter; near-duplicate rejection (quantized param fingerprint); parameter sanity clamps.
- **Validation (generate time):** min/max envelope, gain, drive; reject empty names / forbidden tokens.
- **Loudness (optional offline):** `tools/render_preset_auditions.py` renders sample set after build; peak gate in generator targets −18 to −8 dBFS nominal via gain cap.
- **UI browser:** category filter, text search, prev/next, index label `n/500`.

Legacy 18 ATL Classics entries remain as seed recipes inside the generator (not a separate runtime list).

## MPC keygroup export

**Output layout (one folder per program):**

```
<parent>/<SafeProgramName>/
  <SafeProgramName>.xpm
  TL_<root>_<velLo>-<velHi>.WAV   (MPC-safe uppercase .WAV)
```

**Schema (MPC 2.x keygroup .xpm, aligned with ConvertWithMoss / MPC Software):**

- Root `KeygroupProgram` → `Version/FileVersion` → `Program` with `ProgramType=keygroup`, `Name`, `NumKeygroups`.
- `Instruments/Instrument` per MIDI zone: `LowNote`, `HighNote`, amp envelope defaults.
- `Layers/Layer` per velocity layer: `SampleName`, `RootNote` (MIDI root + 1), `VelStart`, `VelEnd`, `KeyTrack`, slice length fields.

**Render options (defaults):**

| Option | Default |
|--------|---------|
| Note range | C1–C6 (24–84) |
| Step | 3 semitones |
| Velocity layers | 2 (soft / hard) |
| Length | 2.0 s + release tail |
| Sample rate | 44100 Hz |
| Bit depth | 24 (optional 16) |

**Threading:** `KeygroupExportWorker` runs on `juce::Thread`; processor exposes `renderKeygroupOffline()` only while suspended.

**Hardware status:** Not verified on MPC One/Live/X/Key hardware — software schema match + automated XML/WAV validator only.

## Test plan

1. Linux Release build: VST3 + Standalone.
2. `pluginval --strictness-level 5` on VST3 (post-change).
3. `python3 tests/test_mpc_xpm_export.py` — export fixture folder, validate XSD-like checklist + WAV headers.
4. `python3 tools/validate_keygroup.py` — legacy JSON path still OK for multisample import.
5. `python3 tools/generate_preset_bank.py --check` — 500 presets, category counts, forbidden-word scan.
6. Manual: browse presets, export keygroup, import `.xpm` folder in MPC Software (operator).

## Milestones

| ID | Deliverable | Done when |
|----|-------------|-----------|
| M2a | `docs/PLAN.md` on PR | Done |
| M2b | 500-preset library + browser | Done — BinaryData + search/filter/prev/next |
| M2c | Native `.xpm` export + worker UI | Done — **not hardware verified** |
| M2d | Audition manifest script | Done — `audition_manifest.json` (9 categories); WAV via Export Keygroup |
| M2e | pluginval s5 pass | Done (Linux Cloud agent) |

## Risks

- MPC v3 `.xty` keygroups differ from `.xpm`; we target MPC2-compatible `.xpm` + uppercase `.WAV`.
- 500-preset JSON size (~400 KB) acceptable in BinaryData; regen required when schema changes.
- Offline export duration scales with zone count × velocity layers (UI progress required).
