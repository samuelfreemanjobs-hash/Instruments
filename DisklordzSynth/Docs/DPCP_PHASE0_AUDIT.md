# DPCP Phase 0 — Discover audit

**Date:** 2026-03-21 (agent)  
**Prompt:** [docs/DPCP.md](../../docs/DPCP.md) Phase 0  
**Branch context:** `cursor/disklordz-rompler-build-9a2b`

## 1. Repository products (relevant)

| Product | Path | Role in platform |
|---------|------|------------------|
| ROM Factory | `DisklordzSynth/` | WAV → multisample → `.dlrrom`, expansions, QC tools |
| ROMpler | `DisklordzRompler/` | Real-time engine, presets, content mount |
| **Out of scope** | `Wave909/` | Separate VSTi — do not use for ROMPLER content |

## 2. Entity chain — implementation status

| Stage | Spec / code | Status |
|-------|-------------|--------|
| RAW AUDIO | Sample standard v1 | **Documented** |
| SAMPLE ASSET | `SampleAssetRecord`, `DL_SAMPLE_*` | **Schema only** — no DB/import yet |
| MULTISAMPLE | `MultisampleRecord`, `.manifest`, `MultisampleRomBuilder` | **Partial** — line manifest → ROM works |
| TONE | `ToneRecord` | **Schema only** |
| PATCH | `PatchRecord`, `PatchPipeline.h` (legacy descriptors) | **Schema + JSON spec** — no runtime loader |
| PRESET | `PresetRecord`, `FactoryPresets.cpp` | **8 hardcoded** — no catalog file |
| PERFORMANCE | `PerformanceRecord` | **Future** |
| DLRROM01 | 16× `pcm_bank_*.dlrrom`, procedural | **P0 placeholder** |
| Expansions | `DL_EXP_001` manifest + validate CLI | **Stub** |
| Content engine | `ContentEngine.cpp` | **Mount dirs** — not wired to processor yet |
| Streaming | `StreamingSampleCache.h` | **Policy stub** |
| Sound Designer Agent | `SOUND_DESIGN_AGENT.md` | **Contract only** |

## 3. PLUGIN FACTORY OS alignment

| Factory phase | ROMPLER + content line |
|---------------|-------------------------|
| Phase A Discover | **This document** |
| Phase B Specify | Largely done ([CONTENT_PLATFORM_INDEX.md](CONTENT_PLATFORM_INDEX.md)) |
| Phase C Vertical slices | ROMPLER P0 shell/engine/UI partial; Factory Phase 2+ not started |
| Phase D Validate | `ctest -R Disklordz` green; pluginval on ROMPLER as CI allows |

## 4. Gaps (ordered for DPCP Phase 1–3)

1. **No sample database** — cannot track 8k–20k WAVs or enforce IDs at scale.
2. **No `import-wav` / analyze pipeline** — manual `.manifest` only.
3. **Patch JSON not loaded by plugin** — presets not driven from catalog.
4. **Processor uses `FactoryRomLoader`**, not `ContentEngine` — expansions not in runtime path.
5. **No preset fan-out tooling** — 1,000 presets require generator + taxonomy DB.
6. **Streaming not implemented** — OK until ROM > ~1–2 GB ([ROM_BUDGET_V1.md](ROM_BUDGET_V1.md)).
7. **License/entitlement/installer** — documented only (DPCP Phase 7).

## 5. Phase 0 exit criteria

| Criterion | Met? |
|-----------|------|
| Platform docs indexed | Yes |
| Seven-system split documented | Yes |
| Legal/content boundary documented | Yes |
| Gap list + next phase entry | Yes |
| No unplanned product code in Phase 0 commit | Yes (this audit) |

## 6. Recommended Phase 1 default (if user does not override)

Proceed to **DPCP Phase 2 slice 1**: `DisklordzSynth_RomFactory import-wav` → JSON `SampleAsset` export, using [SAMPLE_STANDARD_V1.md](SAMPLE_STANDARD_V1.md) naming.

**Tier-A first instrument (proposed default):** 808/Sub family (aligns with phonk product identity, lightweight sampling strategy).

## 7. Blocking questions for product owner (optional)

None required to start Phase 2 with **procedural + synthetic** assets. Needed before **commercial hero recordings**:

1. **Content source:** all in-house capture vs licensed packs vs hybrid?
2. **v1 ROM budget:** confirm ~7 GB cap or different ship target?
3. **v1 preset count:** ship 64 / 128 / 256 first, or hold for 1,000+ internal-only until browser ready?
4. **Expansion SKUs:** which 1–2 expansion titles for first paid DLC (names only)?

If you defer answers, agents use defaults in §6.
