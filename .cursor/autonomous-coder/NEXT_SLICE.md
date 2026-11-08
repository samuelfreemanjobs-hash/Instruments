# Autonomous Coding Agent — NEXT SLICE

**Queue id:** `cloud-coach-script-hygiene`
**Title:** Verify coach_engine + coach_outreach imports and fix broken paths only

## Acceptance
python3 -m py_compile on touched files; no behavior change unless import error

## Allowlist (only these paths)
```json
[
  "disklordz/audio-command/scripts/",
  "disklordz/autonomous-coder/"
]
```

## Read first
- `disklordz/audio-command/ARCHITECTURE.md`

## Agent
- Skill: `disklordz-autonomous-coding-agent`
- Soul: `disklordz/agents/profit/autonomous-coding-agent/soul.md`

When done: `python3 scripts/autonomous-coder/tick.py --complete cloud-coach-script-hygiene`
