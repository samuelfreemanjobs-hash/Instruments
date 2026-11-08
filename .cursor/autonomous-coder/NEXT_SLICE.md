# Autonomous Coding Agent — NEXT SLICE

**Queue id:** `cloud-factory-paste-human-windows`
**Title:** PASTE_TO_FACTORY human-windows for plugin-skeleton + idle NEXT_SLICE

## Acceptance
factory-queue.py emits Samuel build block; tick emit_waiting shows queue idle

## Allowlist (only these paths)
```json
[
  "scripts/audio-plugin/",
  "scripts/autonomous-coder/",
  "disklordz/audio-command/data/",
  ".cursor/autonomous-coder/"
]
```

## Read first
- `docs/CURSOR_FACTORY_LIAISON.md`

## Agent
- Skill: `disklordz-autonomous-coding-agent`
- Soul: `disklordz/agents/profit/autonomous-coding-agent/soul.md`

When done: `python3 scripts/autonomous-coder/tick.py --complete cloud-factory-paste-human-windows`
