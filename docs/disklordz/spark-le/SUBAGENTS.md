# SparkLE subagents & delegation

How Cloud / Cursor **subagents** map to trap hat, perc, loop, and SparkLE work.

## When to delegate

| Task | Subagent | Prompt seed |
|------|----------|-------------|
| Find preset / lane / DNA in repo | `explore` | “Find spark-le lane for {producer}; thoroughness: medium” |
| Generator or contract CI red | `ci-investigator` | PR URL + failing check name |
| Non-obvious audio/regression bug | `debug` | “Hat roll mud on lane-tay-keith @ 140; contracts LOOP-GRID” |
| SparkLE load + export proof | `computerUse` | Load 8 pads per SOP-013; export 30s WAV |
| Large lane registry refactor | `generalPurpose` | Scoped file list from `lane-registry.ts` only |
| UI preset grouping | `generalPurpose` | KitGenerator family optgroups |

## Task templates (copy into Task tool)

### Trap loop architect

```text
Read docs/disklordz/spark-le/TRAP-HAT-LOOP-DNA.md and PRODUCER-LANE-INDEX.md.
Implement or verify 2-bar top-loop stem for preset {presetId} at {bpm} BPM.
Output: manifest snippet + which dna_template_id applies.
Follow SOP-013 and security baseline. No secrets.
```

### Keeper / memory ingest

```text
Read docs/disklordz/spark-le/MEMORY-AND-CI.md and SOP-015.
Process review file at {path}: extract tags, propose one param nudge for lane-registry.
Do not merge without contract note.
```

### Producer lane add

```text
Add one SparkLaneDef row per DISKLORDZ agent rules: lane-registry.ts, LANES.md,
PRODUCER-LANE-INDEX.md, prompt-params tokens, one RAG snippet. Run npm run build in disklordz/website.
```

## What not to delegate

- Prod deploy, Stripe, Supabase prod (human — SOP-012)  
- Weakening Phase 2 contracts without override  
- Shipping uncleared sample pack rips  

## Related

- [CURSOR_AGENT_PLAYBOOK.md](../../CURSOR_AGENT_PLAYBOOK.md)  
- [spark-trap-loop-architect SKILL](../../../.claude/skills/spark-trap-loop-architect/SKILL.md)
