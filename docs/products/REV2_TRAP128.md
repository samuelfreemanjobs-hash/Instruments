# Product REV2-TRAP-128 — Prophet Rev2–style trap bank

**Target:** 128 multisample programs blending **Young Jeezy**, **Shawty Redd**, and **Gucci Mane** trap synth aesthetics (clean-room).  
**Now:** Sample generator + SFZ + MPC handoff via `trap_synth.py` producer lanes.  
**Later:** JUCE VSTi port (see [docs/FACTORY_GITHUB_RESOURCES.md](../FACTORY_GITHUB_RESOURCES.md)).

## Catalog

```bash
python3 disklordz/sound-factory/scripts/seed_rev2_trap128_catalog.py
```

## Batch render

```bash
python3 disklordz/sound-factory/scripts/run_product_batch.py \
  --catalog disklordz/sound-factory/catalog/rev2_trap128_catalog.json \
  --out disklordz/sound-factory/out \
  --offset 0 --limit 128
```

## VSTi roadmap

Phase A (this repo): multisample factory + SFZ/MPC.  
Phase B: JUCE instrument loading SFZ or embedded zones; DSP upgrade toward Rev2 topology (dual osc + ladder filter). Reference UI/DSP sketch: [tubernard/synth-emulator](https://github.com/tubernard/synth-emulator).

## Agent skill

`.cursor/skills/product-rev2-trap128/SKILL.md`
