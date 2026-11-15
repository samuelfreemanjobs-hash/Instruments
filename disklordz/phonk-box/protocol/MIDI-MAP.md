# Phonk Box MIDI map

**USB MIDI class device** on controller → **Windows** `phonk_engine.py` (or future standalone).

Default channel: **10** (GM drums). Engine accepts **omni** in Phase 0.

## Play mode (default)

| Pad | MIDI note | SparkLE pad | Role |
|-----|-----------|-------------|------|
| 1 | 36 (C1) | 1 | 808 sub |
| 2 | 37 | 2 | knock |
| 3 | 38 | 3 | snare body |
| 4 | 39 | 4 | snap/clap |
| 5 | 42 (F#1) | 5 | closed hat |
| 6 | 46 (A#1) | 6 | open hat |
| 7 | 49 (C#2) | 7 | rim/perc |
| 8 | 51 (D#2) | 8 | cowbell/FX |

Velocity 1–127 → sample gain.

## Sequencer mode (shift / mode button)

| Control | MIDI | Action |
|---------|------|--------|
| Step 1–16 | 48–63 | Toggle step for selected pad |
| Selected pad | CC 64 value 0–7 | Which row is being edited |
| Play | Note 120 | Start pattern |
| Stop | Note 121 | Stop |
| BPM | CC 1 | 60–200 |

## Sound params (808 Lab lanes)

| CC | Param | Maps to factory |
|----|-------|-----------------|
| 70 | kick pitch | `kickPitch` |
| 71 | kick decay | `kickDecay` |
| 72 | snare snap | `snareSnap` |
| 73 | hat bright | `hatBright` |
| 74 | lane preset | Program change 0–127 → future registry index |

## Clock

- **Internal:** engine BPM from CC 1  
- **Future:** MIDI clock in (24 PPQN) to sync DAW
