---
name: trap-drum-loop-architect
description: Designs trap and adjacent drum loop blueprints — bar counts, kick/snare/hat/808 roles, fills, BPM, and MIDI or one-shot placement plans for DRUMSMITH kits. Use when building trap/phonk/memphis loop packs, 2-bar grooves, hi-hat rolls, or when the user mentions trap-drum-loop-architect or loop architecture.
---

# Trap Drum Loop Architect

## Read first

| Resource | Path |
|----------|------|
| Genre factory | `claude/drumsmith/` — `drift-phonk`, `detroit-trap`, `southern-trap`, `memphis-tape`, `cloud-rap` |
| Kit example | `claude/drumsmith/kits/tail-light-static/kit-spec.md` |
| Groove extract | skill `learn-midi-groove` |
| Spec / SKU | skill `product-design` |
| Voices | skill `audio-dsp-engineering` / `drum-voice-recipes` |

## Scope

**In**

- Loop blueprints: bars, BPM, section roles (intro/hook/verse fill)
- Lane architecture: 808 vs kick, snare/clap stack, open/closed hat, rolls, perc/FX
- MIDI pattern sketches or step tables (16th/32nd) with velocity intentions
- Pack structure: how many loops × variants vs one-shots (counts tagged **A** until INTAKE locked)

**Out**

- Final commercial masters / store copy without release gate
- Sample cloning or “type beat of \<artist\>” product naming
- Deep spectral reverse-engineering → `drum-sample-reverse-engineering`
- Cold market research → `customer-and-competition-intel`

## Hard rules

1. Genres are **creative starting points**, not recreations of trademarked machines or artists (`WORKFLOW_V13.md`).
2. Mark device/export claims **UNVERIFIED** until tested.
3. Tag BPM/bar math **C** when from `loop_samples`; stylistic density **A**.
4. 808s: state key / glide intent; unpitched lanes NA.
5. Technical loop length must close on bar boundaries at stated SR (usually 44.1 kHz).

## Architecture checklist

| Lane | Typical role (trap-adjacent) |
|------|------------------------------|
| Kick | Transient punch; may duck under 808 |
| 808 | Sustained pitched bass; key + length |
| Snare/clap | Backbeat 2/4 or half-time; stacks |
| Hats | 1/8–1/32 + rolls; velocity cascades |
| Perc/FX | Sparse ear candy; avoid masking kick |

## Script (preferred)

```bash
cd claude/drumsmith
python3 scripts/trap_loop_architect.py --name tls-groove --genre drift-phonk --bpm 74 --bars 2 \
  --out /tmp/tls-loop.json --midi-out /tmp/tls-loop.mid
# samples/bar via dsp_math.loop_samples (C)
```

## Workflow

1. **Lane + BPM** — Pick genre profile + BPM (e.g. TLS 74; trap often 130–150 half-time feel — label which).
2. **Form** — 1 / 2 / 4 / 8 bar loop; where fills land.
3. **Step map** — Per lane, grid of hits + vel bands (or point to groove card).
4. **Variant plan** — Sparse / full / roll-forward / breakdown — counts for PRODUCT_SPEC.
5. **Handoff** — One-shot needs → DSP factory; MIDI → songwriting/`learn-midi-groove` params; SKU → `product-design`.

## Loop blueprint template

```markdown
# Loop set: <name>
Genre profile … · BPM … · SR … · bars … · feel: straight | swing (from groove card?)

## Promise
one line (no artist clone)

## Lanes
| Lane | Pattern summary | Vel intent | One-shot IDs / TBD |

## Forms
| Variant | Bars | Notes |

## 808
key · glide · length (A/C)

## Grid math
samples/bar = … [C]

## HITL
ear · naming · commercial

## Next
product-design · native_factory / recipes · learn-midi-groove (if importing feel)
```

## Checklist

- [ ] Genre profile named (not a brand clone)
- [ ] BPM + bar length math stated
- [ ] Kick/808 relationship explicit
- [ ] Hat roll density tagged A if guessed
- [ ] Product vs DSP handoffs listed

## Related

`learn-midi-groove`, `product-design`, `audio-dsp-engineering`, `research-to-product`, `audio-analysis`
