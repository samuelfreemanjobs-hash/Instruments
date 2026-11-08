# Autonomous Coding Agent (24/7)

## Purpose

**Fleet agent `autonomous-coding-agent`** drains a **coding queue** around the clock: docs, Python, bridge scripts, Antigravity dispatch — and **escalates** JUCE/DPF C++ to the factory lane. Samuel runs the business; this agent keeps the repo moving.

## Build & run

| Layer | Command |
|-------|---------|
| **Scheduler (GitHub)** | `.github/workflows/autonomous-coding-agent.yml` — every 6h |
| **Scheduler (Cursor)** | Timer `autonomous-coding-agent-24-7` — every 4h Cloud run |
| **Tick (deterministic)** | `python3 scripts/autonomous-coder/tick.py` |
| **Cloud slice** | `python3 scripts/autonomous-coder/tick.py --emit-slice` → read `.cursor/autonomous-coder/NEXT_SLICE.md` |

## Data flow

```text
coding-queue.json
       │
       ▼
tick.py ──► antigravity sync (shell)
       │
       ├──► NEXT_SLICE.md ──► Cursor Cloud (autonomous-coding-agent skill)
       │
       └──► factory-escalation/*.md ──► plugin-factory-liaison / issue
```

## Threading / realtime

Async only. Not for audio realtime.

## Key modules

| Path | Role |
|------|------|
| `coding-queue.json` | Ordered work items |
| `state.json` | Completed ids, last tick |
| `allowlist-paths.json` | Writable path prefixes |
| `../agents/profit/autonomous-coding-agent/` | Agent soul, skill |
| `../../scripts/autonomous-coder/tick.py` | Queue runner |

## Extension points

Add queue items with `owner: tick|cloud|antigravity`. Cloud items must cite acceptance and allowlisted paths.

## Related docs

- [docs/AUTONOMOUS_CODING_AGENT.md](../../docs/AUTONOMOUS_CODING_AGENT.md)
- [ANTIGRAVITY_CLAUDE_CODE_COPILOT.md](../../docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md)
- [CURSOR_FACTORY_LIAISON.md](../../docs/CURSOR_FACTORY_LIAISON.md)
