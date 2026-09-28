# Lofi-12 step sequencer (computer → MIDI)

## Purpose

**6-track × 16-step** editor in the browser with **Web Audio** preview, **live pads**, **factory backing loops**, and **Web MIDI** to the **LIVEN Lofi-12** (tracks 1–4 = hardware; Perc/OpenHat = extra lanes).

## Build & run

```bash
# Browser UI (Chrome / Edge recommended for Web MIDI)
python3 disklordz/lofi12-phonk-factory/sequencer/serve.py
# Open http://127.0.0.1:8765 — select MIDI OUT → Lofi-12 MIDI IN

# Headless / Linux MIDI (optional: pip install mido python-rtmidi)
python3 disklordz/lofi12-phonk-factory/sequencer/scripts/midi_play.py \
  --pattern my_pattern.json --port "Lofi-12"
```

Import Memphis groove from factory:

```bash
python3 disklordz/lofi12-phonk-factory/sequencer/scripts/pattern_from_groove.py \
  --prompt "dj paul memphis 84" --out my_pattern.json
```

## Data flow

```text
Browser grid → pattern JSON (save/load)
  → Web MIDI note on/off + optional clock → Lofi-12 MIDI IN
Python midi_play.py → same JSON → mido port
phonk_groove.build_bar → pattern_from_groove → JSON → sequencer UI
```

## Threading / realtime

Browser `setInterval` / audio clock for step advance; MIDI fire-and-forget. Python player uses monotonic sleep per step.

## Key modules

| Path | Role |
|------|------|
| `static/index.html` | UI shell |
| `static/app.js` | Grid, transport, Web MIDI, persistence |
| `static/style.css` | Layout |
| `serve.py` | Static file server |
| `scripts/midi_play.py` | CLI playback via mido |
| `scripts/pattern_from_groove.py` | Groove → pattern JSON |
| `scripts/pattern_schema.py` | Validate / note ↔ slot map |

## Extension points

- SysEx pattern export to Lofi-12 native format (when documented)
- Per-track MIDI channel matching Lofi track auto-channel
- CC lanes for filter / laid-back from `midi.guide` CC map

## Related docs

- [../docs/PATTERN_GUIDE.md](../docs/PATTERN_GUIDE.md)
- [../docs/PHONK_PROGRAMMING_PLAYBOOK.md](../docs/PHONK_PROGRAMMING_PLAYBOOK.md)
