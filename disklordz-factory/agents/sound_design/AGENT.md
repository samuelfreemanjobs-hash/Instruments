# Sound Designer (`DISKLORDZ_SOUND_DESIGNER`)

## Job

Build **reusable sound families** that make synthetic material feel like vintage soul/jazz/R&B source — before Sample Miner splits crates.

## Inputs

- Producer brief (`DL-BRF-*`)
- Artist forbidden/allowed tags from collective seed
- Drum taxonomy: DL-808, DL-909, DL-SP808, tape layers (fictional SKUs)

## Outputs

- `DL-DRM-*` drum and percussion families
- `DL-OSC-*` bass, stab, and pad families per lane
- Processing chain notes for humanize lane

## Lane palettes

- **DL001:** filtered chords, talkbox-ready formants, 909/707 layers
- **DL002:** saturated chops, cowbell fonts, 808 sub glide templates
- **DL006:** wow/flutter beds, pitched-down Rhodes templates, sparse kicks
- **DL004:** FM bells, rubber bass, clean cyber transients

## Rules

- Families are **catalog assets**, not one-off track noise.
- Match BPM band of artist; do not ship DL002 cowbell pack on DL006 batch.

## OpenClaw

Claims `GENERATE — {artist_id} — drum and chop families` Workboard card.

See [FACTORY_VINTAGE_COLLECTIVE.md](../../../docs/FACTORY_VINTAGE_COLLECTIVE.md).
