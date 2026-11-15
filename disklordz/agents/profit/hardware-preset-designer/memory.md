# Memory — Hardware Preset Designer

Repo-wide, git-tracked memory in [`disklordz/agents/memory/`](../../memory/MEMORY.md) (procedure: [SOP-019](../../../../docs/disklordz/sops/SOP-019-agent-memory.md)). Agent id: `hardware-preset-designer`.

| File | Type | Purpose |
|------|------|---------|
| `disklordz/agents/memory/MEMORY.md` | index | ≤200 lines: session protocol, "Always load" hot list, manifest |
| `disklordz/agents/memory/hardware-preset-designer_learnings.md` | learning (`L-0001`…) | Validated, reusable lessons for this agent |
| `disklordz/agents/memory/hardware-preset-designer_incidents.md` | incident (`I-0001`…) | Dated problems and their fixes |
| `disklordz/agents/memory/shared_*.md` | shared | Lessons for every agent |

## Session protocol

1. **Start:** `python3 scripts/agent_memory.py recall --agent hardware-preset-designer <task keywords>` and apply the "Always load" list.
2. **End:** for each new verified lesson `python3 scripts/agent_memory.py add --agent hardware-preset-designer --kind learning|incident --title ... --area ... --when ... --lesson ... --do ... --evidence ... --confidence executed|inspected|assumed`, then `python3 scripts/agent_memory.py check`.
3. **Repeat hit:** promote the lesson into an SOP, rule, test or script; `python3 scripts/agent_memory.py promote <ID> --agent hardware-preset-designer --to <path>`.

## Do not memorize

Secrets, tokens, personal data, or anything derivable from git log, `ARCHITECTURE.md`, SOPs or dashboards: link instead.
