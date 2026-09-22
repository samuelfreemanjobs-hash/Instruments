---
name: product-rev2-trap128
description: "Build REV2-TRAP-128: 128 Prophet-style trap multisamples (Jeezy, Shawty Redd, Gucci lanes). SFZ, QA, future VSTi port."
---

# Product REV2-TRAP-128

Read [docs/products/REV2_TRAP128.md](../../../docs/products/REV2_TRAP128.md).

## Steps

1. `seed_rev2_trap128_catalog.py`  
2. `run_product_batch.py --limit 32` × 4 chunks  
3. **factory-qa-audio** each chunk  
4. SFZ + MPC handoff via **export-sfz-mpc-project**  
5. VSTi Phase B: JUCE + [tubernard/synth-emulator](https://github.com/tubernard/synth-emulator) research WO  

## Lanes

`trap_synth.LANE_PATCH`: `jeezy`, `shawty_redd`, `gucci`, `neutral`.
