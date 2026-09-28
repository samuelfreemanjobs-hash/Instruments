# Hermes agent repos (per-seat self-improvement)

Each Hermes seat has a **mini-repo** inside the monorepo where it improves **individually** while the canonical SOP stays in `.cursor/skills/hermes-elite-*/SKILL.md`.

## Index

**Root:** [`disklordz/hermes/agent-repos/`](../disklordz/hermes/agent-repos/README.md)

```bash
python3 disklordz/hermes/scripts/hermes_tool.py agent status
python3 disklordz/hermes/scripts/hermes_tool.py agent record-run --seat hermes-dsp --wo WO-2026-001 --summary "pluginval green"
```

## Why not one shared doc?

Seats have different evidence (GUI video vs SQL migrations vs HO JSON). Splitting repos:

- Keeps **playbooks** readable per discipline  
- Lets **lead** review promotion PRs seat-by-seat  
- Avoids cross-contamination (e.g. preset tips in security repo)

## Layers

```text
.cursor/skills/hermes-elite-<seat>/SKILL.md   ← org-wide SOP (PR to change)
.cursor/hermes/seats/*.md                      ← short charter
disklordz/hermes/agent-repos/<seat-id>/       ← individual learning + proposals
```

## Promotion workflow

1. Agent appends `PLAYBOOK.local.md` or adds `proposals/*.md`.
2. **hermes-lead** triages in weekly or pre-merge review.
3. Approved text moves into skill/chartier via normal PR.
4. `CHANGELOG.md` updated; optional playbook entry trimmed if fully promoted.

## Cloud Agent stores

Cursor **persistent agent store** (`/cursor/stores/self`) is runtime-only and not in git. **Agent repos** are the **durable, reviewable** counterpart checked into this repository.

## Related

- [HERMES_SEATS.md](HERMES_SEATS.md)
- [HERMES_AGENT_FRAMEWORK.md](HERMES_AGENT_FRAMEWORK.md)
