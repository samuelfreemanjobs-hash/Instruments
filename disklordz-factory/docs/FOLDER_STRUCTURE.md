# DISKLORDZ filesystem layout (studio / NAS)

Operational files live outside git; this tree is the canonical layout.

```text
DISKLORDZ/
├── 00_HQ/
│   ├── Strategy/
│   ├── Brand/
│   ├── Finance/
│   └── SOPs/
├── 01_MARKET_INTELLIGENCE/
├── 02_ARTISTS/
│   ├── DL001/
│   ├── DL002/
│   └── ...
├── 03_MUSIC/
│   ├── Ideas/
│   ├── Production/
│   ├── Stems/
│   ├── Mixes/
│   └── Masters/
├── 04_SOUND_LIBRARY/
│   ├── Drums/
│   ├── 808/
│   ├── Synth/
│   ├── FX/
│   └── Vocals/
├── 05_SAMPLE_PRODUCTS/
│   ├── OneShots/
│   ├── Loops/
│   ├── Kits/
│   ├── Presets/
│   └── MIDI/
├── 06_VISUALS/
│   ├── Characters/
│   ├── Artwork/
│   ├── Thumbnails/
│   └── Video/
├── 07_CONTENT/
│   ├── YouTube/
│   ├── Shorts/
│   ├── TikTok/
│   └── Instagram/
├── 08_DISTRIBUTION/
├── 09_CATALOG/          # exports / snapshots synced with factory DB
├── 10_ANALYTICS/
├── 11_SOFTWARE/
│   ├── Plugins/
│   ├── Tools/
│   └── Factory/         # this repository path
└── 12_ARCHIVE/
```

Catalog IDs in the database (`DL-TRK-000001`, etc.) map to paths under `03_MUSIC`, `04_SOUND_LIBRARY`, and `05_SAMPLE_PRODUCTS`.
