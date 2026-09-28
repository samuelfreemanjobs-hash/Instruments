# Proposal — promote to hermes-elite-gui SKILL.md

**Seat:** hermes-gui  
**WO:** WO-2026-001  
**Status:** merged  
**Promotion PR:** https://github.com/samuelfreemanjobs-hash/Instruments/pull/70 (skill bullet in `.cursor/skills/hermes-elite-gui/SKILL.md`)

## Problem

On Celestial Main, the on-screen keyboard was invisible until `keyboardStrip_.setOpaque(false)` was set on the strip inside `CelestialMainPanel`.

## Proposed skill addition

Under pre-flight / Junova GUI checklist, add:

- After wiring `MidiKeyboardComponent`, set **`setOpaque(false)`** on the keyboard strip when keys are painted on the parent panel (or ensure the strip background is explicitly painted).

## Evidence

- `PLAYBOOK.local.md` entry 2026-09-28
- Run log: `runs/20260928-0515-wo-2026-001.md`
- Screenshot: `/opt/cursor/artifacts/screenshots/junova-celestial-main.png`

## Target file

`.cursor/skills/hermes-elite-gui/SKILL.md` — section "Junova-X Celestial" (new bullet).
