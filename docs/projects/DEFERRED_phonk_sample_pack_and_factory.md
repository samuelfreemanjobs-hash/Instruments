# [DEFERRED] Disklordz phonk sample pack & factory

**Status:** Start later — not the active focus.  
**Owner:** Sam Freeman / Disklordz  
**Registered in PM:** Use `PM ADD: Phonk sample pack & factory` when Airtable is live (copy scope from below).

---

## What this project is

First commercial **OG Memphis phonk + gritty trap** drum kits under **Disklordz**, using:

- **Brand:** [DISKLORDZ_SAMPLE_PACK_BRAND.md](../DISKLORDZ_SAMPLE_PACK_BRAND.md)  
- **Hardware capture:** [DISKLORDZ_CAPTURE_LIST.md](../DISKLORDZ_CAPTURE_LIST.md) (Sector 01)  
- **Release process:** [DISKLORDZ_RUN_SHEET.md](../DISKLORDZ_RUN_SHEET.md)  
- **Tech architecture:** [DISKLORDZ_BLUEPRINT.md](../DISKLORDZ_BLUEPRINT.md)  
- **Batch tooling:** `tools/render_kit.py`, `tools/schema.sql` (PR [#12](https://github.com/samuelfreemanjobs-hash/Instruments/pull/12))

**Explicitly out of scope for this deferred track:** January “swarm” archives (Agent Zero, Ralph, A2UI, 15-agent Python, fake dashboards).

---

## Proposed outcome

Ship **one** real kit (e.g. **GRAVEYARD SHIFT**) + optional **Disklordz Drum** VSTi fork from `MyFirstSynth`.

---

## Suggested PM fields (when you add to Airtable)

| Field | Suggested value |
|--------|------------------|
| **Urgency** | LOW or MEDIUM until you unpause |
| **Status** | ON HOLD |
| **Current Phase** | PRE-PHASE-0 or PHASE-0 |
| **% Complete** | ~15 (docs + factory stub in repo) |
| **Stack** | JUCE 7 · Python tools · SQLite · Gumroad/BeatStars (TBD) |
| **Next Action** | Pick kit codename; record Sector 01 OR run `render_kit.py`; listen in DAW |
| **Scope** | Deferred phonk/trap pack: brand docs, offline WAV factory, future VSTi. No autonomous agents. |
| **Blockers** | Active priority is **Disklordz Audio PM Agent** + portfolio ops, not kit production |

---

## When to unpause

- PM briefing shows capacity  
- You set Status → ACTIVE and raise Urgency  
- Run Square 1 on [DISKLORDZ_RUN_SHEET.md](../DISKLORDZ_RUN_SHEET.md)

---

## Repo map (already built — do not delete)

```
docs/DISKLORDZ_*.md
docs/DISKLORDZ_BRAND_PROMPT.txt
tools/
MyFirstPlugin/   ← JUCE templates when VSTi phase starts
```
