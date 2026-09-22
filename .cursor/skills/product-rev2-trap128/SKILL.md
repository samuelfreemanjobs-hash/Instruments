---
name: product-rev2-trap128
description: "Build REV2-TRAP-128: 128 Prophet-style trap multisamples (Jeezy, Shawty Redd, Gucci lanes). SFZ, QA, future VSTi port."
---

# Product REV2-TRAP-128

Read [docs/products/REV2_TRAP128.md](../../../docs/products/REV2_TRAP128.md).

## Steps

1. Build **`Rev2TrapOfflineRender`** (see `Rev2Trap/ARCHITECTURE.md`).  
2. `seed_rev2_trap128_catalog.py`  
3. `run_product_batch.py --engine rev2trap --limit 32` × 4 chunks  
4. **factory-qa-audio** subagent each chunk (mandatory)  
5. SFZ + MPC via **export-sfz-mpc-project**  

## Sound direction

**Original trap** + **Prophet Rev2 vibe** — no song references required. Tune programs in `Rev2Trap/Source/Presets/FactoryPresets.cpp`.

## Fallback

`--engine python` uses legacy `trap_synth.py` if VST binary unavailable.
