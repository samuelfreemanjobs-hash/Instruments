# SparkLE elite lanes (documentation + generation)

## Purpose

Operational and **generation** index for **SparkLE standalone** workflows: hard gangsta trap, phonk, Memphis, southern, regional trap, rage, and **hardware** lanes (DR-660, TR-808 clone). Style **archetypes** only — not artist-affiliated samples.

## Build & run

No separate binary. Lanes drive Disklordz kit generation:

```bash
cd disklordz/website && npm ci && npm run build
# Presets include all lane-* ids from lane-registry.ts
```

Sound factory (optional sync):

```bash
python3 disklordz/sound-factory/scripts/generate_kit.py --help
```

## Data flow

```text
lane-registry.ts (drum bases + metadata)
  → lane-resolve.ts (prompt → preset when neutral factory preset)
  → presets.ts (STYLE_PRESETS)
  → prompt-params.ts (resolveDrumParams)
  → factory / product-factory → WAV + manifest
  → SparkLE user samples (SOP-013 inbox)
  → review template → keepers → RAG (SOP-015)
```

## Key modules

| Path | Role |
|------|------|
| [LANES.md](LANES.md) | Human lane catalog by family |
| [TRAP-HAT-LOOP-DNA.md](TRAP-HAT-LOOP-DNA.md) | Hi-hat / perc / top-loop pattern formulas |
| [PRODUCER-LANE-INDEX.md](PRODUCER-LANE-INDEX.md) | Chart producers → preset ids |
| [SUBAGENTS.md](SUBAGENTS.md) | Cursor subagent delegation |
| [MEMORY-AND-CI.md](MEMORY-AND-CI.md) | Keepers, tags, weekly CI |
| [ELITE-PERFORMANCE.md](ELITE-PERFORMANCE.md) | KPIs, rotation, pro checklist |
| [../sops/SOP-012-spark-le-elite-system-charter.md](../sops/SOP-012-spark-le-elite-system-charter.md) | Charter + safety |
| `disklordz/website/src/lib/spark-le/lane-registry.ts` | Source of truth for lane presets |
| `disklordz/website/src/lib/spark-le/lane-resolve.ts` | Prompt inference + `coalesceSparkPreset` |
| `disklordz/website/scripts/verify-spark-lanes.ts` | Registry contract CI (`npm run verify:spark-lanes`) |
| Repo TR-808 / SP-1200 | Hardware reference for `lane-tr-808-clone` tuning ([ARCHITECTURE.md](../../../ARCHITECTURE.md)) |

## Extension points

1. Add `SparkLaneDef` row in `lane-registry.ts`.
2. Update [LANES.md](LANES.md) family table.
3. Add RAG snippet in `knowledge-snippets.ts`.
4. Add contract tests when Phase 2 fixtures exist ([SOP-014](../sops/SOP-014-spark-le-phase-2-trust-contracts.md)).

## Related docs

- [LANE-BACKLOG.md](LANE-BACKLOG.md) — candidate lanes  
- [../README.md](../README.md) · [../../ROADMAP.md](../../ROADMAP.md)
