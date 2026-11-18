# Converter Night — W2 native render (512)

**Date:** 2026-11-17  
**Genre:** memphis-tape · **Plan:** converter-night · **SKU:** CNV  
**Renderer:** `native/build/drumsmith-render` (g++-12)

| Gate | Result |
|------|--------|
| Samples | **512** (factory ceiling — drums @ 3× guide + full FX) |
| technical_qa | **PASS** |
| diversity | **PASS** (0 duplicate SHAs) |
| release_ready | **false** |

## FX families (230 native FX)

| Family | Count | Storefront |
|--------|------:|------------|
| Crash / ride | 40 | `01/…/Crashes_Rides` |
| Aliased | 48 | `02/…/Aliased_One_Shots` |
| Reverse / sweep | 40 | `02/…/Reverse_And_Sweeps` |
| Stab / impact | 40 | `02/…/Stabs_And_Impacts` |
| Texture | 26 | `04/…` |
| Loop slots | 36 | replaced by true 2-bar loops |

## Honest limits

- Original synthesis — not ROM dumps.
- RATE/BIT12 still technique-led until DR-660/SP-1200 WAV refs.
- Recipe engine FX family salts keep 230 FX variants unique.

## Loops

**36** true 2-bar loops (9+9 rhythm @ 94/134, 9+9 tops) including DR-660-technique + Memphis archetype blueprints.
