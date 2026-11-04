# MPC 500 staging tools

Offline helpers for **manual Compact Flash copy** (stock OS). Hardware runs on the Akai; this tree only prepares folders and manifests on the computer.

## Purpose

Map Disklordz or DAW-exported WAVs onto **lane pad maps** (`mpc500/lanes/*/pad-map.json`) and emit a CF-shaped directory tree documented in [docs/MPC500_CF_LAYOUT.md](../../docs/MPC500_CF_LAYOUT.md).

## Build & run

```bash
python3 tools/mpc500/stage_cf_inbox.py --lane phonk --from /path/to/kit/wavs --out ~/Music/Disklordz/MPC500_STAGING
python3 tools/mpc500/stage_cf_inbox.py --lane phonk --from /path/to/kit --out ~/staging --include-melodic --render-chords
python3 tools/mpc500/render_chord_stabs.py --out ~/staging/MPC500/BANK_A/PHONK/MELODIC/CHORDS
python3 tools/mpc500/fx_chain.py --in sample.wav --preset phonk_tape --out ~/staging/MPC500/BANK_A/PHONK/FX/TAPE/sample.WAV
python3 tools/mpc500/generate_melodic_midi.py --pattern phonk_bass_140 --out ~/staging/MPC500/BANK_A/PHONK/MELODIC/MIDI/PHONK140.MID
```

Melodic design: [docs/MPC500_MELODIC_FX.md](../../docs/MPC500_MELODIC_FX.md).

Exit code **2** if required pad filenames are still missing after alias matching.

## Data flow

```text
Kit WAV folder
  → stage_cf_inbox.py (regex aliases → 8.3 CF names)
  → MPC500/BANK_A/<LANE>/SAMPLES/*.WAV + manifest.json
  → user manual copy to Compact Flash
  → MPC LOAD / SAVE PGM+SEQ on hardware
```

## Key modules

| Path | Role |
|------|------|
| `stage_cf_inbox.py` | Lane staging CLI |
| `mpc500/lanes/phonk/pad-map.json` | 12-pad phonk map + MIDI notes |
| `docs/MPC500_CF_LAYOUT.md` | CF tree + MIDI master notes |

## Extension points

| Goal | Action |
|------|--------|
| New genre lane | Add `mpc500/lanes/<id>/pad-map.json` + aliases in script or JSON-driven aliases |
| Maid export | Optional second stage calling MPC Maid JAR |
| CF size budget | Add validator step before copy |

## Related

- [mpc500/ARCHITECTURE.md](../../mpc500/ARCHITECTURE.md)
- [disklordz/daw-inbox/ARCHITECTURE.md](../../disklordz/daw-inbox/ARCHITECTURE.md)
