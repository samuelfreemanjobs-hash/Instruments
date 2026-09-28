---
name: hermes-elite-handoff
description: Hermes handoff — Antigravity inbox, local IDE ↔ Cloud, git HO JSON. No remote IDE control.
---

# Hermes elite handoff

1. Read `disklordz/antigravity/ARCHITECTURE.md`, `docs/HISE_ANTIGRAVITY_LANE.md`.
2. Draft: `python3 disklordz/hermes/scripts/hermes_tool.py handoff draft --wo WO-2026-HISE-001 --title "…"`.
3. Send (user confirms): `./scripts/antigravity-bridge/antigravity-bridge.sh send --wo … --title … --push`.
4. Local IDE (VS Code/Cline): document branch name + paths; never exfiltrate secrets via handoff files.
5. Outbox review: `disklordz/antigravity/inbox/`, `disklordz/hermes/outbox/`.

Prefix WOs: `[Plugin][HISE]` for Antigravity lane.
