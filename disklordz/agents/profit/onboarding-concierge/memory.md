# Memory — Onboarding Concierge

File-tier persistence (no DB). Layout under repo or `~/.claude/projects/disklordz/memory/`.

| File | Type | Purpose |
|------|------|---------|
| `MEMORY.md` | index | ≤200 lines; pointers only |
| `onboarding-concierge_learnings.md` | feedback | Validated runbooks |
| `onboarding-concierge_incidents.md` | project | Dated outages / misconfigs |

## Frontmatter template

```yaml
---
name: Onboarding Concierge incident 2026-03-28
description: One-line for LLM recall
type: feedback
---
```

## Recall

Tier 1: always load MEMORY.md index. Tier 2: LLM picks ≤5 files from manifest using current query + recent tool history.

## Do not memorize

Anything derivable from git log, `ARCHITECTURE.md`, or Stripe dashboard — retrieve instead.
