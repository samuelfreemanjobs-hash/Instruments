# SOP-HANDOFF-001 — Cursor ↔ Antigravity handoff

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-handoff |
| **Consumer seats** | hermes-handoff, hermes-architect |
| **Cadence** | per_hise_wo |

## Purpose

HISE / Antigravity work on Windows receives structured context from Cursor Hermes.

## Procedure

1. Draft handoff JSON:
   ```bash
   python3 disklordz/hermes/scripts/hermes_tool.py handoff draft \
     --wo WO-2026-HISE-001 --title "…" --criteria "…"
   ```
2. Prefer bridge send per [docs/HISE_ANTIGRAVITY_LANE.md](../../../docs/HISE_ANTIGRAVITY_LANE.md).
3. Cursor implements **JUCE port WOs** only; HISE sketch stays Antigravity executor.
4. Close loop: evidence back to GitHub issue; hermes-lead merges JUCE side.

## Verification

- `disklordz/hermes/outbox/HO-*.json` or bridge delivery log.
- WO id on both sides matches.

## Related

- [SOP-OPS-001](SOP-OPS-001-work-order-lifecycle.md)
