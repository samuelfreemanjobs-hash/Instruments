# TRAP-FORGE Studio

Browser-based Atlanta trap drum synthesizer (TR-808–inspired DSP) with per-drum pages, dual amp/filter ADSR, Cardo-style reverb on snare/clap, FL/TR-808 step sequencer, and 24-bit WAV export.

## Purpose

Design punch kicks, long 808 subs, crisp snares, claps, hats, and perc in the vein of classic Southern trap (Jeezy, Gucci, Shawty Redd, Cardo, Mike Will, Drumma Boy, EST Gee) without sample packs.

## Build & run

Static site — no build step.

```bash
cd disklordz/trap-forge
python3 -m http.server 8765
# open http://127.0.0.1:8765/
```

ES modules require HTTP (not `file://`).

## Data flow

1. UI (`js/app.js`) holds `kitState` (params per drum, pattern, BPM, master bus).
2. `selectAndAuditionDrum()` keeps the active tab, scope, and audition in sync (no kick/snare desync).
3. `synth-trap.js` renders each drum to stereo buffers (44100 Hz float → master soft-clip → Web Audio or WAV encoder).
4. Sequencer triggers cached renders polyphonically (`activeSources` set).

## Key modules

| Path | Role |
|------|------|
| `index.html` | Layout, per-drum panels, sequencer, export |
| `css/trap-forge.css` | Royal blue / white theme |
| `js/app.js` | State, UI, sequencer, scope/FFT, export |
| `js/synth-trap.js` | Kick, 808, snare, clap, hats, perc + presets |
| `js/dsp-core.js` | ADSR, filters, lo-fi, saturation, trunk master bus, WAV |
| `js/reverb-cardo.js` | Stereo comb reverb for snare/clap |
| `js/hat-roll.js` | Phase 5 ratchet rolls (32nd/triplet, jitter, drift) + composite buffer |
| `js/export-dnd.js` | Drag-and-drop WAV chips for DAW import |

808 glide: `f = f_root + (f_target - f_root) * (t/T)^2.2`. Transient shaper: ±12 dB in the first 15 ms before saturation. Master: parallel trunk smash + FL soft-clip (`applyTrunkMasterBus`).

## Extension points

- Add drums: implement `synth*` in `synth-trap.js`, register in `SYNTHS`, extend `DRUM_ORDER` and HTML panel.
- New presets: extend `PRESETS` and `<select id="preset-select">`.
- Heavier anti-aliasing: extend `applySaturationBuffer` / hat `softSquare` in `dsp-core.js`.

## Related docs

- Repo index: `/ARCHITECTURE.md`
- Memphis phonk reference: `disklordz/memphis-architect/` (separate product)
