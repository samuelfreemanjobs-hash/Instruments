# Memphis Phonk Architect (MEMPHIS-660)

## Purpose

Single-page **Engine B** phonk drum designer: in-browser DSP for Memphis **kick** and **snare** one-shots, CRT/brutalist UI (Tailwind), SVF filter modes, amp/filter ADSR, 24-bit WAV export.

## Build & run

No build step. Requires network for Tailwind CDN + Google Fonts on first load.

```bash
cd disklordz/memphis-architect
python3 -m http.server 8765
```

Open `http://127.0.0.1:8765/`

## Data flow

```text
index.html (inline JS)
  → synthesizeKick / synthesizeSnare @ 44.1 kHz
  → canvas oscilloscope (click = preview)
  → downloadWav (24-bit mono PCM)
```

**Kick chain:** pitch-mod sine → amp ADSR → ZOH decimate + quantize → tape LP or 12 dB SVF (LP/BP/HP) with filter ADSR → tanh clip → −0.3 dBFS normalize.

**Snare chain:** membrane + BP noise + flam clap → decimate/quantize → 105 Hz HPF → 11.5 kHz LP → tanh → normalize.

## Key modules

| Path | Role |
|------|------|
| `index.html` | UI, DSP, WAV encoder (self-contained) |

Legacy modular `js/` split removed in favor of this Gemini reference build.

## Extension points

- Extract `synthesizeKick` / `synthesizeSnare` to shared module for unit tests.
- Host under `disklordz/website/public/memphis-architect/` for production.
- Engine A patch sheets remain a separate Gemini Gem workflow.

## Related docs

- [disklordz/sound-factory/ARCHITECTURE.md](../sound-factory/ARCHITECTURE.md)
- [disklordz/ARCHITECTURE.md](../ARCHITECTURE.md)
