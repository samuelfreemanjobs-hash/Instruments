# Memphis Phonk Architect (MEMPHIS-660)

## Purpose

Browser-based **Engine B** phonk drum designer for Disklordz / Instruments producers: mathematically synthesize Memphis phonk **kicks** and **snares**, preview in real time, export **24-bit / 44.1 kHz mono WAV** without a DAW. Complements Gemini Gem **Engine A** (Serum/Vital patch sheets).

## Build & run

Static ES modules — no build step.

```bash
cd disklordz/memphis-architect
python3 -m http.server 8765
```

Open `http://127.0.0.1:8765/` (required for module imports; `file://` may block).

## Data flow

```text
UI sliders → kick-engine.js / snare-engine.js (Float32 DSP)
  → canvas waveform + Web Audio preview
  → encodeWav24Mono → download
```

Kick chain: pitch-mod sine → amp ADSR → filter preset × filter ADSR → SP-1200 decimate → tape LP → tanh clip → −0.3 dBFS normalize.

Snare chain: membrane (sine/tri) + BP noise + flam clap → decimate → HPF 105 Hz → tape LP → clip → normalize.

## Key modules

| Path | Role |
|------|------|
| `js/dsp-utils.js` | ADSR, decimation, filters, WAV24 export |
| `js/kick-engine.js` | MEMPHIS-660 kick renderer + style presets |
| `js/snare-engine.js` | DR-660 tri-layer snare + styles |
| `js/app.js` | Navigation, UI, preview, export |
| `index.html` | Kick + Snare pages |

## Extension points

- Add hat/cowbell pages mirroring snare pattern.
- Optional Python mirror under `scripts/` for CI parity with Gemini Gem output.
- Host under `disklordz/website` static route when productized.

## Related docs

- [disklordz/sound-factory/ARCHITECTURE.md](../sound-factory/ARCHITECTURE.md)
- [disklordz/ARCHITECTURE.md](../ARCHITECTURE.md)
