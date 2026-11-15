---
name: spark-le-memory-ci
description: SparkLE keeper memory, hat/loop tags, weekly CI loop — scores to RAG and lane-registry updates.
when_to_use: After SparkLE review sessions, keeper promotion, or weekly lane quality maintenance.
paths:
  - "docs/disklordz/spark-le/MEMORY-AND-CI.md"
  - "docs/disklordz/sops/SOP-015*.md"
  - "disklordz/website/src/lib/spark-le/lane-registry.ts"
  - "disklordz/website/src/lib/rag/knowledge-snippets.ts"
---

# Skill — SparkLE memory & CI

## When to use

- Completed [session review template](../../../docs/disklordz/templates/TEMPLATE-spark-le-session-review.md)
- Weekly tag rollup from reviews
- Promoting a **keeper** top-loop or hat one-shot

## Procedure

1. Read [MEMORY-AND-CI.md](../../../docs/disklordz/spark-le/MEMORY-AND-CI.md).
2. **Extract** — keeper Y/N, lane, tags, `dna_template_id` if any.
3. **One fix** — single change: DNA doc line, registry `drum` or `nudge`, or RAG snippet (≤2 sentences).
4. Run `cd disklordz/website && npm run verify:spark` after registry or generator edits.
5. **Evidence** — attach review path or A/B note; tag [executed] in commit message.
6. **Do not** skip Phase 2 contracts if implemented; log override in PR if needed.

## Hat/loop tags (use exactly)

`hat-roll` · `hat-mud` · `pocket-pull` · `staccato` · `32-roll` · `open-hat-wash` · `perc-clutter` · `kick-bleed` · `dna-mismatch`

## Success criteria

- [ ] One tag → one tracked change
- [ ] Keeper copies include manifest + version
- [ ] RAG updated when keeper note is reusable

## Related SOPs

- [SOP-015 Phase 3 taste](../../../docs/disklordz/sops/SOP-015-spark-le-phase-3-taste-keepers.md)
- [SOP-017 Phase 5 cadence](../../../docs/disklordz/sops/SOP-017-spark-le-phase-5-elite-leadership.md)
