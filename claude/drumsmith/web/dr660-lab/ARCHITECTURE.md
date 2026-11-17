# DR-660 Memphis DSP Lab (browser)

## Purpose

Local **Web Audio** workbench for 90s Memphis-style drum workflow: DR-660–inspired **16-step sequencer** (lookahead + swing), live voice triggers, cassette saturation, 12-bit bitcrush, and lo-fi tape delay. Analysis and prototyping only — not a Boss ROM dump and not a commercial kit.

Who uses it: DiskLordz / DRUMSMITH agents and Sam, iterating groove feel before native kit renders.

## Build & run

No build step. From this directory:

```bash
python3 -m http.server 8770 --bind 127.0.0.1
# open http://127.0.0.1:8770/
```

Unit tests (Node, no browser):

```bash
node --test tests/*.test.mjs
```

## Data flow

1. UI transport → `DR660Sequencer.start/stop`
2. Lookahead `setInterval` (~25 ms) schedules note events on `AudioContext` clock
3. Lagged ticks call `catchUpExpiredSteps` so past slots are skipped (no burst of late hits)
4. Per step: swing offset on odd 0-based 16ths → `triggerSound(track, time, velocity)`
5. `DrumEngine` plays short synthesized one-shots into `MemphisFXBus.input` (velocity² gain; CHH chokes OHH; `stopAll` on transport Stop)
6. Master FX bus: HPF pre-emphasis → 12-bit bitcrush worklet → dark LPF → cassette WaveShaper → lo-fi tape delay → destination

## Threading / realtime

Browser audio thread is owned by Web Audio. UI never calls `setTimeout` for *audio* timing — only for playhead paint. Keep `scheduleAheadTime` ≈ 100 ms. Transport generation invalidates pending visual callbacks; `DrumEngine.stopAll` cancels queued oscillators/buffers.

## Key modules

| Path | Role |
|------|------|
| `index.html` | Single-rack UI: transport, 16-step grid, FX mixer |
| `js/Sequencer.js` | Lookahead clock, swing, catch-up, pattern, velocity |
| `js/DrumEngine.js` | Lightweight synth one-shots + stopAll |
| `js/MasterChain.js` | `MemphisFXBus` — HPF → bitcrush → LPF → cassette → tape delay |
| `js/TapeDelay.js` | Dry/wet delay with filtered feedback |
| `js/cassetteCurve.js` | Soft-clip tape WaveShaper curves |
| `js/bitcrush-processor.js` | AudioWorklet 12-bit + downsample |
| `js/app.js` | Wire UI ↔ audio |
| `css/lab.css` | Obsidian/gunmetal + burgundy rack styling |
| `tests/sequencer_math.test.mjs` | Swing / 16th / catch-up |
| `tests/cassette_curve.test.mjs` | Cassette curve shape |

## Extension points

- Swap synth triggers for decoded WAV buffers (CNV masters / DR-660 refs when staged)
- Hi-hat choke groups already present; optional 32nd/64th ratchets
- MIDI import/export of archetype patterns from `kits/converter-night/loops/`

## Related docs

- `kits/converter-night/recipes/DR660_LOOPS.md`
- `kits/converter-night/docs/RESEARCH_MEMPHIS_TOP20.md`
- `docs/WORKBENCH_LOOP.md`
