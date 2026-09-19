# DiskLordz content platform — spec index

**Cursor workflow:** [DPCS_BUILD_METHODOLOGY.md](../../docs/DPCS_BUILD_METHODOLOGY.md) · Content phases: [CONTENT_PLATFORM_ROADMAP.md](CONTENT_PLATFORM_ROADMAP.md) · Phase 0 audit: [CONTENT_PLATFORM_PHASE0_AUDIT.md](CONTENT_PLATFORM_PHASE0_AUDIT.md)

Build order (factory before 1,000 presets):

| # | Document | Purpose |
|---|----------|---------|
| 01 | [DISKLORDZ_ROM_SPECIFICATION.md](DISKLORDZ_ROM_SPECIFICATION.md) | Wave ROM binary + catalog |
| 02 | [SAMPLE_DATABASE_SCHEMA.md](SAMPLE_DATABASE_SCHEMA.md) | SampleAsset, Multisample, Patch records |
| 03 | [MULTISAMPLE_FORMAT.md](MULTISAMPLE_FORMAT.md) | Zones, velocity, IDs |
| 04 | [PATCH_FORMAT.md](PATCH_FORMAT.md) | 4-tone patch + macros |
| 05 | [CONTENT_MANIFEST_FORMAT.md](CONTENT_MANIFEST_FORMAT.md) | Package + schema versions |
| 06 | [EXPANSION_FORMAT.md](EXPANSION_FORMAT.md) | `DL_EXP_*` folder layout |
| 07 | [SAMPLE_STANDARD_V1.md](SAMPLE_STANDARD_V1.md) | Recording/editing/naming |
| 08 | [SAMPLE_PROCESSING_PIPELINE.md](SAMPLE_PROCESSING_PIPELINE.md) | Import → validate → ROM |
| 09 | [SOUND_DESIGN_AGENT.md](SOUND_DESIGN_AGENT.md) | AI patch JSON contract |
| 10 | [ROM_BUDGET_V1.md](ROM_BUDGET_V1.md) | Factory GB caps by category |
| — | [QC_VALIDATION.md](QC_VALIDATION.md) | Content / audio / plugin checks |
| — | [../DisklordzRompler/Docs/STREAMING_ARCHITECTURE.md](../DisklordzRompler/Docs/STREAMING_ARCHITECTURE.md) | Disk streaming (multi-GB) |
| — | [ROM_FACTORY.md](../DisklordzRompler/Docs/ROM_FACTORY.md) | Product pipeline summary |

C++ mirrors: `DisklordzSynth/include/disklordz/content/*.h`

Tools: `DisklordzSynth_RomFactory`, `DisklordzSynth_BuildRawRom`
