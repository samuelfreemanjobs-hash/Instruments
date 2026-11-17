# Memphis Drum Loop Factory OS (browser prototype)

## Purpose

Interactive **kit-factory workbench** for SP-1200–era Memphis / Southern / phonk grammar: procedural patterns, Lock+Mutate, synthesized voices, vintage DSP, WAV/MIDI export, and an agent meta-prompt for Claude Code / Antigravity. **Commercial SKU = original drum sample kits** (oneshots + loops). Lab/OS are generators that feed that kit — not a DAW/VST product. **Original synthesis only** — not a sample dump of copyrighted recordings.

Who uses it: DiskLordz / DRUMSMITH kit design; Sam iterating Factory OS UX before modularizing into the DR-660 lab and oneshot/loop packaging pipelines.

## Build & run

Self-contained single HTML (Tailwind CDN + Lucide). From this directory:

```bash
python3 -m http.server 8780 --bind 127.0.0.1
# open http://127.0.0.1:8780/
```

Sibling modular lab (preferred long-term home for DSP modules):

```bash
cd ../dr660-lab && python3 -m http.server 8770 --bind 127.0.0.1
```

## Data flow (hybrid pipeline)

```text
Pattern Intelligence → Groove Engine → DDSP Drum Synth → SP/Tape DSP → Loop Renderer → Critic
         │                  │                │                  │              │
   Lock+Mutate         micro-swing      808/SP synths     bitcrush/LPF    OfflineAudio
   32-step grid        odd-16th lag     hats/cowbell      cassette/tape   WAV + MIDI
```

1. UI locks tracks → **Mutate Unlocked** regenerates unlocked rows (hats, bells, perc).
2. Lookahead scheduler triggers procedural voices into an SP-style FX chain.
3. Offline path renders 2-bar masters/stems to 16-bit PCM WAV; Type-0 MIDI export available.
4. Critic scores bounce heuristics (or Gemini when wired); meta-prompt box copies the agent blueprint.

## Threading / realtime

Web Audio owns the audio thread. Scheduler uses `currentTime` + lookahead. Offline export uses `OfflineAudioContext` (not the live graph).

## Key modules (this prototype)

| Path | Role |
|------|------|
| `index.html` | Monolithic Gemini Factory OS (UI + audio + export) |
| `BUILD_PLAN.md` | Claude Code / Antigravity / Cursor lane map |
| `meta-prompt.md` | Pasteable master agent prompt |
| `../dr660-lab/` | Modular Web Audio lab (sequencer, FX bus, cowbell, ratchets) — merge target |

## Extension points

- Split `index.html` into ES modules aligned with `dr660-lab/js/*`
- Native/CLI renderer: `claude/drumsmith/native` + Converter Night kits
- Antigravity: MIDI fixtures, packaging scripts, pytest on pattern JSON
- Claude Code: headless groove mutate CLI, OfflineAudio tests; optional DAW surface only if kit SKU already shipping

## Related docs

- `BUILD_PLAN.md` · `meta-prompt.md`
- `../dr660-lab/ARCHITECTURE.md`
- `docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md`
- `kits/converter-night/docs/RESEARCH_MEMPHIS_TOP20.md`
