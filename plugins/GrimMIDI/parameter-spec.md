# Grim-MIDI — parameter spec v1.1

| ID | Type | Range | Default | Notes |
|----|------|-------|---------|-------|
| `root_key` | int | 0–11 | 8 (Ab) | Pitch class for degree math |
| `scale` | choice | 4 scales | Natural Minor | Nat / Harm / Phrygian / Phryg Dominant |
| `chord_type` | choice | 8 qualities | Minor 7 | See `GrimChordEngine::ChordQuality` |
| `inversion` | int | 0–3 | 0 | |
| `spread` | float | 0–100 | 45 | Voicing width |
| `strum_ms` | float | 0–120 | 22 | Sample-accurate note-ons |
| `velocity_humanize` | int | 0–40 | 12 | Deterministic LCG |
| `octave` | int | −2–2 | 0 | Applied in trigger resolver |
| `trigger_mode` | choice | 3 | Degree Pads | Nearest / Degree Pads / Chromatic Root |

## Programs

- `getNumPrograms()` = **100**
- Each program loads `scale`, `chord_type`, and optional `root_key` from `ProgressionPreset`.

## MIDI safety

- Parameter changes flush active chord notes.
- CC 123 / all-notes-off honored.
- Transport not playing → release all (configurable in future).
