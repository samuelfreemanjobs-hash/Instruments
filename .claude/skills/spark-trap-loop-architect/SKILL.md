---
name: spark-trap-loop-architect
description: Architect trap hi-hat, perc, and top-loop patterns for SparkLE — producer DNA, lane presets, 2-bar stems.
when_to_use: User asks for trap hat rolls, top loops, producer bounce (Metro, Southside, Tay Keith, Wheezy), or SparkLE loop packs.
paths:
  - "docs/disklordz/spark-le/**"
  - "disklordz/website/src/lib/spark-le/**"
  - "disklordz/website/src/lib/generation/loop-render.ts"
---

# Skill — Spark trap loop architect

## When to use

- Map a **producer bounce** to lanes + [TRAP-HAT-LOOP-DNA.md](../../../docs/disklordz/spark-le/TRAP-HAT-LOOP-DNA.md)
- Design **2-bar kick-less top** stems for SparkLE pads 5–8
- Add or tune `dna_template_id` / registry params

## Prerequisites

1. [PRODUCER-LANE-INDEX.md](../../../docs/disklordz/spark-le/PRODUCER-LANE-INDEX.md)
2. [SOP-013](../../../docs/disklordz/sops/SOP-013-spark-le-phase-1-feed-spark.md) (SparkLE feed)
3. [lane-registry.ts](../../../disklordz/website/src/lib/spark-le/lane-registry.ts)

## Procedure

1. **Resolve preset** — `presetId` from index (e.g. Tay Keith → `lane-tay-keith`).
2. **Read DNA row** — grid formula for CH/OH/PERC.
3. **Set manifest** — BPM, `lane`, `family`, optional `dna_template_id`.
4. **Generate or render** — factory/loop-render when wired; else document MIDI grid for manual SparkLE program.
5. **Export stems** — `Loops/Tops_Kickless_2bar.wav` + manifest.
6. **Review** — [TEMPLATE-spark-le-session-review.md](../../../docs/disklordz/templates/TEMPLATE-spark-le-session-review.md); tag `hat-roll`, `pocket-pull`, etc.

## Success criteria

- [ ] Producer named in prompt maps to documented preset
- [ ] 2-bar loop spec matches DNA doc section
- [ ] Kick-less top stem listed in manifest
- [ ] No uncleared commercial loop samples

## Delegate

See [SUBAGENTS.md](../../../docs/disklordz/spark-le/SUBAGENTS.md) — `computerUse` for SparkLE proof.

## Continuous improvement

After session, run [spark-le-memory-ci](../spark-le-memory-ci/SKILL.md) tags → one registry or DNA edit.
