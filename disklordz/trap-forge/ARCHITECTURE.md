# TRAP-FORGE / MPC TRAP-FORGE Studio

Browser-based Atlanta trap drum synthesizer (TR-808–inspired DSP) with per-drum pages, dual amp/filter ADSR, Cardo-style reverb on snare/clap, FL/TR-808 step sequencer, **MPC 4×4 pad matrix**, **AI Vibe Forge (Gemini)**, PWA offline shell, and **16/24/32-bit** WAV export.

## Purpose

Design punch kicks, long 808 subs, crisp snares, claps, hats, and perc in the vein of classic Southern trap (Jeezy, Gucci, Shawty Redd, Cardo, Mike Will, Drumma Boy, EST Gee) without sample packs.

## Build & run

Static site (no AI proxy):

```bash
cd disklordz/trap-forge
python3 -m http.server 8765
# open http://127.0.0.1:8765/
```

**Recommended (AI Vibe Forge + PWA origin):**

```bash
pip install -r requirements-serve.txt
export GEMINI_API_KEY=...
python3 serve.py   # http://127.0.0.1:8765
```

ES modules require HTTP (not `file://`). Install as PWA from browser menu when served over HTTP(S).

## Data flow

1. **Primary UI:** `index.html` + `js/mpc-trap-forge-ui.js` — Akai-themed MPC shell (OfflineAudioContext engine, 16-level pads, kit mode, sequencer, `.XPM` export). **Modular studio:** `studio.html` + `js/studio-app.js` (full kit, Cardo reverb, DnD shelf). Legacy: `js/app.js`.
2. `selectAndAuditionDrum()` keeps tabs, `#canvas-visualizer`, and audition in sync.
3. `synth-trap.js` renders each drum to stereo buffers (44100 Hz float → master soft-clip → Web Audio or WAV encoder).
4. Sequencer triggers cached renders polyphonically (`activeSources` set).

## Key modules

| Path | Role |
|------|------|
| `index.html` | MPC Elite shell (6 tabs, Akai red theme, waveform canvas) |
| `js/mpc-trap-forge-ui.js` | Standalone OfflineAudioContext synth, pads, AI forge, export |
| `studio.html` | Modular TRAP-FORGE studio (per-drum kit, visualizer shelf) |
| `js/studio-app.js` | State, `renderDrumSample`, sequencer, export, visualizer |
| `js/trap-presets.js` | `DRUM_DEFAULTS`, `NOTE_FREQS`, `PRODUCER_PRESETS` |
| `css/trap-forge.css` | Legacy theme (used by `app.js` path) |
| `js/app.js` | Legacy state/UI (unchanged modules) |
| `js/synth-trap.js` | Kick, 808, snare, clap, hats, perc; SVF + `fl_clip` drive |
| `js/dsp-core.js` | ADSR, filters, lo-fi, saturation, trunk master bus, WAV |
| `js/reverb-cardo.js` | Stereo comb reverb for snare/clap |
| `js/hat-roll.js` | Phase 5 ratchet rolls (32nd/triplet, jitter, drift) + composite buffer |
| `js/ai-vibe-forge.js` | Gemini prompt → DSP params (proxy or direct key) |
| `js/mpc-pad-grid.js` | 4×4 MPC audition matrix (pitch, rolls, velocity) |
| `serve.py` | FastAPI static host + `/api/ai/vibe` |
| `manifest.webmanifest` · `sw.js` | PWA install + offline app shell |

808 glide: `f = f_root + (f_target - f_root) * (t/T)^2.2`. **Elite DSP (studio):** layered kick/clap/snare passes with `phaseAlignMix` (`js/elite-layers.js`); 2× half-band OS saturation (`applyNonlinearOversample2x`); band-limited/pink noise (`fillBandLimitedNoise`, `fillPinkNoise`). MPC shell: glide ms/semi on tab 4; modular studio: `glideInterval` on preset load (Mike Will +7 st).

## Extension points

- Add drums: implement `synth*` in `synth-trap.js`, register in `SYNTHS`, extend `DRUM_ORDER` and HTML panel.
- New presets: extend `PRODUCER_PRESETS` in `trap-presets.js` and `#preset-selector`.
- Smoke: `node scripts/smoke.mjs` from `trap-forge/`.
- Heavier anti-aliasing: extend `applySaturationBuffer` / hat `softSquare` in `dsp-core.js`.

## Related docs

- Repo index: `/ARCHITECTURE.md`
- Memphis phonk reference: `disklordz/memphis-architect/` (separate product)
