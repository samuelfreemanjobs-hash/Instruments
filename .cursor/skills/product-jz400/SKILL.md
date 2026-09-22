---
name: product-jz400
description: "Build the JZ400 Young Jeezy-type 400-instrument multisample product. Catalog seeding, chunked batch render, SFZ+MPC, QA subagent."
---

# Product JZ400

Read [docs/products/JZ400.md](../../../docs/products/JZ400.md).

## Steps

1. `seed_jz400_catalog.py --target 400`  
2. Expand `catalog/jz400_albums.json` for mixtapes (WO).  
3. Chunked `run_product_batch.py --offset N --limit 50`.  
4. **factory-qa-audio** on each chunk.  
5. **export-sfz-mpc-project** verified per slot.  
6. Optional YouTube ref per `referenceQuery` before re-render.  

## Non-goals

- No sampling from commercial masters.  
- Drums excluded except **bass_808** archetype.  

## Orchestrator

Uses `instrument-orchestrator` + `pm-airtable-factory-trigger` for WO-driven chunks.
