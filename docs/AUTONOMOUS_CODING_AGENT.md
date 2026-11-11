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

## When the queue is idle (`NEXT_SLICE.md` → `waiting`)

Cloud slices in `coding-queue.json` are finite. When all are **complete**, `--emit-slice` writes **waiting** — the agent may still refresh docs, `PASTE_TO_FACTORY.md`, and Antigravity **prep** handoffs (allowlisted only).

**Human gate:** until Coach marks a sprint id done (e.g. **`plugin-skeleton`** = Windows VST3 + DAW), do not add new C++ slices. Samuel: Actions **DiskLordz Audio (Windows VST3)** or `build-windows.ps1` on `main` ([CURSOR_OPERATOR_ON_20_PRO.md](CURSOR_OPERATOR_ON_20_PRO.md)).

Antigravity parallel work (e.g. **WO-AG-001** docs) does not block the VST proof.

## Safety

- No `Source/` / `DPF-*` C++ without factory escalation file.
- No Stripe / Supabase prod (R10).
- One slice, one commit per Cloud turn.

## Related

- [disklordz/autonomous-coder/ARCHITECTURE.md](../disklordz/autonomous-coder/ARCHITECTURE.md)
- [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md)
