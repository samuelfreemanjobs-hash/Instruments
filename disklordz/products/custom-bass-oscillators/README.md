# DiskLordz Custom Bass Oscillators (commerce)

Sellable pack built from `src/oscillators/*` + `src/mkii/oscillators/*` (14 bass units only).

| Artifact | Path |
|----------|------|
| Product brief | [PRODUCT.md](PRODUCT.md) |
| Unit index | [catalog.json](catalog.json) |
| EULA draft | [legal/EULA_PLACEHOLDER.md](legal/EULA_PLACEHOLDER.md) |
| Bundle zip | `release/products/disklordz-custom-bass-oscillators-1.0.0.zip` |
| Single SKU zips (×14) | `release/products/singles/disklordz-<slug>-1.0.0.zip` |
| SKU table | `release/products/SKU_TABLE-1.0.0.md` |

```bash
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/package-disklordz-bass-product.sh
```

Wire Stripe / download on Disklordz.com after Sam approves EULA and hardware QA (see PRODUCT.md checklist).
