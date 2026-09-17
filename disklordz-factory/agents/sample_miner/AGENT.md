# Sample Miner (`DISKLORDZ_SAMPLE_MINER`)

## Job

Turn approved **parent tracks** into **Supply** products — crate splits aligned to the vintage collective model.

## Inputs

- `DL-TRK-*` with `artist_id` (DL001, DL002, DL006, DL004)
- Music Producer brief (`DL-BRF-*`) with vintage source tags
- Brand SKU prefix from `artist_collective_seed.json`

## Outputs (crate split standard)

| Slot | Description |
|------|-------------|
| Hook loop | 8-bar hero sample behavior |
| Drum ghost | Break-derived one-shots |
| Harmony stems | Keys / Rhodes / strings MIDI map |
| Bass family | DI + saturated variant |
| FX bed | Tape/noise layer when DL006 brief requires screw |

Asset kinds: `DL-KIT-*`, MIDI maps, one-shot packs, preset bundles.

## Rules

- **Lane isolation:** Do not publish one kit marketed for both Midnight Circuit and Disklordz.
- **Rights:** Every kit inherits parent `rights_status`; block extract if parent is `draft` or failed QA.
- **Naming:** `{sku_prefix}{descriptor}` e.g. `disklordz-screw-chop-kit`.

## OpenClaw

Claims Workboard card `PACKAGE — {artist_id} — crate split kits`; attaches kit manifest as proof artifact.

See [FACTORY_VINTAGE_COLLECTIVE.md](../../../docs/FACTORY_VINTAGE_COLLECTIVE.md).
