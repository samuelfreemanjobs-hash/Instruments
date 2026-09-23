# Product REV2-TRAP-128 — Prophet Rev2–style trap bank

**Target:** 128 **original** trap multisamples — heavy **Prophet Rev2 vibe** (ladder filter, dual osc), influenced by Atlanta trap production aesthetics **without** copying specific records.

## Generator (primary)

**Rev2 Trap VSTi** in [`Rev2Trap/`](../../Rev2Trap/ARCHITECTURE.md):

- 128 factory programs (procedural lanes: Jeezy / Shawty Redd / Gucci / neutral × categories)
- Headless: `Rev2TrapOfflineRender` → multisample WAVs

```bash
cmake --build build -j --target Rev2TrapOfflineRender
python3 disklordz/sound-factory/scripts/run_product_batch.py \
  --catalog disklordz/sound-factory/catalog/rev2_trap128_catalog.json \
  --out disklordz/sound-factory/out \
  --engine rev2trap --limit 32 --offset 0
```

`--engine auto` uses Rev2Trap when the binary exists.

## Exports (every slot)

- `instrument-map.json`, `.sfz`, `project.json`, `HANDOFF_MPC.md`
- QA: `factory-qa-audio` skill / `factory_qa.py`

## Python fallback

If JUCE binary missing, batch falls back to `trap_synth.py` (legacy).

## Play the synth

```bash
cmake --build build -j --target Rev2Trap_Standalone
```

## Roadmap

[docs/FACTORY_IMPROVEMENT_ROADMAP.md](../FACTORY_IMPROVEMENT_ROADMAP.md)

## Agent skill

`.cursor/skills/product-rev2-trap128/SKILL.md`
