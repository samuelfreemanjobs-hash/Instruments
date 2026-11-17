# Converter Night — W2 native render (384)

**Date:** 2026-11-17  
**Genre:** memphis-tape · **Plan:** converter-night · **SKU:** CNV  
**Renderer:** `native/build/drumsmith-render` (g++-12)

| Gate | Result |
|------|--------|
| Samples | **384** (3× prior 128 guide; factory ceiling 512) |
| technical_qa | **PASS** |
| diversity | **PASS** (0 duplicate SHAs) |
| release_ready | **false** |

Follows storefront/bank guide with RATE/EDGE/ROMP hero weight, scaled 3×. Refs remain analysis-only.

## Honest limits

- Original synthesis — not ROM dumps from the 26 archives.
- RATE/BIT12 still technique-led until DR-660/SP-1200 WAV refs.
- Recipe engine salts by `variant` + `cycle` so 808/kick/FX stay unique past 16.

## Loops

**36** true 2-bar loops via `render_loop_from_blueprint.py` (`--variant` master offset): 18 rhythm + 18 top.
