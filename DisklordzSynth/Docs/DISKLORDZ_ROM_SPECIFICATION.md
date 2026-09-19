# DISKLORDZ ROM specification (DLRROM01)

Binary **wave ROM** — flat wave table + PCM pool (not pre-baked “instrument = one WAV”).

## File

- Magic: `DLRROM01`
- Version: 2 header (28 bytes): `bankIndex`, `bankCategory` (maps to ROM layer)
- `WaveEntry`: PCM offset, frames, loop, root MIDI, engine id, **multisampleSetId** (1–128; 0 = standalone wave)

## Cards

- Factory ships **16** `pcm_bank_XX.dlrrom` files (~448 waves/card procedural P0).
- Mapped to **5 ROM layers** (REAL … CYBER_SHIFT) via `romLayerForBankIndex()`.

## Relationship to database

| DB entity | ROM representation |
|-----------|---------------------|
| SampleAsset | One wave entry (or streaming ref in future v3) |
| Multisample | Shared `multisampleSetId` across zone waves |
| Tone | Default program + bank pointer |
| Patch | Not stored in ROM — lives in preset/patch catalog |

## Expansion ROM

Expansion packages may add `.dlrrom` cards referenced by manifest (`EXPANSION_FORMAT.md`). Content engine mounts without recompiling plugin.

## Do not

- Store finished “instrument” as a single interleaved WAV in ROM.
- Embed third-party workstation ROM data.

Implementation: `RawRomFormat.h`, `RawRomBuilder.cpp`, `MultisampleRomBuilder.cpp`.
