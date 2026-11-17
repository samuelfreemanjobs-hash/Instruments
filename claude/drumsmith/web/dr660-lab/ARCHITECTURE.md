# DR-660 Memphis DSP Lab (browser)

## Purpose

Local **Web Audio** workbench for 90s Memphis-style drum workflow: DR-660–inspired **16-step sequencer** (lookahead + swing), live voice triggers, and stubs for cassette saturation / filter grit. Analysis and prototyping only — not a Boss ROM dump and not a commercial kit.

Who uses it: DiskLordz / DRUMSMITH agents and Sam, iterating groove feel before native kit renders.

## Build & run

No build step. From this directory:

```bash
python3 -m http.server 8770 --bind 127.0.0.1
# open http://127.0.0.1:8770/
```

Unit tests (Node, no browser):

```bash
node --test tests/sequencer_math.test.mjs
```

## Data flow

1. UI transport → `DR660Sequencer.start/stop`
2. Lookahead `setInterval` (~25 ms) schedules note events on `AudioContext` clock
3. Per step: swing offset on odd 0-based 16ths → `triggerSound(track, time, velocity)`
4. `DrumEngine` plays short synthesized one-shots (kick/snare/hat/cowbell/…) into master chain
5. Master chain stubs: filter → cassette waveshaper → destination

## Threading / realtime

Browser audio thread is owned by Web Audio. UI never calls `setTimeout` for *audio* timing — only for playhead paint. Keep `scheduleAheadTime` ≈ 100 ms.

## Key modules

| Path | Role |
|------|------|
| `index.html` | 4-section workspace shell |
| `js/Sequencer.js` | Lookahead clock, swing, pattern, velocity |
| `js/DrumEngine.js` | Lightweight synth one-shots |
| `js/MasterChain.js` | Filter + cassette stub nodes |
| `js/app.js` | Wire UI ↔ audio |
| `css/lab.css` | Console layout |
| `tests/sequencer_math.test.mjs` | Swing / 16th duration |

## Extension points

- Swap synth triggers for decoded WAV buffers (CNV masters / DR-660 refs when staged)
- Fill cassette `WaveShaperNode` curves and pre-emphasis (Gemini next step #1)
- MIDI import/export of archetype patterns from `kits/converter-night/loops/`

## Related docs

- `kits/converter-night/recipes/DR660_LOOPS.md`
- `kits/converter-night/docs/RESEARCH_MEMPHIS_TOP20.md`
- `docs/WORKBENCH_LOOP.md`
