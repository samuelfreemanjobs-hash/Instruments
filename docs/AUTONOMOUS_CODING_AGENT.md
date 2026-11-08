# Autonomous Coding Agent (24/7)

DiskLordz **fleet agent #35** — `autonomous-coding-agent` — keeps coding and automation moving while you run the business.

## Stack

| Runtime | Role |
|---------|------|
| **GitHub Actions** | Every **6h**: Antigravity sync, queue tick, `NEXT_SLICE.md` |
| **Cursor Cloud + timer** | Every **4h**: read slice, one allowlisted commit, push |
| **Antigravity (Windows)** | Build/pytest/fixtures per queue dispatch |
| **Claude Code** | Terminal DPF — use `request-claude-support.sh` |
| **plugin-factory-liaison** | JUCE/C++ slices — this agent **escalates**, does not implement |

## Invoke

**Cursor / Copilot skill:** `disklordz-autonomous-coding-agent`

```bash
python3 scripts/autonomous-coder/tick.py --emit-slice
# Cloud: open .cursor/autonomous-coder/NEXT_SLICE.md and execute one slice
python3 scripts/autonomous-coder/tick.py --complete <queue-id>
```

## Queue

Edit `disklordz/autonomous-coder/coding-queue.json`. Writable paths: `allowlist-paths.json`.

## Safety

- No `Source/` / `DPF-*` C++ without factory escalation file.
- No Stripe / Supabase prod (R10).
- One slice, one commit per Cloud turn.

## Related

- [disklordz/autonomous-coder/ARCHITECTURE.md](../disklordz/autonomous-coder/ARCHITECTURE.md)
- [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md)
