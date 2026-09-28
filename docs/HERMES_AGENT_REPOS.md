# Hermes agent repos (per-seat self-improvement)

Each Hermes seat has a **mini-repo** inside the monorepo where it improves **individually** while the canonical SOP stays in `.cursor/skills/hermes-elite-*/SKILL.md`.

## Index

**Root:** [`disklordz/hermes/agent-repos/`](../disklordz/hermes/agent-repos/README.md)

```bash
python3 disklordz/hermes/scripts/hermes_tool.py agent status
python3 disklordz/hermes/scripts/hermes_tool.py agent record-run --seat hermes-dsp --wo WO-2026-001 --summary "pluginval green"
python3 disklordz/hermes/scripts/agent_promote.py scan
python3 disklordz/hermes/scripts/agent_promote.py open --source "$(git branch --show-current)"
```

**Lead auto-opens:** pushing changes under `agent-repos/**/proposals/` triggers [`.github/workflows/hermes-agent-promote.yml`](../.github/workflows/hermes-agent-promote.yml) to create **draft** promotion PRs. CD marks `**Status:** approved` in the proposal file; after skill merge, set `**Status:** merged` and `**Promotion PR:**`.

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

1. Agent appends `PLAYBOOK.local.md` or adds `proposals/*.md` (`**Status:** draft`).
2. **hermes-lead** (CI on push) auto-opens a **draft promotion PR** for CD review.
3. CD sets `**Status:** approved`; lead merges skill/chartier edits (same PR or follow-up).
4. Set `**Status:** merged` + `**Promotion PR:**` URL; update seat `CHANGELOG.md`; note promoted lines in `PLAYBOOK.local.md`.

## Monorepo vs external repos

Default is **in-monorepo** agent folders (simple PR path, one CI). External per-seat GitHub repos remain optional later (mirror/submodule) if isolation is needed — no change required until CD picks a model.

## Cloud Agent stores

Cursor **persistent agent store** (`/cursor/stores/self`) is runtime-only and not in git. **Agent repos** are the **durable, reviewable** counterpart checked into this repository.

## Related

- [HERMES_SEATS.md](HERMES_SEATS.md)
- [HERMES_AGENT_FRAMEWORK.md](HERMES_AGENT_FRAMEWORK.md)
