# Kit spec — Tail Light Static (W1)

**Intake ref:** `../../INTAKE.md`  
**Generated:** 2026-11-17 · DRUMSMITH W1 (repo land)  
**Ship status:** **NOT READY** (no renders; device exports UNVERIFIED)

## Concept

Drift-phonk one-shot + loop pack for night-drive beats: long 808 tail, iron cowbell accent, papery snare, dusty hats. Master grid **74 BPM**; bar = **143,027 samples** @ 44.1 kHz — `loop_samples(1, 74, 44100)` in `scripts/dsp_math.py` `[computed]`.

## Hit manifest

| Voice | Count | Naming | Notes |
|-------|------:|--------|-------|
| Kick | 5 | `TLS_Kick_0x.wav` | Pitch sweep + click `[A]` |
| 808 | 5 | `TLS_808_0x.wav` | Glide decay 1.2–2.8 s `[A]` |
| Snare | 4 | `TLS_Snare_0x.wav` | Tonal body + noise `[S]` MemphisPhonkFactory brief |
| Clap | 3 | `TLS_Clap_0x.wav` | Stack 2–3 ms offsets `[A]` |
| CH | 6 | `TLS_CH_0x.wav` | 808 metal `[S]` |
| OH | 4 | `TLS_OH_0x.wav` | Long tail for drift sustain |
| Rim | 3 | `TLS_Rim_0x.wav` | |
| Cowbell | 4 | `TLS_Cow_0x.wav` | F5 detuned square `[A]` |
| Perc | 3 | `TLS_Perc_0x.wav` | Filtered noise hits |
| Loops | 2 | `TLS_Loop_Main_2bar_74bpm.wav`, `TLS_Top_1bar_74bpm.wav` | Kick in main only |

`[S]` sourced from repo creative brief / drum-engine patterns. `[A]` agent preset — tune by ear.

## Folder tree

```text
tail-light-static/
  README.txt
  KITS/tail-light-static/
    Kicks/
    808/
    Snares/
    Claps/
    Hats/
    Cowbells/
    Perc/
    Loops/
  docs/provenance/tail-light-static.md
  device-exports/   # empty until profiles verified
```

## DSP bus chain (render)

1. Voice synth @ 44.1 kHz mono  
2. DC blocker (5 Hz one-pole) `[S]`  
3. Soft clip ceiling **−1.0 dBFS** `[S]` verify_kit contract  
4. 8-sample micro-fade in/out `[S]`  
5. Optional tape hiss layer −24 dBFS on loops only `[A]`

## Real-time rules (FORGE handoff)

- No `malloc` on audio thread; max voice block 512 `[S]`  
- Anti-aliasing: polyBLEP on discontinuities / oversample ×2 on nonlinear stages `[S]` drum-engine skill

## Device export plan (UNVERIFIED)

| Device | Path hint | Status |
|--------|-----------|--------|
| FL Studio | `device-exports/flstudio/*.wav` + `.nfo` | UNVERIFIED |
| MPC 500 | 16-bit mono slices, 99 char names | UNVERIFIED |
| MPC Sample | `.xpm` map pending slicer | UNVERIFIED |
| Maschine+ | `Maschine/` tree | UNVERIFIED |
| MV-1 | 44.1 mono, short names | UNVERIFIED |
| Lofi12 | kit JSON + samples | UNVERIFIED |

**Hard halt:** Do not mark storefront “ready” until Sam confirms maps against `disklordz_slicer.py` (path TBD in monorepo) or manual MPC proof.

## Provenance

Template: `docs/provenance/tail-light-static.md` (create on first render batch).

## Verification plan (W3)

```bash
python3 claude/drumsmith/scripts/verify_kit.py KITS/tail-light-static --bpm 74 --json
```

CALIPER must echo PASS before ship copy.

## Open questions

See INTAKE row 13. v1 scope is **74 BPM only** unless Sam overrides.