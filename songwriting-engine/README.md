# Antigravity Songwriting Engine

An isolated MIDI-first composition engine, developed alongside the DiskLordz 808 Lab and MPC500 tooling in this repo. It currently provides a deterministic four-track generator, canonical JSON projects, chord voicing selection, and Type 0/Type 1 MIDI export with re-import validation.

## Quick start on Windows

From this directory in PowerShell (the `-ExecutionPolicy Bypass` applies only to this process):

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup.ps1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\test.ps1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\generate-example.ps1
```

Setup installs the engine into the installed Python 3.12+ environment from `requirements.lock`, performs a hash-checked user Rustup installation if needed, and installs locked npm dependencies with `npm ci`. Cargo dependencies are pinned by `src-tauri/Cargo.lock`. The development app and Windows package commands are `scripts/dev.ps1` and `scripts/build-windows.ps1` respectively. The generated sample is written under `artifacts/` as project JSON, two MIDI files, and a JSON validation report.

## CLI

```powershell
python -m songwriting.cli check
python -m songwriting.cli generate --config .\configs\smoker_sketch.json --output-dir .\artifacts
python -m songwriting.cli generate-prompt --prompt "95 BPM, D minor, smoky West Coast. Chords: Dm9, Gm9, Bbmaj7, A7. 32 bars." --seed 48127 --output-dir .\artifacts
python -m songwriting.cli validate-midi .\artifacts\Smoker_Sketch_001_Type1.mid --project .\artifacts\Smoker_Sketch_001.json --type 1
```

Musical time is stored in quarter-note beats. MIDI ticks are introduced only at export using the project's PPQ setting. Unsupported chord symbols fail with an explicit error rather than silently falling back.

The desktop's optional **Creative Brief** field accepts tempo, key/mode, style cues, a `Chords:` progression, song length in bars, and an optional `seed` value. For example: `95 BPM, D minor, smoky West Coast. Chords: Dm9, Gm9, Bbmaj7, A7. 32 bars.` The same parser is available through `generate-prompt` and does not call a remote model.

## MPC500 export

```powershell
python -m songwriting.cli export-mpc500 --project .\artifacts\Smoker_Sketch_001.json --out E:\SONGS\WCS95D
# or in one step: generate-prompt ... --mpc500-dir E:\SONGS\WCS95D
```

Writes a folder for the MPC500 CF card, re-timed to the MPC500's 96 PPQ with same-pitch overlaps trimmed and every file re-validated:

| File | Use |
|------|-----|
| `<CODE>_SONG.MID` | Whole arrangement, Type 1, one track per part |
| `<CODE>_S01INT.MID` … | One sequence per section, for MPC Song mode in the listed order |
| `<CODE>_PAD.MID`, `_EP`, `_BAS`, `_LD` | Single-track Type 0 files; set the MPC track's MIDI out to the listed channel to drive an external synth |
| `SONG.TXT`, `mpc500-export.json` | Song-mode order, channels, instrument hints |

`<CODE>` is style initials + BPM + key (e.g. `WCS95D`), so names stay within the MPC500's 16 characters. Whether the MPC500 loads Type 1 files as multi-track sequences has not been checked on hardware; if it flattens them, use the per-track Type 0 files.

## Current limits

The current release slice includes a deterministic brief parser, D natural minor and the listed scales, a conservative chord-symbol grammar, full chord-tone voicings with range limits, minimum-movement selection, style-dependent groove densities, scale-locked lead notes, a React/Tauri workbench, and a PyInstaller sidecar for Windows release builds. Gospel passing-chord transformations, MIDI import into the editable project model, a note-editing piano roll, audio synthesis, and verified MPC compatibility remain later phases. The MIDI has been structurally checked by reopening it with `mido`; musical quality still requires listening in a target DAW/device.
