# Antigravity ↔ Cursor bridge

## Purpose

File-based and Git-based handoff between **Cursor Cloud Agent** (Linux, SaaS/JUCE factory) and **Google Antigravity IDE** (Windows, HISE lane). There is **no direct IDE remote session** — agents stay decoupled and safe.

## Build & run

| Side | Command |
|------|---------|
| **Cursor / Linux / macOS** | `./scripts/antigravity-bridge/antigravity-bridge.sh send --wo WO-2026-HISE-001 --title "..."` |
| **Windows (Antigravity)** | `.\scripts\antigravity-bridge\install-antigravity-workspace.ps1` then `.\scripts\antigravity-bridge\sync-handoff.ps1` |

## Data flow

```text
Cursor Cloud ──writes──► disklordz/antigravity/inbox/HO-*.json
        │                        │
        │ git push               │ git pull (Antigravity)
        ▼                        ▼
   GitHub repo ◄────────── Antigravity IDE (HISE build)
        ▲                        │
        │ git push               │ publish-handoff.ps1
        └──── outbox/HO-*.json ◄─┘
```

Optional: `gh issue comment` when `GH_ISSUE` is set in handoff.

## Threading / realtime

Handoffs are **async** (minutes/hours). Not for audio realtime.

## Key modules

| Path | Role |
|------|------|
| `handoff.schema.json` | JSON shape |
| `AGENT_INSTRUCTIONS.md` | Paste into Antigravity project rules |
| `inbox/` | Cursor → Antigravity |
| `outbox/` | Antigravity → Cursor |
| `../scripts/antigravity-bridge/` | CLI installers |

## Extension points

- Add Airtable WO pull to `send` subcommand.
- Slack notify on new inbox file (reuse `setup-disklordz-integrations.sh` webhook).

## Related docs

- [docs/HISE_ANTIGRAVITY_LANE.md](../../docs/HISE_ANTIGRAVITY_LANE.md)
- [docs/DISKLORDZ_SAAS_AGENT_LANES.md](../../docs/DISKLORDZ_SAAS_AGENT_LANES.md)
