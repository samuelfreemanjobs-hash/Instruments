---
name: hise-antigravity-handoff
description: "Assign HISE sketch work via Antigravity git bridge. Use when the deliverable is HiseScript, sample maps, or Windows VST3 export — not JUCE CI."
---

# HISE Antigravity handoff

Cloud Agent **cannot** run HISE/MSVC locally.

## Docs

- [docs/HISE_ANTIGRAVITY_LANE.md](../../../docs/HISE_ANTIGRAVITY_LANE.md)
- [docs/HISE_SKETCH_LANE.md](../../../docs/HISE_SKETCH_LANE.md)
- [disklordz/antigravity/ARCHITECTURE.md](../../../disklordz/antigravity/ARCHITECTURE.md)

## Send work

```bash
./scripts/antigravity-bridge/antigravity-bridge.sh send --wo WO-... --title "..." --push
```

## Deliverables in handoff

- Brief + InstrumentMapSpec or SFZ
- Sample list with roots/velocity
- Acceptance: local `.vst3` + 1-pager sonic summary

## Prefix

Airtable / WO: `[Plugin][HISE]` · owner `antigravity-hise`
