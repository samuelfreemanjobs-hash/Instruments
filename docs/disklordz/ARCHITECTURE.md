# Disklordz documentation hub

## Purpose

Central **SOPs**, **templates**, and cross-links for Cursor agents and humans working on Disklordz inside the Instruments monorepo. This is not runtime code — it is operational knowledge distilled from work orders, Cloud Agent runs, and fleet automation.

## Build & run

No build. Read the index:

```bash
ls docs/disklordz/sops/
ls docs/disklordz/templates/
```

## Data flow

```text
Airtable WO / user goal
  → template (structured Cloud prompt)
  → agent executes SOP checklist
  → PR + ROADMAP / ARCHITECTURE updates
  → fleet sync (if agents touched)
```

## Key modules

| Path | Role |
|------|------|
| [README.md](README.md) | Index of SOPs and templates |
| [sops/](sops/) | Step-by-step procedures |
| [templates/](templates/) | Copy-paste blocks for Cursor, GitHub, PM ADD |

## Extension points

Add `SOP-NNN-short-name.md` and link from [README.md](README.md). Reusable prompt blocks go in `templates/`.

## Related docs

- [../ROADMAP.md](../ROADMAP.md) · [../CURSOR_AGENT_PLAYBOOK.md](../CURSOR_AGENT_PLAYBOOK.md)
- [../../DISKLORDZ_AGENTS.md](../../DISKLORDZ_AGENTS.md) · [../../disklordz/ARCHITECTURE.md](../../disklordz/ARCHITECTURE.md)
