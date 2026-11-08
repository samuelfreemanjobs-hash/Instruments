# SparkLE keepers (Phase 3)

**Version:** 1.0 · **SOP:** [SOP-015](../sops/SOP-015-spark-le-phase-3-taste-keepers.md)

## Folder layout

Store **outside** the repo if files are large; structure stays the same:

```text
keepers/
  <presetId>/                 # e.g. lane-smooth-jazz-rap
    manifest.json             # copy of spark-le-manifest + review block filled
    OneShots/                 # 8 pads when kit promoted
    Loops/
    reviews/
      YYYY-MM-DD_<batch_id>.md  # session review snapshot
```

**Max 12 keepers per `presetId`.** When full, demote lowest **Vibe** score (tie-break: Groove).

## Promote checklist

- [ ] Phase 2 contracts green (`npm run verify:spark` in website)
- [ ] SparkLE session completed; review template filed
- [ ] **Keeper = Y** with one-line why
- [ ] No clip; peaks match manifest `target_lufs` notes
- [ ] `lineage.parent_seed` or `parent_keeper_id` set if mutation
- [ ] `dna_template_id` set when top-loop stem promoted
- [ ] Procedural/original only — no uncleared samples

## Demote / revert

1. Tag review: `dna-mismatch` or contract fail.
2. Remove from `keepers/<presetId>/` index (keep WAV archived if unsure).
3. If default params changed because of this keeper, **revert PR** before next batch.

## Feed back into system (weekly)

| Tag dominant | Action |
|--------------|--------|
| `hat-roll` / `hat-mud` | [TRAP-HAT-LOOP-DNA.md](TRAP-HAT-LOOP-DNA.md) row |
| `pocket-pull` | Half-time A/B note in review |
| `kick-bleed` | `808-SUB` threshold or pad 1 decay in SparkLE |
| `perc-clutter` | Pad 7/8 manifest swap |

Run skill: [.claude/skills/spark-le-memory-ci/SKILL.md](../../../.claude/skills/spark-le-memory-ci/SKILL.md)

## Related

[MEMORY-AND-CI.md](MEMORY-AND-CI.md) · [SOP-016 lineage](../sops/SOP-016-spark-le-phase-4-lineage.md)
