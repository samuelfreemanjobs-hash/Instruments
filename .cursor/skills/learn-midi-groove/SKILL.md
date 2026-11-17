---
name: learn-midi-groove
description: Extracts groove models from MIDI or timed hit lists — swing, microtiming, velocity curves, ghost notes, and bar-loop structure — as tagged parameters for new patterns. Use when learning a groove from a .mid, humanizing drums, documenting feel, or when the user mentions learn-midi-groove, swing map, or velocity pocket.
---

# Learn MIDI Groove

## Read first

| Resource | Path |
|----------|------|
| MIDI export / validate | `songwriting-engine/songwriting/midi.py`, `mpc.py` |
| Hz ↔ MIDI | `claude/drumsmith/scripts/dsp_math.py` (`hz_to_midi`, `midi_to_hz`) |
| Loop length math | `loop_samples` in `dsp_math.py` (kit-spec BPM grids) |
| Product / loops | skill `product-design`, `trap-drum-loop-architect` |

## Scope

**In**

- Parse or describe MIDI drum/perc tracks into a **groove card** (grid, swing, timing deltas, velocities)
- Build reusable feel params (PPQ, swing %, push/pull ms or ticks, accent map)
- Map GM/MPC-style note numbers → kit categories when known
- Tag every number **S** (from file), **C** (computed), **A** (stylized guess)

**Out**

- Cloning copyrighted melodies/hooks as product content without clearance
- Claiming a famous producer’s “secret” feel without sourced MIDI
- Synthesizing WAVs → `audio-dsp-engineering`
- Full trap arrangement → `trap-drum-loop-architect`

## Hard rules

1. **Rights** — State MIDI provenance; analysis-only unless Sam cleared redistribution.
2. **No listening claims** — Feel adjectives from humans only; meters from file math.
3. Prefer **tick/ms tables** over vibes; document PPQ and BPM used.
4. Ghost notes and flams are first-class (velocity + offset), not noise.
5. Do not overwrite kit masters with imported MIDI audio renders unless asked.

## Method

1. **Load** — Note list: tick, pitch, velocity, duration; tempo/PPQ/time sig from header or user.
2. **Normalize** — Quantize grid (e.g. 1/16) as reference; measure signed error per hit (**C**).
3. **Swing** — Even vs odd subdivision delay; report as % or tick offset (**C**), not “groovy.”
4. **Velocity** — Histogram + accent pattern per bar (kick/snare/hat lanes).
5. **Roles** — Assign pitches to kit lanes (kick/snare/hat/808/perc) — document map.
6. **Emit** — Groove card + optional pattern JSON/table for architects.

## Groove card template

```markdown
# Groove: <name>
BPM … · PPQ … · bars … · source … · rights …

## Grid
subdivision · swing (C) · push/pull summary

## Timing (S/C)
| Bar | Lane | Tick | Δticks vs grid | Vel |

## Velocity curve
accents / ghosts / dynamics notes

## Lane map
MIDI note → kit category

## Params for reuse
swing, humanize_ticks, accent_template_id — each S|C|A

## Next
trap-drum-loop-architect | product-design | songwriting MIDI export
```

## Checklist

- [ ] PPQ/BPM stated
- [ ] Timing deltas computed, not invented
- [ ] Rights line present
- [ ] Lane map explicit
- [ ] Hand-off skill named

## Related

`trap-drum-loop-architect`, `product-design`, `audio-analysis`, `deep-research`
