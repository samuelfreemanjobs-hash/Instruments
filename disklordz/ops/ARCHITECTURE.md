# Disklordz operating system (in-repo)

**Purpose:** Single place for **how we run** the company in software: work orders, agents, CI, products, and revenue lanes. Agents maintain this via **`hermes-sop`**.

## Layers

```text
Grok / Planner     → specs, WOs, acceptance criteria
Hermes lead        → route seats, branch/PR, evidence
Hermes seats (16)  → implement (each reads elite skill + linked SOPs)
hermes-sop         → author, audit, index SOPs; close gaps with seat owners
Airtable / GitHub  → system of record (human confirms Done)
```

## Canonical paths

| Path | Role |
|------|------|
| [sops/README.md](sops/README.md) | Human SOP index |
| [sops/INDEX.json](sops/INDEX.json) | Machine registry (CI + `hermes_tool sop audit`) |
| [docs/HERMES_SOP_OPERATIONS.md](../../docs/HERMES_SOP_OPERATIONS.md) | SOP agent charter + collaboration model |
| [docs/HERMES_AGENT_FRAMEWORK.md](../../docs/HERMES_AGENT_FRAMEWORK.md) | Seat team |
| [docs/PRODUCT_FINISH_PLAYBOOK.md](../../docs/PRODUCT_FINISH_PLAYBOOK.md) | Finish products automation |
| [disklordz/hermes/ARCHITECTURE.md](../hermes/ARCHITECTURE.md) | Hermes CLI toolkit |

## Commands

```bash
python3 disklordz/hermes/scripts/hermes_tool.py sop index
python3 disklordz/hermes/scripts/hermes_tool.py sop audit
python3 disklordz/hermes/scripts/hermes_tool.py sop coverage
python3 disklordz/hermes/scripts/hermes_tool.py ops checklist
```

## Extension

New recurring procedure → open PR adding `sops/SOP-*.md` + row in `INDEX.json`. **`hermes-sop`** drafts; **owner seat** reviews accuracy; **lead** merges.
