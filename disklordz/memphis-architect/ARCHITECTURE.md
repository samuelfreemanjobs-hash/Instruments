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
| `scripts/generate_kick.py` | CLI Engine B kick (parity with `synthesizeKick`) |
| `scripts/generate_snare.py` | CLI Engine B snare (parity with `synthesizeSnare`) |
| `scripts/smoke.py` | RIFF / 24-bit render smoke test |
| `docs/` | Gemini Gem instructions + research → knob mapping |

Legacy modular `js/` split removed in favor of this Gemini reference build.

## Extension points

- Optional kick click/sub layers (research 1–2.8 kHz / 35–55 Hz) as extra oscillators.
- Host under `disklordz/website/public/memphis-architect/` for production.
- Engine A patch sheets: [docs/GEM_MEMPHIS-660_KICK.md](docs/GEM_MEMPHIS-660_KICK.md), [docs/GEM_MEMPHIS-DR660_SNARE.md](docs/GEM_MEMPHIS-DR660_SNARE.md).

## Related docs

- [docs/README.md](docs/README.md) — Gem setup and research index
- [docs/RESEARCH_TO_ENGINE_B.md](docs/RESEARCH_TO_ENGINE_B.md)
- [docs/808_SNARE_MEMPHIS_SYSTEMATIC.md](docs/808_SNARE_MEMPHIS_SYSTEMATIC.md)
- [disklordz/sound-factory/ARCHITECTURE.md](../sound-factory/ARCHITECTURE.md)
- [disklordz/ARCHITECTURE.md](../ARCHITECTURE.md)
