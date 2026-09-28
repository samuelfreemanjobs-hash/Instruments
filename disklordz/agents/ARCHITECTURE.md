# Disklordz AI agent fleet

## Purpose

Fifteen **profit agents** with Claude-Code-style file trees (`agent.md`, `skill.md`, `subagents.md`, `soul.md`, …) for Cursor, Claude Code, Copilot skills, and Cloud Agents working on Disklordz.

## Build & run

```bash
# Regenerate trees from specs (after editing profit/_specs.json)
python3 disklordz/agents/profit/scaffold_agents.py

# Validate fleet registry
python3 disklordz/agents/profit/scaffold_agents.py --check
```

Load any agent in Claude Code: copy frontmatter from `disklordz/agents/profit/<id>/agent.md` into `.claude/agents/<id>.md`, or point Cloud Agent at `AGENTS.md` + the agent `skill.md`.

## Data flow

```text
User / WO / CI event
  → skill.md (when_to_use) selects agent
  → agent.md system prompt + tools.md allowlist
  → loop.md query() SOP (compress → model → tools → yield)
  → subagents.md for fork/delegate
  → memory.md file-tier persistence
  → hooks.md PreToolUse / Stop gates
```

## Key modules

| Path | Role |
|------|------|
| [`profit/`](profit/) | 15 profit agents, one directory each |
| [`profit/registry.json`](profit/registry.json) | Machine index |
| [`profit/scaffold_agents.py`](profit/scaffold_agents.py) | Generator (source of truth: `_specs.json`) |
| [docs/DISKLORDZ_PROFIT_AGENTS.md](../../docs/DISKLORDZ_PROFIT_AGENTS.md) | Human guide |
| [docs/DISKLORDZ_PROFIT_AGENTS_EXTENDED.md](../../docs/DISKLORDZ_PROFIT_AGENTS_EXTENDED.md) | 15 additional agent ideas |

## Extension points

Add an agent: extend `profit/_specs.json`, run `scaffold_agents.py`, update `registry.json`.

## Related docs

- [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](../../docs/DISKLORDZ_ILLUGEN_RESEARCH.md)
- [disklordz/integrations/ARCHITECTURE.md](../integrations/ARCHITECTURE.md)
