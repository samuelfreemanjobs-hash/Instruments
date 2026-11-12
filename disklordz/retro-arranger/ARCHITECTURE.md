# Retro Arranger — 1980s session band MIDI

## Purpose

Multi-agent **1980s retro-modern** arranger: Python session band (`arranger`, `keys`, `bass`, `drum`, `lead`, `guitar`) → isolated GM MIDI stems + Type 1 multitrack, served via FastAPI with **DAW drag-and-drop** UI.

## Build & run

```bash
cd disklordz/retro-arranger
pip install -r requirements.txt
python scripts/smoke.py
python scripts/export_stems.py --out ./out
python app.py   # http://127.0.0.1:8790
```

## Data flow

```text
ArrangementSpec → pipeline.compose()
  → agents/*.py (note events + CC)
  → midi_writer (mido Type 0 stems + Type 1 full)
  → FastAPI /api/generate → static UI (DownloadURL drag)
```

## Key modules

| Path | Agent / role |
|------|----------------|
| `agents/arranger_agent.py` | Form, progressions, rootless voicings |
| `agents/keys_agent.py` | Rhodes, pad, stabs |
| `agents/bass_agent.py` | Monophonic synth / electric bass |
| `agents/drum_agent.py` | GM Linn/808 map, swing + humanize |
| `agents/guitar_agent.py` | 16th funk comp |
| `agents/lead_agent.py` | Solo call-and-response |
| `midi_writer.py` | note_on/off, program change, CC |
| `app.py` + `static/index.html` | Web dashboard |

## Threading

Single-process FastAPI; scale batch export via CLI workers, not shared state.

## Extension points

- Web Audio preview / soundfont player in UI
- Per-agent LLM arrangement overrides
- Vaporwave tempo/key macros

## Related

- [disklordz/ARCHITECTURE.md](../ARCHITECTURE.md)
