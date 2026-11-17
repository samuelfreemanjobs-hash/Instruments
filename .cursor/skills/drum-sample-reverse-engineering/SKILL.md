---
name: drum-sample-reverse-engineering
description: Derives synthesis recipe hypotheses from reference drum one-shots — envelope, pitch contour, noise vs tone mix, saturation cues — for original DRUMSMITH/FORGE voices. Use when reverse-engineering a kick/snare/808/hat, matching a reference vibe without cloning, filling voice recipes from analysis, or when the user mentions drum-sample-reverse-engineering or recipe from WAV.
---

# Drum Sample Reverse Engineering

## Read first

| Area | Path |
|------|------|
| Voice recipes | `claude/drumsmith/.claude/skills/drum-voice-recipes/SKILL.md` |
| Native voices | `claude/drumsmith/docs/V2_NATIVE.md` |
| Analysis meters | skill `audio-analysis` |
| DSP implement | skill `audio-dsp-engineering` |
| Provenance / soul | `claude/drumsmith/SOUL.md`, `docs/provenance/` |

## Scope

**In**

- Decompose a **lawfully held** reference WAV into measurable traits + recipe hypotheses
- Map traits → parametric-v2 / native voice knobs (808, kick, snare, clap, hat, perc, FX)
- Tag every number **S** (sourced meter), **C** (computed), **A** (ear/guess)
- Emit a recipe draft for factory/native render — **original synthesis**, not resampling

**Out**

- Shipping or distributing uncleared third-party samples
- Claiming a clone / 1:1 recreation of a commercial pack
- Listening verdicts without human notes
- Implementing C++/Python DSP (hand off to `audio-dsp-engineering`)

## Legal / provenance hard rules

1. **No unlicensed third-party masters** in kit output — synthesize; do not copy PCM into products.
2. Reference audio is for **analysis only** unless Sam cleared commercial rights.
3. Never write “identical to \<brand kit\>” in specs or store copy.
4. Unknown rights → stop or park HITL (`DUTIES.md` unknown provenance).
5. Prefer genre-lane references and public-domain / owned stems when available.

## Method

1. **Rights check** — State source and clearance status before analysis.
2. **Classify** — Voice family (808 / kick / snare / clap / hat / perc / FX); mono/stereo; pitched?
3. **Measure** — Run `audio-analysis` path (`verify_kit` / `pitch_check` / peak-RMS windows). Record numbers with **S/C**.
4. **Structure listen notes (human)** — Transient hardness, body length, noise color, click, saturation — mark human quotes vs **[A]** agent guesses.
5. **Hypothesis stack** — Ordered DSP stages (e.g. pitch env → sine/tri body → noise → LPF → sat → micro-fade). Each param **S/C/A**.
6. **Render A/B** — Generate **original** take via `native_factory` / recipes; compare meters only; ear gate is human.
7. **Lock recipe** — Write parametric entry + provenance stub; discard reference PCM from ship tree.

## Recipe card template

```markdown
# RE: <hit name> ← ref <id or “internal”>

## Rights
clearance: owned | licensed | analysis-only | HITL

## Classification
family / pitched? / SR / channels / duration (S)

## Measured
peak, RMS, pitch screen, env length estimates (S/C)

## Human notes
…

## Synthesis hypothesis
1. …
Params: { … }  # each tagged S|C|A

## Gaps
what meters cannot tell (stereo image, perceived “dirt”, etc.)

## Next
render command + audio-analysis re-check; Sam ear gate if ship-bound
```

## Checklist

- [ ] Rights status written
- [ ] Meters from scripts, not invented
- [ ] Recipe is synthesis params, not “use this WAV”
- [ ] No brand-clone marketing language
- [ ] Hand-off to `audio-dsp-engineering` for code, `product-design` for SKU naming

## Related

`audio-analysis`, `audio-dsp-engineering`, `product-design`, `deep-research`
