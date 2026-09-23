---
name: audio-generation-batch
description: "Generate drum kits and product packs in batch using sound-factory Python and/or website generation libs. Use for SKU batches, variations, and manifest-compliant ZIP outputs."
---

# Audio generation batch

## Sources of truth

- Manifest types: `disklordz/website/src/lib/manifest.ts`
- Generation spec: `disklordz/website/src/lib/generation/generation-spec.ts`
- Offline CLI: `disklordz/sound-factory/scripts/generate_kit.py`, `generate_stub_kits.py`
- Online factory: `disklordz/website/src/lib/generation/factory.ts`, `product-factory.ts`, `variations.ts`

## Batch workflow

1. Normalize input to **GenerationSpec** + `presetId` / artist lane (RAG: `python3 disklordz/rag/scripts/query_local.py`).
2. For offline batch: run `generate_kit.py` with `--out` under `disklordz/sound-factory/out/<jobId>/`.
3. For web parity: prefer shared param tables (`prompt-params.ts` ↔ Python `PRESET_BASE`).
4. Variations: use `variations.ts` patterns or N seeds in Python with distinct `kitId` / `variationBatchId`.
5. Write `manifest.json` matching `DISKLORDZ_DRUM_KIT_MANIFEST` or `DISKLORDZ_PRODUCT_PACK_MANIFEST`.
6. Every sample: compute SHA-256; set `provenance` and `sourceId`.

## Commands

```bash
python3 disklordz/sound-factory/scripts/generate_stub_kits.py
python3 disklordz/sound-factory/scripts/generate_kit.py --preset mpc-ready-808 --out /tmp/kit-out
cd disklordz/website && npm ci && npm run build
```

## Fail closed

- Invalid schema, missing hash, or empty WAV → stop; do not publish ZIP.
