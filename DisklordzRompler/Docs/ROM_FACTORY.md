# DISKLORDZ ROM FACTORY

Full platform map: [docs/DISKLORDZ_PLATFORM.md](../../docs/DISKLORDZ_PLATFORM.md) · Spec index: [DisklordzSynth/Docs/CONTENT_PLATFORM_INDEX.md](../../DisklordzSynth/Docs/CONTENT_PLATFORM_INDEX.md)

Commercial-scale rompler content is **not** 1,000 independent sampled instruments. It is a production pipeline:

```text
RAW WAV (8k–20k files)
        ↓  normalize · analyze · loop · root · velocity
MULTISAMPLE (~250–350 source instruments)
        ↓  map zones → DLRROM wave table
TONE (~500–1,000 ROM programs)
        ↓  4 layers + macros
PATCH / PRESET (1,000+ factory programs)
```

One source multisample (e.g. Concert Grand) fans out to many presets (Dark Piano, VHS Piano, Phonk Piano, …) via **tone stacking, macros, and naming** — same model as large workstation ROMplers (many patches, shared wave ROM).

## Five ROM layers (original content only)

| Layer | Role |
|-------|------|
| **REAL** | Own/licensed acoustic & electric recordings |
| **CLASSIC_DIGITAL** | Original 80s/90s/2000s-style digital timbres (not copied ROM) |
| **ANALOG** | Original analog-style synth waves |
| **LO_FI** | Tape, vinyl, cassette, degraded digital |
| **CYBER_SHIFT** | Disklordz signature: phonk, wave, Detroit, night drive |

Factory plugin cards `pcm_bank_00…15` map onto these layers (see `disklordz/RomFactorySpec.h`).

## Twenty instrument families

Acoustic piano, electric piano, keys, organ, bells/mallets, guitar, bass, 808/sub, synth bass, leads, plucks, pads, strings, brass, woodwinds, choir/vocal, percussive tonal, **texture**, **atmosphere**, FX.

Browser targets (from product plan): ~1,100 categorized presets built from ~350 sources.

## Sample tiers (ROM budget)

| Tier | Use | Typical WAV count / source |
|------|-----|----------------------------|
| **Hero** | Piano, rhodes, hero strings | 500–2,000 |
| **Core** | Most melodic instruments | 30–200 |
| **Synth** | Single-cycle / short ROM | 1–20 |
| **Texture** | Noise, tape, VHS, FX beds | 1–5 |

Do **not** sample everything at 12 velocity layers — tier controls depth.

## Content directory (production)

```text
DISKLORDZ_ROM/
├── 01_ACOUSTIC/   (Piano, EP, Organ, Guitar, Bass, Strings, Brass, Woodwinds)
├── 02_BELLS/
├── 03_MALLETS/
├── 04_BASS/       (Acoustic, Electric, Synth, 808/)
├── 05_SYNTH/
├── 06_STRINGS/
├── 07_BRASS/
├── 08_VOCAL/
├── 09_GUITAR/
├── 10_TEXTURE/
├── 11_ATMOSPHERE/
├── 12_FX/
└── 13_CYBER_SHIFT/
```

**Never** ship third-party workstation ROM dumps (Fantom/Triton/Motif/SRX/etc.). Architecture may be inspired; **audio must be original, licensed, or synthesized in-repo.**

## Multisample manifest (WAV → ROM)

Each source instrument folder contains zone WAVs + a `.manifest` file (see [FactoryContent/README.md](../FactoryContent/README.md)).

Pipeline tool:

```bash
DisklordzSynth_RomFactory build-manifest /path/to/Piano_001 /out/pcm_bank_custom.dlrrom 0
```

Procedural placeholder cards (current P0 factory):

```bash
DisklordzSynth_RomFactory build-procedural build/DisklordzRompler/generated
```

## Patch → preset fan-out

`PatchPipeline.h` defines:

- **ToneDescriptor** — ROM card + multisample program + level  
- **PatchDescriptor** — four tones  
- **PresetCatalogEntry** — preset name + patch + source multisample id  

Macros (Character, Drive, Crush, …) differentiate presets that share tones.

Example:

```text
Grand Piano multisample
  → Grand Piano, Dark Piano, VHS Piano, Lo-Fi Piano, Phonk Piano, …
```

## Factory pipeline stages (target automation)

1. Real recordings / synthesis / field texture capture  
2. Raw WAV  
3. Sample normalizer · analyzer · loop detector · root tagger · velocity analyzer  
4. Multisample builder → `.dlrrom` wave entries  
5. Tone builder → ROM programs  
6. Patch builder → 4-tone performances  
7. Macro generator · FX preset builder · metadata  
8. Preset validator → shipped plugin bank  

Agents can start from `/RawRecordings/Piano/` and emit manifest + zones + patch stubs under `/ROM/Piano/`.

## Related

- [SAMPLE_PACK.md](SAMPLE_PACK.md) — DLRROM01 on-disk format  
- [ARCHITECTURE.md](../ARCHITECTURE.md) — runtime loader  
- [DisklordzSynth/ARCHITECTURE.md](../../DisklordzSynth/ARCHITECTURE.md) — offline tools  
