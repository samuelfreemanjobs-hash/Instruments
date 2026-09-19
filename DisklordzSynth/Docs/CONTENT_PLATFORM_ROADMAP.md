# DiskLordz content platform — phased roadmap

**Agent workflow:** **DPCP** = [PLUGIN_FACTORY_OS_PROMPT.md](../../docs/PLUGIN_FACTORY_OS_PROMPT.md) **§0** (Discover → Specify → Code in vertical slices → Prove). Defined in [.cursor/rules/autonomous-builder-dpcp.mdc](../../.cursor/rules/autonomous-builder-dpcp.mdc).

Use this roadmap for ROM Factory, expansions, and ROMpler content integration. Run **§0 inside each roadmap phase** below.

**Products:** `DisklordzSynth/` = ROM Factory · `DisklordzRompler/` = ROMpler engine · do **not** edit `Wave909/` for this line.

**Platform map:** [DISKLORDZ_PLATFORM.md](../../docs/DISKLORDZ_PLATFORM.md) · **Spec index:** [CONTENT_PLATFORM_INDEX.md](CONTENT_PLATFORM_INDEX.md)

---

## Entity chain (non-negotiable)

```text
RAW AUDIO → SAMPLE ASSET (DL_SAMPLE_*) → MULTISAMPLE (DL_MULTI_*)
    → TONE (DL_TONE_*) → PATCH (DL_PATCH_*) → PRESET → PERFORMANCE
```

Presets fan out from patches; patches fan out from multisamples. **IDs, not filenames.**

---

## Phase 0 — Discover (docs + audit only)

**Goal:** Know what exists; gap vs 1,000-preset platform; no new DSP.

**Agent checklist:**

1. Read `/ARCHITECTURE.md`, `AGENTS.md`, [DISKLORDZ_PLATFORM.md](../../docs/DISKLORDZ_PLATFORM.md).
2. Audit `DisklordzSynth/` tools and `DisklordzRompler/` runtime loaders.
3. Write/update [CONTENT_PLATFORM_PHASE0_AUDIT.md](CONTENT_PLATFORM_PHASE0_AUDIT.md) (status table + next phase entry criteria).
4. Confirm legal boundary: original / licensed / procedural only — [SAMPLE_STANDARD_V1.md](SAMPLE_STANDARD_V1.md).
5. **Commit:** docs only. **Validate:** N/A (no code change).

**Exit criteria:** Audit published; Phase 1 scope agreed (or defaults documented).

---

## Phase 1 — Specify (schemas frozen)

**Goal:** Stable formats before mass content.

**Deliverables:**

- Sample DB schema locked ([SAMPLE_DATABASE_SCHEMA.md](SAMPLE_DATABASE_SCHEMA.md))
- PATCH / EXPANSION / MANIFEST formats ([PATCH_FORMAT.md](PATCH_FORMAT.md), [EXPANSION_FORMAT.md](EXPANSION_FORMAT.md))
- ROM budget v1 ([ROM_BUDGET_V1.md](ROM_BUDGET_V1.md))
- QC gates ([QC_VALIDATION.md](QC_VALIDATION.md))
- Sound Designer Agent contract ([SOUND_DESIGN_AGENT.md](SOUND_DESIGN_AGENT.md))

**Exit criteria:** `ContentIds`, record headers, example `DL_EXP_001` manifest validate via `DisklordzSynth_RomFactory validate-package`.

---

## Phase 2 — Import pipeline (ROM Factory vertical slice)

**Goal:** WAV folders → `SampleAsset` rows → multisample manifests → `.dlrrom`.

**Slices (one commit each, green `ctest -R DisklordzSynth` per slice):**

1. `import-wav` → normalized `SampleAssetRecord` (JSON/SQLite export)
2. Analyze pass (peak, DC, root — stub OK with manual tags)
3. `build-multisample` from DB → DLRROM01
4. Category ROM budget reporter (fail if over cap)

**Exit criteria:** One hero instrument path end-to-end (e.g. 8-zone procedural or real WAV set) with IDs.

---

## Phase 3 — Patch & preset factory

**Goal:** 1 multisample → N presets (macros + taxonomy).

**Slices:**

1. Patch JSON loader → `RomplerParams` + `toneRomBank` / `toneProgram`
2. Preset catalog file (`DL_PKG_FACTORY`) with taxonomy tags
3. Agent template outputs validated patch JSON only ([SOUND_DESIGN_AGENT.md](SOUND_DESIGN_AGENT.md))
4. Duplicate preset name checker

**Exit criteria:** ≥8 presets from ≥2 source multisamples; catalog schema in repo.

---

## Phase 4 — Content engine & expansions

**Goal:** Mount factory + `DL_EXP_*` without plugin rebuild.

**Slices:**

1. `ContentEngine` loads factory + expansion `rom/` (message thread)
2. Entitlement interface (stub: local allow-list file)
3. Expansion installer layout documented for future installer

**Exit criteria:** Second package mounts alongside factory in test harness.

---

## Phase 5 — Streaming (multi-GB)

**Goal:** Disk streaming when ROM > RAM budget — [STREAMING_ARCHITECTURE.md](../DisklordzRompler/Docs/STREAMING_ARCHITECTURE.md).

**Exit criteria:** Zone preload + cache policy implemented; audio thread never blocks on I/O.

---

## Phase 6 — Scale content (production)

**Goal:** Move toward 250–350 multis / 1,000+ presets per [ROM_FACTORY.md](../DisklordzRompler/Docs/ROM_FACTORY.md).

**Human/content dependencies:** recordings, licenses, genre packs.

**Exit criteria:** ROM budget report green; QC suite on full factory catalog.

---

## Phase 7 — Release platform

**Goal:** Installer, license/entitlement (non-RT), update/migration ([CONTENT_MANIFEST_FORMAT.md](CONTENT_MANIFEST_FORMAT.md)), [THIRD_PARTY_LICENSES.md](../../docs/THIRD_PARTY_LICENSES.md).

**Exit criteria:** Version migration test (engine 1.1 loads content 1.0).

---

## Per-phase validation (mandatory)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target DisklordzSynthTests DisklordzRomplerTests DisklordzRompler_VST3
ctest --test-dir build -R Disklordz
```

Report `NOT VERIFIED` for DAW matrix until run.

---

## How to invoke in Cursor

```text
Follow DPCP (PLUGIN FACTORY OS §0) for each slice.
For content-platform phase N, use DisklordzSynth/Docs/CONTENT_PLATFORM_ROADMAP.md.
Product boundary: DISKLORDZ ROMPLER + DisklordzSynth only; not WAVE-909.
```
