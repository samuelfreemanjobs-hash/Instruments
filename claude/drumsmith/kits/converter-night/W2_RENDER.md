# Converter Night — W2 native render (128 max)

**Date:** 2026-11-17  
**Genre:** memphis-tape · **Plan:** converter-night · **SKU:** CNV  
**Renderer:** `native/build/drumsmith-render` (g++-12)

| Gate | Result |
|------|--------|
| Samples | **128** (factory ceiling) |
| technical_qa | **PASS** |
| diversity | **PASS** |
| release_ready | **false** |

Follows storefront/bank guide with RATE/EDGE/ROMP hero weight. Refs remain analysis-only.

## Honest limits

- Original synthesis — not ROM dumps from the 26 archives.
- RATE/BIT12 still technique-led until DR-660/SP-1200 WAV refs.
- Recipe engine now salts variants ≥16 so 808s stay unique (seed ignored by native 808).

## Loops

12 true 2-bar loops via `render_loop_from_blueprint.py` (6 rhythm + 6 top).
