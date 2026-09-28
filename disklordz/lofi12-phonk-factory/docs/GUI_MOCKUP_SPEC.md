# GUI mockup spec (align your design here)

Use this when sketching the factory + sequencer UI. Current implementation follows **v0.2** below; your mockup can rename or rearrange—keep the **capabilities** so we can wire it 1:1.

## Layout (recommended)

```text
┌─────────────────────────────────────────────────────────────┐
│  Transport: BPM · Play/Stop · MIDI out                      │
├─────────────────────────────────────────────────────────────┤
│  Factory: Style preset · Prompt · [New drum loop] [New sample pack] │
├─────────────────────────────────────────────────────────────┤
│  FX: Filter · Reverb · Drive · Wobble · Cassette · Bit crush │
├─────────────────────────────────────────────────────────────┤
│  Track tabs: Kick | Snare | Hat | Cowbell | Perc | OpenHat | All │
│  ┌─ Per-track 16-step sequencer (one track focused) ─────┐  │
│  │  Step editor · [Export this instrument loop WAV]      │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  Live pads · Backing loop · Download                        │
└─────────────────────────────────────────────────────────────┘
```

## Must-have behaviors

| Control | Behavior |
|---------|----------|
| **Per-track tab** | Shows **only that track’s** 16 steps (large cells) + step note/velocity editor |
| **All tracks** | 6×16 overview grid (existing) |
| **New drum loop** | New variation seed, full mix WAV, optional playback |
| **New sample pack** | ZIP of 16-slot phonk bank (24 kHz mono WAVs + manifest) |
| **Export instrument loop** | WAV for **one** stem (factory groove **or** current grid for that track) |
| **Cassette FX** | Wow/flutter + muffled band + hiss (post-drive) |
| **Bit crusher** | Sample-rate / bit-depth reduction (SP-1200 adjacent) |

## API (already implemented for mockup wiring)

| Method | Path | Notes |
|--------|------|--------|
| GET | `/api/presets` | Style list |
| POST | `/api/render_loop` | `{ prompt, bpm, variation, fx, stem?, preset? }` → WAV |
| POST | `/api/render_pattern_stem` | `{ pattern, trackIndex, fx }` → WAV from grid |
| POST | `/api/generate_bank` | `{ prompt, preset?, variation }` → ZIP |
| POST | `/api/groove_pattern` | Fill 6-track JSON |

## Hardware mapping

- Lofi-12 **4** hardware sequencer tracks = Kick, Snare, Hat, Cowbell.
- **Perc / OpenHat** = extra MIDI lanes or second bank on computer.

## After your mockup

Drop a PNG/Figma link in the issue or `references/gui/` and note:

1. Tab order and colors  
2. Which FX are per-track vs global  
3. Whether “generate” replaces backing automatically  

We’ll match spacing/labels in `sequencer/static/` without changing the API contract above.
