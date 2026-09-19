# DisklordzSynth — architecture

**Offline procedural sample engines** — kept as a **separate product line** from DISKLORDZ ROMPLER. You can ship/sell WAV packs, `.dlrom` banks, or license the static library without bundling the rompler VSTi.

Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md).

## Purpose

Four sellable offline engines ([Docs/ENGINES.md](Docs/ENGINES.md)):

| Engine | Technique |
|--------|-----------|
| `additive` | Harmonic partial sums + decay |
| `karplus` | Karplus–Strong / plucked strings |
| `wave` | Procedural single-cycle wavetable scan |
| `subtractive` | Saw/pulse through resonant LP-style filter |

## Build

```bash
cmake --build build -j --target DisklordzSynth_BuildRawRom DisklordzSynth_RenderWav DisklordzSynth_ExportWavPack DisklordzSynthTests
```

## CLI products (retained)

| Binary | Role |
|--------|------|
| `DisklordzSynth_BuildRawRom` | Write 16-card `DLRROM01` library (or single card with `--single`) |
| `DisklordzSynth_BundleRawRoms` | Optional DLRROMCAT archive of all `.dlrrom` cards |
| `DisklordzSynth_BuildFactoryPack` | Legacy `DLROMPR1` region pack (commercial tooling) |
| `DisklordzSynth_RenderWav` | Single-engine WAV export for sample packs |
| `DisklordzSynth_ExportWavPack` | Dump factory ROM waves as WAV folder |

## Library

`DisklordzSynth` static target — headers under `include/disklordz/`.

## Data formats

- **DLRROM01** — [include/disklordz/RawRomFormat.h](include/disklordz/RawRomFormat.h) (ROMPLER `RawRomBank`)
- **DLROMPR1** — [include/disklordz/DlromPackFormat.h](include/disklordz/DlromPackFormat.h) (legacy pack builder only)
- **WAV** — 24-bit mono via `WavWriter`

## Commercial use

See [Docs/COMMERCIAL.md](Docs/COMMERCIAL.md). All PCM is generated in-repo; no third-party samples.

## Related

- Consumer: [DisklordzRompler/ARCHITECTURE.md](../DisklordzRompler/ARCHITECTURE.md)
