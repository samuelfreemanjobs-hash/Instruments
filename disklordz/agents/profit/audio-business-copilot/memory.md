# Memory — Audio Business Copilot

File-tier persistence (no DB). Layout under repo or `~/.claude/projects/disklordz/memory/`.

| File | Type | Purpose |
|------|------|---------|
| `MEMORY.md` | index | ≤200 lines; pointers only |
| `audio-business-copilot_learnings.md` | feedback | Validated runbooks |
| `audio-business-copilot_incidents.md` | project | Dated outages / misconfigs |

## Frontmatter template

```yaml
---
name: Audio Business Copilot incident 2026-03-28
description: One-line for LLM recall
type: feedback
---
```

## Recall

Tier 1: always load MEMORY.md index. Tier 2: LLM picks ≤5 files from manifest using current query + recent tool history.

## Do not memorize

Anything derivable from git log, `ARCHITECTURE.md`, or Stripe dashboard — retrieve instead.

## Audio track anchors (always Tier 1)

| Resource | Path |
|----------|------|
| Master plan | `docs/DISKLORDZ_MASTER_PLAN.md` |
| DPF architecture | `docs/DPF_TECHNICAL_ARCHITECTURE.md` |
| CHORD ENGINE PRD | `docs/CHORD_ENGINE_PRD.md` |
| Sprint progress | `disklordz/audio-command/data/progress.json` |
| Operator PWA | `disklordz/audio-command/pwa/index.html` |
| Weekly reviews | `disklordz/audio-command/reviews/` |

Engineering work is **out of scope** for this agent — delegate to Cursor + `golden-wav-qa`.
