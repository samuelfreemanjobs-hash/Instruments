# SparkLE memory & continuous improvement

**Version:** 1.0 · Extends [SOP-015](../sops/SOP-015-spark-le-phase-3-taste-keepers.md) and [ELITE-PERFORMANCE.md](ELITE-PERFORMANCE.md).

## Memory layers

| Layer | Store | What gets saved |
|-------|--------|-----------------|
| **Session** | [TEMPLATE-spark-le-session-review.md](../templates/TEMPLATE-spark-le-session-review.md) | Scores, keeper Y/N, tags |
| **Keeper audio** | `keepers/<lane>/` (your store) | WAV + manifest + `dna_template_id` |
| **Param lineage** | `lane-registry.ts` `nudge` + PR history | Evidence-linked changes |
| **RAG** | `knowledge-snippets.ts` → future pgvector | Short chunks from keeper notes |
| **Agent store** | `/cursor/stores/self` (optional) | Weekly KPI snapshot JSON |

## Hat / loop specific tags

Use in review + RAG:

`hat-roll` · `hat-mud` · `pocket-pull` · `staccato` · `32-roll` · `open-hat-wash` · `perc-clutter` · `kick-bleed` · `strip-loop` · `dna-mismatch`

## Weekly CI loop (15 min)

1. Read last 7 reviews — count tags.  
2. Pick **dominant tag** → one change: DNA doc, registry `drum` base, or contract threshold.  
3. If **hat-roll** wins: edit [TRAP-HAT-LOOP-DNA.md](TRAP-HAT-LOOP-DNA.md) for that producer row.  
4. Run `npm run verify:spark-lanes` and `npm run build` if TS touched.  
5. Log line in ROADMAP or team notes: `[executed] lane X: tag Y → change Z`.

## Monthly

- Blind A/B top loops from two presets @ same BPM ([SOP-016](../sops/SOP-016-spark-le-phase-4-lineage.md)).  
- Refresh RAG snippets from top 3 keeper notes ([SOP-007](../sops/SOP-007-rag-corpus.md)).

## KPIs (hat/loop focus)

| KPI | Target |
|-----|--------|
| Keeper rate on **top-loop** stems | ≥40% |
| `hat-mud` tag rate | ↓ week over week |
| Loops with `dna_template_id` | →100% |

## Skill

Automated agent path: [.claude/skills/spark-le-memory-ci/SKILL.md](../../../.claude/skills/spark-le-memory-ci/SKILL.md)
