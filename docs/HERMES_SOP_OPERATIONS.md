# Hermes SOP & Procedures agent

**Seat ID:** `hermes-sop`  
**Skill:** [.cursor/skills/hermes-elite-sop/SKILL.md](../.cursor/skills/hermes-elite-sop/SKILL.md)  
**Charter:** [.cursor/hermes/seats/sop-procedures.md](../.cursor/hermes/seats/sop-procedures.md)

## Mission

Build and maintain a **running operating system** for Disklordz: every recurring action has a named procedure, an owner seat, and an automated verification hook where possible.

## How it works with other agents

```text
                    ┌─────────────────┐
                    │   hermes-sop    │
                    │ index · audit   │
                    │ draft · coverage│
                    └────────┬────────┘
                             │ collaborates
     ┌───────────────────────┼───────────────────────┐
     ▼                       ▼                       ▼
hermes-ops              hermes-devops            hermes-web
(WO lifecycle)          (plugin CI SOP)          (SaaS deploy SOP)
     │                       │                       │
     └───────────────────────┴───────────────────────┘
                             │
                    implement seats execute SOPs
                    elite skills stay seat-specific
```

| Phase | hermes-sop | Other seats |
|-------|------------|-------------|
| **Discover** | Run `sop audit`, read user/lead requests | Report friction in `agent record-run` |
| **Draft** | Write `disklordz/ops/sops/SOP-*.md` from template | Confirm steps match reality |
| **Index** | Update `INDEX.json` + README | — |
| **Verify** | CI-friendly `sop audit` | Run verification commands in SOP |
| **Promote** | Link SOP from elite skill (PR) | Review accuracy |

## Canonical library

- [disklordz/ops/ARCHITECTURE.md](../disklordz/ops/ARCHITECTURE.md)
- [disklordz/ops/sops/README.md](../disklordz/ops/sops/README.md)
- [disklordz/ops/sops/INDEX.json](../disklordz/ops/sops/INDEX.json)

## Invoke in Cursor

```text
hermes-sop: Audit all procedures and add SOP for <topic>. Run sop audit until green.
```

Or Task subagent with the same prefix after reading the elite skill.

## Automation

```bash
python3 disklordz/hermes/scripts/hermes_tool.py sop index      # human-readable catalog
python3 disklordz/hermes/scripts/hermes_tool.py sop audit     # exit 1 if broken index
python3 disklordz/hermes/scripts/hermes_tool.py sop coverage  # seats vs SOPs
python3 disklordz/hermes/scripts/hermes_tool.py sop coverage --seat hermes-dsp
```

Optional CI (local/nightly): run `sop audit` after doc changes under `disklordz/ops/`.

## Related

- [HERMES_AGENT_FRAMEWORK.md](HERMES_AGENT_FRAMEWORK.md) — now **16** Hermes seats
- [PRODUCT_FINISH_PLAYBOOK.md](PRODUCT_FINISH_PLAYBOOK.md) — product finish automation
- [HERMES_AGENT_REPOS.md](HERMES_AGENT_REPOS.md) — seat learnings → SOP promotion path
