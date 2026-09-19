# DLRROM01 — factory PCM expansion library

## Origin

Factory audio is **synthesized at build time** by the four **[DisklordzSynth](../../DisklordzSynth/)** engines. The rompler ships **16 separate raw ROM files** (`pcm_bank_00.dlrrom` … `pcm_bank_15.dlrrom`), modeled after workstation expansion cards (Fantom SRX / Triton EXB / Motif PLG-style **multi-ROM** libraries).

Each card contains:

- **32 multisample programs** (8 per engine × 4 engines)
- **12 zones** per program (roots 28–84)
- **64 standalone** waves (set id 0) for one-shots and future mapping

**~448 waves per card × 16 cards ≈ 7,168 waves** in the embedded factory library.

| Rompler tone | Engine family | Program ids on a card |
|--------------|---------------|------------------------|
| 0 | additive | 1–8 |
| 1 | karplus | 9–16 |
| 2 | wave (looped) | 17–24 |
| 3 | subtractive | 25–32 |

Presets pick **ROM card** (`toneRomBank`) and **program variant** (`toneProgram` 0…7) per layer.

## File layout (v2 header)

```text
RomHeader     (magic DLRROM01, version 2, bankIndex, category, waveCount, pcm pool size)
WaveEntry[]
float pcm[]
```

See `DisklordzSynth/include/disklordz/RawRomFormat.h` and `Source/Assets/RawRomBank.h`.

## Manual regeneration

```bash
cmake --build build -j --target DisklordzSynth_BuildRawRom DisklordzRompler_GenerateRom
./build/DisklordzSynth/DisklordzSynth_BuildRawRom /tmp/pcm_roms
ls /tmp/pcm_roms/pcm_bank_*.dlrrom | wc -l   # 16
```

Build output is **16 loose `.dlrrom` files** (~12 MB each, ~192 MB total library). They are copied into the VST3/Standalone bundle at `Resources/DisklordzRom/` (workstation-style swappable PCM cards). Optional **`DisklordzSynth_BundleRawRoms`** builds a DLRROMCAT shipping archive for sample-pack products.

## Future

- P1: user-imported `.dlrrom` packs and card slots
- P2: SaaS-generated expansion ROM downloads
