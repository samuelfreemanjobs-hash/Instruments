#!/usr/bin/env python3
"""Generate elite agent file trees from _specs.json (Claude Code blueprint SOP)."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SPECS = ROOT / "_specs.json"
REQUIRED_FILES = (
    "agent.md",
    "skill.md",
    "subagents.md",
    "soul.md",
    "hooks.md",
    "memory.md",
    "tools.md",
    "loop.md",
)


def subagents_md(spec: dict) -> str:
    aid = spec["id"]
    return f"""# Sub-agents — {spec["title"]}

Coordinator/worker split per Claude Code coordinator mode.

| Sub-agent | Model | Tools | Role |
|-----------|-------|-------|------|
| `{aid}-explore` | haiku | Read, Grep, Glob | Gather repo + API evidence only |
| `{aid}-verify` | inherit | Read, Bash | Run verification scripts; no writes |
| `{aid}-implement` | inherit | Read, Edit, Write | Apply minimal diff after coordinator plan |

## Delegation rules

1. Coordinator **never** delegates understanding — pass exact paths, env names, and API routes.
2. Messages delivered **between tool rounds** only (command queue invariant).
3. `{aid}-verify` must run `disklordz/integrations/scripts/verify-integrations.sh` when touching SaaS routes.
4. Max fan-out: 3 workers per turn unless user approves.

## Fork agents (cache-safe)

When summarizing long transcripts, use fork with **byte-identical** tool array and placeholder tool results (see `loop.md`).
"""


def soul_md(spec: dict) -> str:
    lines = spec.get("soul") or []
    body = "\n".join(f"- {line}" for line in lines)
    return f"""# Soul — {spec["title"]}

Identity and non-negotiables for this agent.

## Principles

{body}

## Voice

- Direct, operator-grade, no hype.
- Lead with evidence (command output, API JSON, file citations).
- Disklordz lanes: phonk, drift, cyber funk, screw, French touch — use corpus terms only when retrieved.

## Trust hierarchy

User instructions → `AGENTS.md` → product `ARCHITECTURE.md` → this soul → skill.md.

## META charter (normative)

Obey [`disklordz/agents/charter/META_CHARTER.md`](../../charter/META_CHARTER.md) (META v3.1). Tag evidence R8: `[executed]` / `[inspected]` / `[assumed]`. R10 gates: Supabase prod migrations, Stripe writes, API breaks, force-push — require explicit human confirmation. Skills `/zero-pause`, `/weave`, `/premortem` are **explicit invoke only**.
"""


def hooks_md(spec: dict) -> str:
    return f"""# Hooks — {spec["title"]}

Lifecycle interceptors (27-event model; implement v0 with these).

| Event | Type | Action |
|-------|------|--------|
| SessionStart | command | `python3 disklordz/rag/scripts/chunk_corpus.py` (if docs changed) |
| UserPromptSubmit | prompt | Reject prompts asking to exfiltrate secrets or disable RLS |
| PreToolUse | command | Block `Bash(rm -rf`, `Bash(curl * | bash`, force-push git |
| PreToolUse | prompt | For Stripe/Supabase **writes**, require explicit user confirmation |
| PostToolUse | command | Append last command to `~/.claude/projects/disklordz/memory/{spec["id"]}_session.log` |
| Stop | prompt | If task incomplete, list remaining steps before allowing stop |

## Snapshot

Freeze hook config at session start; update only via user `/hooks` or repo change + restart.

## Exit codes (command hooks)

- `0` success
- `2` block
- other = warning only
"""


def memory_md(spec: dict) -> str:
    return f"""# Memory — {spec["title"]}

File-tier persistence (no DB). Layout under repo or `~/.claude/projects/disklordz/memory/`.

| File | Type | Purpose |
|------|------|---------|
| `MEMORY.md` | index | ≤200 lines; pointers only |
| `{spec["id"]}_learnings.md` | feedback | Validated runbooks |
| `{spec["id"]}_incidents.md` | project | Dated outages / misconfigs |

## Frontmatter template

```yaml
---
name: {spec["title"]} incident 2026-03-28
description: One-line for LLM recall
type: feedback
---
```

## Recall

Tier 1: always load MEMORY.md index. Tier 2: LLM picks ≤5 files from manifest using current query + recent tool history.

## Do not memorize

Anything derivable from git log, `ARCHITECTURE.md`, or Stripe dashboard — retrieve instead.
"""


def tools_md(spec: dict) -> str:
    tools = spec.get("tools") or []
    mcp = spec.get("mcp") or []
    tool_list = ", ".join(tools) if tools else "(MCP / external only)"
    mcp_list = ", ".join(mcp) if mcp else "none"
    return f"""# Tools — {spec["title"]}

## Allowlist

Built-in: {tool_list}

MCP: {mcp_list}

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

{spec.get("upstream_repo", "")}

## Automation entrypoints

{spec.get("automate", "")}

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
"""


def loop_md(spec: dict) -> str:
    return f"""# Loop SOP — {spec["title"]}

AsyncGenerator control plane for this agent (pseudocode).

```text
async function* query(state):
  while not done:
    state = compress(state)     # snip → microcompact → collapse → autocompact
    response = await stream(model, state)
    yield response.messages
    if not response.tool_calls:
      if stop_hook_allows: return completed
      continue
    batches = partition(response.tool_calls)  # per-invocation safety
    for batch in batches:
      results = await execute_batch(batch)    # 14-step pipeline
      yield results.messages
      state += results
```

## Terminals (discriminated)

`completed` | `max_turns` | `aborted_tools` | `prompt_too_long` | `permission_denied` | `hook_stopped`

## Compression budget

Trigger autocompact at `window - 13000` tokens; hard stop at `window - 3000`.

## Invariants

- Every `tool_use` paired with `tool_result` before next model call.
- Cancel: synthetic results for queued tools.
- Cost: track per-turn; sub-agents roll up to parent session.

## Primary verify command

```bash
bash disklordz/integrations/scripts/verify-integrations.sh
# or agent-specific checks in skill.md
```
"""


def skill_md(spec: dict) -> str:
    return f"""---
name: disklordz-{spec["id"]}
description: {spec["description"]}
when_to_use: Profit lever — {spec.get("profit_lever", "")}. Invoke for Disklordz {spec["title"]} tasks.
disable-model-invocation: false
context: fork
paths:
  - "disklordz/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/**"
---

# Skill — {spec["title"]}

## When to use

{spec["description"]}

## Prerequisites

- Read `AGENTS.md`, `disklordz/website/ARCHITECTURE.md`, and `disklordz/agents/profit/{spec["id"]}/agent.md`.
- Run automation: {spec.get("automate", "n/a")}

## Success criteria

- [ ] Evidence attached (logs, curl, CI link)
- [ ] No secrets in git
- [ ] `npm run build` in `disklordz/website` when SaaS touched

## Anti-patterns

- Skipping ARCHITECTURE.md
- Stripe/Supabase writes without confirmation
- Unbounded sub-agent fan-out
"""


def agent_md(spec: dict) -> str:
    tools_yaml = json.dumps(spec.get("tools") or [])
    mcp = spec.get("mcp") or []
    mcp_block = ""
    if mcp:
        mcp_block = f"mcpServers: {json.dumps(mcp)}\n"
    return f"""---
description: "{spec["description"]}"
tools: {tools_yaml}
model: {spec.get("model", "inherit")}
permissionMode: {spec.get("permission_mode", "default")}
maxTurns: {spec.get("max_turns", 40)}
skills:
  - disklordz-{spec["id"]}
{mcp_block}hooks:
  PreToolUse:
    - type: command
      command: "echo pre-tool ${{TOOL_NAME}}"
---

# Agent — {spec["title"]} (`{spec["id"]}`)

You are the **{spec["title"]}** profit agent for **Disklordz Drum SaaS**.

## Mission

{spec["description"]}

**Profit lever:** {spec.get("profit_lever", "")}

## Operating context

- Monorepo: Instruments — SaaS root `disklordz/website/`
- Integrations hub: `disklordz/integrations/`
- Activate prod: `bash disklordz/integrations/scripts/activate-integrations.sh --all`

## Query loop

Follow `loop.md` in this directory. Push complexity to boundaries (MCP, hooks, permissions); keep the loop typed and testable.

## Charter

[`disklordz/agents/charter/META_CHARTER.md`](../../charter/META_CHARTER.md) binds all turns. Deviations: `OVERRIDE(R#): reason` (META-0); never override R10 on prod/Stripe/schema.

## Sub-agents

See `subagents.md`. Delegate exploration and verification; you synthesize.

## Memory & soul

Load `memory.md` layout at session start. Obey `soul.md` non-negotiables.

## Output format

1. **Finding** (1–3 bullets)
2. **Actions taken** (commands / files)
3. **Profit impact** (conversion, MRR, cost, or risk)
4. **Next automation** (script or workflow name)
"""


def write_agent(spec: dict) -> None:
    dest = ROOT / spec["id"]
    dest.mkdir(parents=True, exist_ok=True)
    files = {
        "agent.md": agent_md(spec),
        "skill.md": skill_md(spec),
        "subagents.md": subagents_md(spec),
        "soul.md": soul_md(spec),
        "hooks.md": hooks_md(spec),
        "memory.md": memory_md(spec),
        "tools.md": tools_md(spec),
        "loop.md": loop_md(spec),
    }
    for name, content in files.items():
        (dest / name).write_text(content, encoding="utf-8")


def build_registry(specs: dict) -> dict:
    agents = []
    for s in specs["agents"]:
        agents.append(
            {
                "id": s["id"],
                "title": s["title"],
                "profit_lever": s.get("profit_lever"),
                "path": f"disklordz/agents/profit/{s['id']}",
                "entry": f"disklordz/agents/profit/{s['id']}/agent.md",
                "skill": f"disklordz/agents/profit/{s['id']}/skill.md",
                "upstream_repo": s.get("upstream_repo"),
            }
        )
    return {"version": specs["version"], "agent_count": len(agents), "agents": agents}


def check_trees() -> int:
    data = json.loads(SPECS.read_text(encoding="utf-8"))
    errors = 0
    for spec in data["agents"]:
        d = ROOT / spec["id"]
        for fname in REQUIRED_FILES:
            if not (d / fname).is_file():
                print(f"missing: {d / fname}", file=sys.stderr)
                errors += 1
    return errors


def publish_github_skills(spec: dict) -> None:
    repo_root = ROOT.parents[1]
    dest_dir = repo_root / ".github" / "skills" / f"disklordz-{spec['id']}"
    dest_dir.mkdir(parents=True, exist_ok=True)
    content = (ROOT / spec["id"] / "skill.md").read_text(encoding="utf-8")
    (dest_dir / "SKILL.md").write_text(content, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="Verify file trees only")
    parser.add_argument(
        "--publish-skills",
        action="store_true",
        help="Copy skill.md into .github/skills/disklordz-<id>/SKILL.md",
    )
    args = parser.parse_args()
    if args.check:
        sys.exit(check_trees())

    data = json.loads(SPECS.read_text(encoding="utf-8"))
    for spec in data["agents"]:
        write_agent(spec)
        print(f"Wrote {spec['id']}/ ({len(REQUIRED_FILES)} files)")
        if args.publish_skills:
            publish_github_skills(spec)
            print(f"  → .github/skills/disklordz-{spec['id']}/SKILL.md")
    registry = build_registry(data)
    (ROOT / "registry.json").write_text(json.dumps(registry, indent=2) + "\n", encoding="utf-8")
    print(f"Updated registry.json ({registry['agent_count']} agents)")


if __name__ == "__main__":
    main()
