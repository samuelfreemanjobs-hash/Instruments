# MPC 500 — melodic layer, scale lock, FX & resampling

Extends the phonk lane with **16 chord triggers**, **scale-locked** bass/melody, and **bitcrush / saturation / filter / tape wobble** — tuned for **stock OS**, **12 physical pads**, **manual CF copy**, **MPC MIDI master**.

Machine data: [`mpc500/lanes/phonk/scale-lock.json`](../mpc500/lanes/phonk/scale-lock.json) · Chord map: [`chord-triggers.json`](../mpc500/lanes/phonk/chord-triggers.json) · Patterns: [`melodic-patterns.json`](../mpc500/lanes/phonk/melodic-patterns.json).

## 12 pads → 16 chord triggers

The MPC 500 has **12 pads**, not 16. v1 uses **pad banks** (A + B) on the same program or two linked programs:

| Bank | Pads | Chord slots |
|------|------|-------------|
| **A** | 1–12 | Chords **1–12** |
| **B** | 1–4 | Chords **13–16** |
| **B** | 5–12 | *(optional)* bass one-shots / FX hits |

On hardware: assign chord WAVs per [`chord-triggers.json`](../mpc500/lanes/phonk/chord-triggers.json), switch **pad bank** to reach 13–16. If your OS labels banks differently, keep the **chord index 1–16** as source of truth and remap once **[executed]**.

Drums stay in **BANK_A/PHONK** (see [MPC500_CF_LAYOUT.md](MPC500_CF_LAYOUT.md)). Melodic/chord content lives in **BANK_A/PHONK/MELODIC/** (and resampled variants under **FX/**).

## Scale lock (phonk default)

| Field | v1 value |
|-------|----------|
| Root | **C** |
| Scale | **Natural minor** (C D Eb F G Ab Bb) |
| Lock mode | **Render-time** — tools only emit notes/chords from scale; hardware uses pre-tuned samples |
| Alternate | **D minor**, **F minor** — change `root` in `scale-lock.json` and re-run render scripts |

**Scale lock on stock OS** is not a global DAW quantizer — you **commit** lock by loading samples or sequences already in key. Optional later: JUCE companion receiving MIDI and re-quantizing (see [mpc500/ARCHITECTURE.md](../mpc500/ARCHITECTURE.md)).

## CF layout (melodic + FX)

```text
MPC500/BANK_A/PHONK/
  SAMPLES/           ← drums (existing)
  MELODIC/
    CHORDS/          ← CH01.WAV … CH16.WAV (after render_chord_stabs.py)
    BASS/            ← BS01.WAV … (one-shots or short loops)
    MIDI/            ← *.MID from generate_melodic_midi.py (optional load / slave)
  FX/
    CRUSH/           ← bitcrushed resample targets
    TAPE/            ← wow/flutter renders
    WET/             ← saturated + filtered chains
  PGM/               ← PHONK_DRUM.PGM, PHONK_MEL.PGM, …
  SEQ/
```

## FX chain (offline → resample on MPC)

**Order for computer prep** (see `tools/mpc500/fx_chain.py`):

```text
input WAV
  → optional HPF (mud cut)
  → saturation (soft clip)
  → bitcrush / downsample
  → resonant LP/HP (phonk filter sweep)
  → tape wobble (wow/flutter + mild saturation)
  → normalize → write to FX/ or MELODIC/
```

**On the MPC (stock OS) — resampling workflow:**

1. Load **dry** sample on a pad; set filter / envelope if available per sample.
2. **Play** pad(s) + sequence; route **main out → line in** (or mix bus record if your rig allows).
3. **Record** new sample from input; **truncate**; save to **FX/** or replace pad.
4. Repeat for heavier crush — each generation adds grit (classic MPC “sample the sample”).

Document your routing (mono/stereo, input gain) in `manifest.json` after first pass **[executed]**.

## Basslines & melodic patterns

| Asset | Tool | Use |
|-------|------|-----|
| 16 chord stabs | `render_chord_stabs.py` | One WAV per chord index, scale-locked |
| Bass + motif MIDI | `generate_melodic_midi.py` | Drag to DAW slave or convert to MPC sequence manually |
| Pattern library | `melodic-patterns.json` | Step grids + note refs for vibe sessions |

Patterns assume **140 BPM**, **16 steps**; MPC master clock drives slaves.

## Tooling

```bash
# Chord WAVs (placeholder synth stabs — replace with your resamples)
python3 tools/mpc500/render_chord_stabs.py --out ~/Music/Disklordz/MPC500_STAGING/MPC500/BANK_A/PHONK/MELODIC/CHORDS

# FX variants of one sample
python3 tools/mpc500/fx_chain.py --in kick.wav --preset phonk_crush --out ~/.../FX/CRUSH/KICK808_CR.WAV

# MIDI bass + melody (scale-locked)
python3 tools/mpc500/generate_melodic_midi.py --pattern phonk_bass_140 --out ~/.../MELODIC/MIDI/PHONK140.MID

# Stage drums + melodic tree
python3 tools/mpc500/stage_cf_inbox.py --lane phonk --from kit/ --out staging/ --include-melodic
```

## Session recipe (vibe coding)

1. Render or record **CH01–CH16**; load bank **A** then **B** for 13–16.
2. Load **BS*** bass one-shots or sequence from MIDI reference.
3. Run **fx_chain** on 808 / cowbell / chords; **resample on MPC** for second-pass dirt.
4. Sequence drums on tracks; overdub **chord hits** on pads; MIDI out **bass** to monosynth if slaved.
5. **SAVE** PGM + SEQ; backup CF **BACKUP/**.

## Future (repo)

| Feature | Owner |
|---------|--------|
| 16-pad USB chord UI + scale lock | `dsp-vst-architect` JUCE helper |
| Disklordz phonk harmonic packs | `midnight-circuit` + factory metadata |
| SP1200 parallel lane | Hybrid clock from MPC master |
