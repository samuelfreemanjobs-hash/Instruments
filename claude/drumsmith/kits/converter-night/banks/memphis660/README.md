# MEMPHIS-660 oneshot bank (kit staging)

Original synthesized kick / snare / closed-hat oneshots for the **Converter Night** drum sample kit.

## Generate (local — WAV gitignored)

```bash
cd claude/drumsmith
python3 scripts/memphis_oneshots/batch_memphis_bank.py \
  --out kits/converter-night/banks/memphis660
```

Outputs PCM16 mono @ 44.1 kHz under `Kicks/`, `Snares/`, `Hi-Hats_Closed/` plus `manifest.json`.

## Product rules

- Commercial SKU = drum sample kit (oneshots + loops)
- No copyrighted PCM; no artist names on filenames
- `release_ready` stays false until HITL — do not merge into storefront ship tree without listening pass

## Related

- `scripts/memphis_oneshots/bank_recipe.json`
- `docs/GEM_MEMPHIS_660_KICK.md` · `docs/GEM_MEMPHIS_DR660_SNARE.md`
- Headless loop: `scripts/memphis_oneshots/render_memphis_loop.py`
