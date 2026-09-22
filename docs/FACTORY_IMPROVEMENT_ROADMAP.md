# Sound factory — improvement roadmap

Prioritized upgrades for **quality**, **Rev2 vibe**, and **agent automation**.

## Architecture shift (done in this branch)

| Before | After |
|--------|--------|
| Python `trap_synth` only | **Rev2 Trap JUCE VSTi** + `Rev2TrapOfflineRender` for REV2-TRAP-128 |
| No plugin product | `Rev2Trap/` VST3 + Standalone + 128 factory programs |

JZ400 can move to Rev2Trap engine in WO-SF-050 (same offline renderer, different preset banks).

---

## Tier 1 — Sound quality (highest ROI)

1. **Rev2 DSP depth** — wavetable osc blend, hard sync, filter FM, poly aftertouch (Prophet character).
2. **Analog drift** — slow per-voice detune / cutoff drift (Rev2 “alive” feel).
3. **Trap macro layer** — one-knob “Trap” (filter slam + drive + short env) per preset category.
4. **Post-render chain** — offline `-6 dBTP` limiter + mono sub check before QA.
5. **QA v2** — crest factor, low-band energy ratio, reject “harsh” partials (simple spectral slope test).

## Tier 2 — Factory pipeline

1. **SpectralDiff gate** — optional vs reference patch WAV per category (golden internal refs, not songs).
2. **best-of-n-runner** — 3 seeds / programs; QA subagent keeps best.
3. **Chunk CI** — GitHub Action: build Rev2Trap + render 4 programs + `factory_qa.py`.
4. **JZ400 on Rev2Trap** — separate preset bank file `Jz400Presets.cpp` (400 programs).

## Tier 3 — Product / UX

1. **Rev2Trap UI** — macro knobs, preset browser, spectrum preview.
2. **CLAP** export (monorepo pattern from JD Upgraded).
3. **Load SFZ in plugin** — verify round-trip factory output.
4. **MPCTK automation** — submodule + job step invokes `mpctk` when on Mac/Windows agent.

## Tier 4 — Agents

1. **factory-qa-audio** always after batch (mandatory subagent).
2. **PM/Airtable** chunk fields: `offset`, `limit`, `engine=rev2trap`.
3. **MPC-agent** registered in repo for keygroup batch.

## Tier 5 — Research (GitHub)

| Resource | Use |
|----------|-----|
| [tubernard/synth-emulator](https://github.com/tubernard/synth-emulator) | Rev2 UI/DSP ideas |
| [timandtheocean/autosamplerT](https://github.com/timandtheocean/autosamplerT) | SFZ layout QA |
| MPCTK | MPC keygroups |
| Wave909 DSP modules | ZDF filter, destructive stage reuse |

---

## What “better overall” means operationally

```text
Design in Rev2Trap (musical)
    → Offline render multisamples (consistent)
    → factory_qa + optional SpectralDiff
    → SFZ + MPC handoff
    → Human spot-check 1 per batch on hardware
    → Ship chunk in PR
```

**Original trap only** — no song cloning required; presets stay in **lane + category** space (lead/pad/stab/sub).
